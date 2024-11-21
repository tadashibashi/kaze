#include "AudioSource.h"
#include <kaze/snd/AudioCommands.h>
#include <kaze/core/intrinsics.h>
#include <kaze/core/memory.h>

KSND_NS_BEGIN

// Returns if AudioSource is accessed via an invalid handle
#define HANDLE_GUARD() do { if (getError().code == Error::InvalidHandle) { \
    KAZE_PUSH_ERR(Error::InvalidHandle, "Attempted to use invalid handle in {}", KAZE_FUNCTION); \
    return; \
} } while(0)

// Returns a specific value if AudioSource is accessed via an invalid handle
#define HANDLE_GUARD_RET(ret) do { if (getError().code == Error::InvalidHandle) { \
    KAZE_PUSH_ERR(Error::InvalidHandle, "Attempted to use invalid handle in {}", KAZE_FUNCTION); \
    return (ret); \
} } while(0)

AudioSource::AudioSource(AudioSource &&other) noexcept :
    m_context(other.m_context),
    m_volume(other.m_volume), m_panner(other.m_panner),
    m_effects(other.m_effects),
    m_outBuffer(std::move(other.m_outBuffer)), m_inBuffer(std::move(other.m_inBuffer)),
    m_fadePoints(std::move(other.m_fadePoints)), m_fadeValue(other.m_fadeValue),
    m_clock(other.m_clock), m_parentClock(other.m_parentClock),
    m_paused(other.m_paused), m_pauseClock(other.m_pauseClock), m_unpauseClock(other.m_unpauseClock),
    m_releaseOnPauseClock(other.m_releaseOnPauseClock), m_shouldDiscard(other.m_shouldDiscard)
{

}

auto AudioSource::init_(AudioContext *context, const Uint64 parentClock, const Bool paused) -> Bool
{
    m_context = context;
    m_clock = 0;
    m_parentClock = parentClock;
    m_paused = paused;

    m_pauseClock = std::numeric_limits<Uint64>::max();
    m_unpauseClock = std::numeric_limits<Uint64>::max();

    m_shouldDiscard = False;
    m_fadeValue = 1.f;

    m_panner = m_context->createObjectImpl<PanEffect>();
    m_volume = m_context->createObjectImpl<VolumeEffect>();

    addEffectImpl(0, m_panner.cast<AudioEffect>());
    addEffectImpl(1, m_volume.cast<AudioEffect>());
    return True;
}

auto AudioSource::release_() -> void // callback on destruct
{
    for (auto &effect : m_effects)
    {
        m_context->releaseObjectImpl(effect);
    }
    m_effects.clear();
}

auto AudioSource::release() -> void
{
    HANDLE_GUARD();
    m_shouldDiscard = True;
    m_context->pushCommand(commands::ContextFlagRemovals {
        .context = m_context,
    });
}

/// Find starting fade point index, if there is no fade, e.g. < 2 points available, or the last fadepoint clock time
/// was surpassed, -1 is returned.
/// @param points list of points to check, must be sorted by clock time, low to high
/// @param clock  current clock point to check
/// @param outIndex [out] index to retrieve - this value is the last fadepoint that is less than `clock`, thus the
///                       first of two points to interpolate the resulting fade value.
/// @returns whether there is a next available fade at the index after `outIndex`. Since there must be a second
///          fade point to interpolate between, this also means whether to perform the fade or not.
static auto findFadePointIndex(const List<FadePoint> &points, const Uint64 clock, Int *outIndex) -> Bool
{
    Int res = -1;
    const Size size = points.size();
    for (Size i = 0; i < size; ++i)
    {
        if (points[i].clock > clock)
        {
            break;
        }

        ++res;
    }

    *outIndex = res;

    return res + 1 < size;
}

auto AudioSource::read(const Ubyte **pcmPtr, Int64 length) -> Int64
{
    if (m_inBuffer.size() != length)
    {
        m_inBuffer.resize(length, 0);
    }

    if (m_outBuffer.size() != length)
    {
        m_outBuffer.resize(length, 0);
    }

    memory::set(m_outBuffer.data(), 0, m_outBuffer.size());

    Int64 unpauseClock = (Int64)m_unpauseClock - (Int64)m_parentClock;
    Int64 pauseClock = (Int64)m_pauseClock - (Int64)m_parentClock;

    for (Int i = 0; i < length;)
    {
        if (m_paused)
        {
            // Next unpause occurs within this chunk
            if (unpauseClock < (length - i) / (2 * sizeof(Float)) && unpauseClock > -1)
            {
                i += (Int)unpauseClock;

                if (pauseClock < unpauseClock) // if pause clock comes before unpause, unset it, it's redundant
                {
                    m_pauseClock = -1;
                    pauseClock = -1;
                }

                if (pauseClock > -1)
                    pauseClock -= unpauseClock;
                m_unpauseClock = -1;
                m_paused = False;
            }
            else
            {
                break;
            }
        }
        else
        {
            // Check if there is a pause clock ahead to see how many samples to read until then
            const bool pauseThisFrame = (pauseClock < (length - i) / (2 * sizeof(Float)) && pauseClock > -1);
            const Int64 bytesToRead = pauseThisFrame ? (Int64)pauseClock * 2LL * sizeof(Float) : length - i;

            Int bytesRead = 0;
            // read bytes here
            if (bytesToRead > 0)
                bytesRead = readImpl(m_outBuffer.data() + i, bytesToRead);

            i += bytesRead;

            if (pauseThisFrame)
            {
                if (unpauseClock < pauseClock) // if unpause clock comes before pause, unset it, it's redundant
                {
                    m_unpauseClock = -1;
                    unpauseClock = -1;
                }

                m_paused = true;
                m_pauseClock = -1;

                if (m_releaseOnPauseClock)
                {
                    release();
                    break;
                }
            }

            if (pauseClock > -1)
                pauseClock -= bytesToRead / (2 * sizeof(Float)); // 2 for each channel
            if (unpauseClock > -1)
                unpauseClock -= bytesToRead / (2 * sizeof(Float)); // 2 for each channel
        }
    }

    const auto sampleCount = length / sizeof(Float);
    for (auto &effect : m_effects)
    {
        if (effect->process((Float *)m_outBuffer.data(), (Float *)m_inBuffer.data(), (Int)sampleCount))
        {
            std::swap(m_outBuffer, m_inBuffer);

            // clear inBuffer to 0
            memory::set(m_inBuffer.data(), 0, m_inBuffer.size());
        }

    }

    // Apply fade points
    Int fadeIndex = -1;
    Uint64 fadeClock = m_parentClock;

    for (auto sample = (float *)m_outBuffer.data(), end = (float *)(m_outBuffer.data() + m_outBuffer.size());
        sample < end;
        )
    {
        if (findFadePointIndex(m_fadePoints, fadeClock, &fadeIndex)) // returns whether we are currently in a fade
        {
            // Get fadepoint data
            const auto clock0 = m_fadePoints[fadeIndex].clock; // starting clock
            const auto value0 = m_fadePoints[fadeIndex].value; // starting value

            const auto clock1 = m_fadePoints[fadeIndex + 1].clock; // end clock
            const auto value1 = m_fadePoints[fadeIndex + 1].value; // end value

            // Perform fade until end or clock1, whichever comes first
            const auto fadeEnd = std::min<uint32_t>(static_cast<uint32_t>(end - sample) / 2U, clock1 + 1 - fadeClock);
            const auto clockDiff = clock1 - clock0;
            const auto valueDiff = value1 - value0;

            uint32_t f = 0;
#if KAZE_CPU_SSE
            const auto clockDiffVec = _mm_set1_ps(static_cast<float>(clockDiff));
            const auto valueDiffVec = _mm_set1_ps(valueDiff);
            const auto value0Vec = _mm_set1_ps(value0);
            for (; f <= fadeEnd - 16; f += 16)
            {
                const auto clockOffsetVec = _mm_set1_ps(
                    static_cast<float>(fadeClock) - static_cast<float>(clock0));

                const auto amounts0 = _mm_div_ps(_mm_add_ps(_mm_set_ps(1, 1, 0, 0), clockOffsetVec), clockDiffVec);
                const auto amounts1 = _mm_div_ps(_mm_add_ps(_mm_set_ps(3, 3, 2, 2), clockOffsetVec), clockDiffVec);
                const auto amounts2 = _mm_div_ps(_mm_add_ps(_mm_set_ps(5, 5, 4, 4), clockOffsetVec), clockDiffVec);
                const auto amounts3 = _mm_div_ps(_mm_add_ps(_mm_set_ps(7, 7, 6, 6), clockOffsetVec), clockDiffVec);
                const auto amounts4 = _mm_div_ps(_mm_add_ps(_mm_set_ps(9, 9, 8, 8), clockOffsetVec), clockDiffVec);
                const auto amounts5 = _mm_div_ps(_mm_add_ps(_mm_set_ps(11, 11, 10, 10), clockOffsetVec), clockDiffVec);
                const auto amounts6 = _mm_div_ps(_mm_add_ps(_mm_set_ps(13, 13, 12, 12), clockOffsetVec), clockDiffVec);
                const auto amounts7 = _mm_div_ps(_mm_add_ps(_mm_set_ps(15, 15, 14, 14), clockOffsetVec), clockDiffVec);
                const auto result0 = _mm_add_ps(_mm_mul_ps(valueDiffVec, amounts0), value0Vec);
                const auto result1 = _mm_add_ps(_mm_mul_ps(valueDiffVec, amounts1), value0Vec);
                const auto result2 = _mm_add_ps(_mm_mul_ps(valueDiffVec, amounts2), value0Vec);
                const auto result3 = _mm_add_ps(_mm_mul_ps(valueDiffVec, amounts3), value0Vec);
                const auto result4 = _mm_add_ps(_mm_mul_ps(valueDiffVec, amounts4), value0Vec);
                const auto result5 = _mm_add_ps(_mm_mul_ps(valueDiffVec, amounts5), value0Vec);
                const auto result6 = _mm_add_ps(_mm_mul_ps(valueDiffVec, amounts6), value0Vec);
                const auto result7 = _mm_add_ps(_mm_mul_ps(valueDiffVec, amounts7), value0Vec);
                _mm_store_ps(sample, _mm_mul_ps(_mm_load_ps(sample), result0));
                _mm_store_ps(sample + 4, _mm_mul_ps(_mm_load_ps(sample + 4), result1));
                _mm_store_ps(sample + 8, _mm_mul_ps(_mm_load_ps(sample + 8), result2));
                _mm_store_ps(sample + 12, _mm_mul_ps(_mm_load_ps(sample + 12), result3));
                _mm_store_ps(sample + 16, _mm_mul_ps(_mm_load_ps(sample + 16), result4));
                _mm_store_ps(sample + 20, _mm_mul_ps(_mm_load_ps(sample + 20), result5));
                _mm_store_ps(sample + 24, _mm_mul_ps(_mm_load_ps(sample + 24), result6));
                _mm_store_ps(sample + 28, _mm_mul_ps(_mm_load_ps(sample + 28), result7));

                fadeClock += 16;
                sample += 32;
            }
#elif KAZE_CPU_WASM_SIMD
            const auto clockDiffVec = wasm_f32x4_splat(static_cast<float>(clockDiff));
            const auto valueDiffVec = wasm_f32x4_splat(valueDiff);
            const auto value0Vec = wasm_f32x4_splat(value0);
            for (; f <= fadeEnd - 16; f += 16)
            {
                const auto clockOffsetVec = wasm_f32x4_splat(static_cast<float>(fadeClock) - static_cast<float>(clock0));
                const auto amounts0 = wasm_f32x4_div(wasm_f32x4_add(wasm_f32x4_make(0, 0, 1, 1), clockOffsetVec), clockDiffVec);
                const auto amounts1 = wasm_f32x4_div(wasm_f32x4_add(wasm_f32x4_make(2, 2, 3, 3), clockOffsetVec), clockDiffVec);
                const auto amounts2 = wasm_f32x4_div(wasm_f32x4_add(wasm_f32x4_make(4, 4, 5, 5), clockOffsetVec), clockDiffVec);
                const auto amounts3 = wasm_f32x4_div(wasm_f32x4_add(wasm_f32x4_make(6, 6, 7, 7), clockOffsetVec), clockDiffVec);
                const auto amounts4 = wasm_f32x4_div(wasm_f32x4_add(wasm_f32x4_make(8, 8, 9, 9), clockOffsetVec), clockDiffVec);
                const auto amounts5 = wasm_f32x4_div(wasm_f32x4_add(wasm_f32x4_make(10, 10, 11, 11), clockOffsetVec), clockDiffVec);
                const auto amounts6 = wasm_f32x4_div(wasm_f32x4_add(wasm_f32x4_make(12, 12, 13, 13), clockOffsetVec), clockDiffVec);
                const auto amounts7 = wasm_f32x4_div(wasm_f32x4_add(wasm_f32x4_make(14, 14, 15, 15), clockOffsetVec), clockDiffVec);

                const auto results0 = wasm_f32x4_add(wasm_f32x4_mul(valueDiffVec, amounts0), value0Vec);
                const auto results1 = wasm_f32x4_add(wasm_f32x4_mul(valueDiffVec, amounts1), value0Vec);
                const auto results2 = wasm_f32x4_add(wasm_f32x4_mul(valueDiffVec, amounts2), value0Vec);
                const auto results3 = wasm_f32x4_add(wasm_f32x4_mul(valueDiffVec, amounts3), value0Vec);
                const auto results4 = wasm_f32x4_add(wasm_f32x4_mul(valueDiffVec, amounts4), value0Vec);
                const auto results5 = wasm_f32x4_add(wasm_f32x4_mul(valueDiffVec, amounts5), value0Vec);
                const auto results6 = wasm_f32x4_add(wasm_f32x4_mul(valueDiffVec, amounts6), value0Vec);
                const auto results7 = wasm_f32x4_add(wasm_f32x4_mul(valueDiffVec, amounts7), value0Vec);

                wasm_v128_store(sample, wasm_f32x4_mul(wasm_v128_load(sample), results0));
                wasm_v128_store(sample + 4, wasm_f32x4_mul(wasm_v128_load(sample + 4), results1));
                wasm_v128_store(sample + 8, wasm_f32x4_mul(wasm_v128_load(sample + 8), results2));
                wasm_v128_store(sample + 12, wasm_f32x4_mul(wasm_v128_load(sample + 12), results3));
                wasm_v128_store(sample + 16, wasm_f32x4_mul(wasm_v128_load(sample + 16), results4));
                wasm_v128_store(sample + 20, wasm_f32x4_mul(wasm_v128_load(sample + 20), results5));
                wasm_v128_store(sample + 24, wasm_f32x4_mul(wasm_v128_load(sample + 24), results6));
                wasm_v128_store(sample + 28, wasm_f32x4_mul(wasm_v128_load(sample + 28), results7));

                fadeClock += 16;
                sample += 32;
            }
#elif KAZE_CPU_ARM_NEON
            const auto clockDiffVec = vdupq_n_f32(static_cast<float>(clockDiff));
            const auto valueDiffVec = vdupq_n_f32(static_cast<float>(valueDiff));
            const auto value0Vec = vdupq_n_f32(static_cast<float>(value0));
            for (; f < fadeEnd - 16; f += 16)
            {
                const auto clockOffsetVec = vdupq_n_f32(static_cast<float>(fadeClock) - static_cast<float>(clock0));
                const auto amounts0 = vdivq_f32(vaddq_f32(float32x4_t{0, 0, 1, 1}, clockOffsetVec), clockDiffVec);
                const auto amounts1 = vdivq_f32(vaddq_f32(float32x4_t{2, 2, 3, 3}, clockOffsetVec), clockDiffVec);
                const auto amounts2 = vdivq_f32(vaddq_f32(float32x4_t{4, 4, 5, 5}, clockOffsetVec), clockDiffVec);
                const auto amounts3 = vdivq_f32(vaddq_f32(float32x4_t{6, 6, 7, 7}, clockOffsetVec), clockDiffVec);
                const auto amounts4 = vdivq_f32(vaddq_f32(float32x4_t{8, 8, 9, 9}, clockOffsetVec), clockDiffVec);
                const auto amounts5 = vdivq_f32(vaddq_f32(float32x4_t{10, 10, 11, 11}, clockOffsetVec), clockDiffVec);
                const auto amounts6 = vdivq_f32(vaddq_f32(float32x4_t{12, 12, 13, 13}, clockOffsetVec), clockDiffVec);
                const auto amounts7 = vdivq_f32(vaddq_f32(float32x4_t{14, 14, 15, 15}, clockOffsetVec), clockDiffVec);

                const auto results0 = vaddq_f32(vmulq_f32(valueDiffVec, amounts0), value0Vec);
                const auto results1 = vaddq_f32(vmulq_f32(valueDiffVec, amounts1), value0Vec);
                const auto results2 = vaddq_f32(vmulq_f32(valueDiffVec, amounts2), value0Vec);
                const auto results3 = vaddq_f32(vmulq_f32(valueDiffVec, amounts3), value0Vec);
                const auto results4 = vaddq_f32(vmulq_f32(valueDiffVec, amounts4), value0Vec);
                const auto results5 = vaddq_f32(vmulq_f32(valueDiffVec, amounts5), value0Vec);
                const auto results6 = vaddq_f32(vmulq_f32(valueDiffVec, amounts6), value0Vec);
                const auto results7 = vaddq_f32(vmulq_f32(valueDiffVec, amounts7), value0Vec);

                vst1q_f32(sample, vmulq_f32(vld1q_f32(sample), results0));
                vst1q_f32(sample + 4, vmulq_f32(vld1q_f32(sample + 4), results1));
                vst1q_f32(sample + 8, vmulq_f32(vld1q_f32(sample + 8), results2));
                vst1q_f32(sample + 12, vmulq_f32(vld1q_f32(sample + 12), results3));
                vst1q_f32(sample + 16, vmulq_f32(vld1q_f32(sample + 16), results4));
                vst1q_f32(sample + 20, vmulq_f32(vld1q_f32(sample + 20), results5));
                vst1q_f32(sample + 24, vmulq_f32(vld1q_f32(sample + 24), results6));
                vst1q_f32(sample + 28, vmulq_f32(vld1q_f32(sample + 28), results7));

                sample += 32;
                fadeClock += 16;
            }
#else
            for (; f <= fadeEnd - 4; f += 4)
            {
                const auto clockOffset = fadeClock - clock0; // current offset from clock0
                const float amount0 = (float)(clockOffset) / (float)(clockDiff);
                const float amount1 = (float)(clockOffset + 1) / (float)(clockDiff);
                const float amount2 = (float)(clockOffset + 2) / (float)(clockDiff);
                const float amount3 = (float)(clockOffset + 3) / (float)(clockDiff);
                const auto result0 = valueDiff * amount0 + value0;
                const auto result1 = valueDiff * amount1 + value0;
                const auto result2 = valueDiff * amount2 + value0;
                const auto result3 = valueDiff * amount3 + value0;
                sample[0] *= result0;
                sample[1] *= result0;
                sample[2] *= result1;
                sample[3] *= result1;
                sample[4] *= result2;
                sample[5] *= result2;
                sample[6] *= result3;
                sample[7] *= result3;

                fadeClock += 4;
                sample += 8;
            }
#endif
            for (;f < fadeEnd; ++f)
            {
                const float amount = (float)(fadeClock - clock0) / (float)(clockDiff);
                const auto result = valueDiff * amount + value0;
                *sample++ *= result;
                *sample++ *= result;
                ++fadeClock;
            }

            m_fadeValue = value1;
        }
        else
        {
            // find end point
            Uint64 endIndex;
            if (fadeIndex + 1 < m_fadePoints.size()) // there's next fadepoint
            {
                endIndex = std::min<Uint64>(m_fadePoints[fadeIndex + 1].clock - fadeClock, static_cast<Uint64>(end - sample));
            }
            else
            {
                endIndex = static_cast<Uint64>(end - sample);
            }

            if (m_fadeValue == 1.f) // fade of 1 has no effect, just move the sample ptr
                sample += endIndex;
            else                    // perform fade multiplier on all samples until endIndex
            {
                for (Uint64 i = 0; i < endIndex; ++i)
                    *sample++ *= m_fadeValue;
            }
        }

    }

    // Remove all fade points that have been passed
    if (fadeIndex > 0)
        m_fadePoints.erase(m_fadePoints.begin(), m_fadePoints.begin() + (fadeIndex - 1));

    if (pcmPtr)
        *pcmPtr = m_outBuffer.data();

    m_clock += length / (2 * sizeof(Float));
    return length;
}

auto AudioSource::pauseAt(Uint64 clock, Bool releaseOnPause) -> Bool
{
    HANDLE_GUARD_RET(False);
    m_context->pushImmediateCommand(
        commands::SourceSetPause{
            .source = this,
            .releaseOnPause = releaseOnPause,
            .clock = clock
        }
    );

    return True;
}

auto AudioSource::unpauseAt(Uint64 clock) -> Bool
{
    HANDLE_GUARD_RET(False);
    m_context->pushImmediateCommand(
        commands::SourceSetUnpause {
            .source = this,
            .clock = clock
        }
    );

    return True;
}

auto AudioSource::setPaused(Bool pause) -> Bool
{
    HANDLE_GUARD_RET(False);
    return pause ? pauseAt(std::numeric_limits<Uint64>::max()) :
        unpauseAt(std::numeric_limits<Uint64>::max());
}

auto AudioSource::isPaused() const -> Bool
{
    HANDLE_GUARD_RET(False);
    return m_paused;
}

auto AudioSource::removeEffect(const Handle<AudioEffect> &effect) -> Bool
{
    HANDLE_GUARD_RET(False);

    if ( !effect.isValid() )
    {
        KAZE_PUSH_ERR(Error::InvalidHandle, "`effect` passed to `AudioSource::removeEFfect` "
            "is an invalid handle");
        return False;
    }

    m_context->pushCommand(
        commands::SourceRemoveEffect {
            .source = this,
            .effect = effect
        }
    );

    return True;
}

auto AudioSource::getEffect(Size position) -> Handle<AudioEffect>
{
    HANDLE_GUARD_RET(Handle<AudioEffect>{});

    if (position >= m_effects.size())
    {
        KAZE_PUSH_ERR(Error::OutOfRange,
            "AudioSource::getEffect: `position` is out of range; must be "
            "< {}, but got {}", m_effects.size(), position);
        return {};
    }

    return m_effects[position];
}

auto AudioSource::getEffect(Size position) const -> Handle<const AudioEffect>
{
    HANDLE_GUARD_RET(Handle<const AudioEffect>{});

    if (position >= m_effects.size())
    {
        KAZE_PUSH_ERR(Error::OutOfRange,
            "AudioSource::getEffect: `position` is out of range; must be "
            "< {}, but got {}", m_effects.size(), position);
        return {};
    }

    return (Handle<const AudioEffect>)m_effects[position];
}

auto AudioSource::getEffectCount() const -> Size
{
    HANDLE_GUARD_RET(0);

    return m_effects.size();
}

auto AudioSource::getVolume() const -> Float
{
    return m_volume->volume();
}

auto AudioSource::setVolume(Float value) -> void
{
    m_volume->volume(value);
}


auto AudioSource::getClock() const -> Uint64
{
    HANDLE_GUARD_RET(0);

    return m_clock;
}


auto AudioSource::getParentClock() const -> Uint64
{
    HANDLE_GUARD_RET(0);

    return m_parentClock;
}

auto AudioSource::addFadePoint(Uint64 clock, Float value) -> Bool
{
    HANDLE_GUARD_RET(False);

    m_context->pushImmediateCommand(
        commands::SourceAddFadePoint {
            .source = this,
            .clock = clock,
            .value = value,
        }
    );

    return True;
}

auto AudioSource::fadeTo(Uint64 clock, Float value) -> Bool
{
    HANDLE_GUARD_RET(False);

    m_context->pushImmediateCommand(
        commands::SourceFadeTo {
            .source = this,
            .clock = clock,
            .value = value,
        }
    );

    return True;
}

auto AudioSource::removeFadePoints(Uint64 clockBeginPoint, Uint64 clockEndPoint) -> Bool
{
    HANDLE_GUARD_RET(False);

    m_context->pushImmediateCommand(
        commands::SourceRemoveFadePoint {
            .source = this,
            .beginClock = clockBeginPoint,
            .endClock = clockEndPoint,
        }
    );

    return True;
}

auto AudioSource::getFadeValue() const -> Float
{
    HANDLE_GUARD_RET(-1.f);
    return m_fadeValue;
}

auto AudioSource::updateParentClock(const Uint64 parentClock) -> Bool
{
    HANDLE_GUARD_RET(False);
    m_parentClock = parentClock;
    return True;
}

auto AudioSource::swapBuffers(AlignedList<Ubyte , 16> *buffer) -> void
{
    HANDLE_GUARD();
    m_outBuffer.swap(*buffer);
}

// ===== Command implementations ==============================================

auto AudioSource::setPauseImpl(Bool pause, Uint64 clock, Bool releaseOnPause) -> void
{
    if (pause)
    {
        if (clock == std::numeric_limits<Uint64>::max() || (clock > 0 && clock < m_parentClock))
        {
            // command took too long to send, activate at the next clock buffer
            m_pauseClock = m_parentClock;
        }
        else
        {
            m_pauseClock = clock;
        }

        m_releaseOnPauseClock = releaseOnPause;
    }
    else
    {
        if (clock == std::numeric_limits<Uint64>::max() || (clock > 0 && clock < m_parentClock))
        {
            // command took too long to send, activate at the next clock buffer
            m_unpauseClock = m_parentClock;
        }
        else
        {
            m_unpauseClock = clock;
        }
    }
}

auto AudioSource::addEffectImpl(Int insertIndex, const Handle<AudioEffect> &effect) -> void
{
    const auto it = m_effects.begin() + insertIndex;
    m_effects.insert(it, effect);
}

auto AudioSource::removeEffectImpl(const Handle<AudioEffect> &effect) -> void
{
    for (auto it = m_effects.begin(); it != m_effects.end(); ++it)
    {
        if (*it == effect)
        {
            m_effects.erase(it);
            break;
        }
    }
}

auto AudioSource::addFadePointImpl(Uint64 clock, Float value) -> void
{
    Bool didInsert = False;
    for (auto it = m_fadePoints.begin(); it != m_fadePoints.end(); ++it)
    {
        if (clock == it->clock) // If same, replace the value
        {
            it->value = value;
            didInsert = True;
            break;
        }

        if (clock < it->clock)
        {
            m_fadePoints.insert(it, FadePoint(clock, value));
            didInsert = True;
            break;
        }
    }

    if ( !didInsert )
    {
        m_fadePoints.emplace_back(clock, value);
    }
}

auto AudioSource::removeFadePointImpl(Uint64 clockBegin, Uint64 clockEnd) -> void
{
    // remove-erase idiom on all fadepoints between clock values
    m_fadePoints.erase(
        std::remove_if(m_fadePoints.begin(), m_fadePoints.end(), [clockBegin, clockEnd](const FadePoint &point) {
            return point.clock >= clockBegin && point.clock < clockEnd;
        }),
        m_fadePoints.end());
}

auto AudioSource::fadeToImpl(Uint clock, Float value) -> void
{
    // Remove any fade point between now and the fade value
    removeFadePointImpl(m_parentClock, clock);

    // Set fade point ramp
    addFadePointImpl(m_parentClock, m_fadeValue); // from now
    addFadePointImpl(clock, value);               // to target
}

KSND_NS_END
