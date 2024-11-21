#include "DelayEffect.h"
#include <kaze/core/intrinsics.h>
#include <kaze/core/memory.h>

KSND_NS_BEGIN

DelayEffect::DelayEffect() :
    m_delayTime(48000), m_feedback(), m_wet(.5f), m_delayHead(0)
{
}

DelayEffect::DelayEffect(DelayEffect &&other) noexcept :
    AudioEffect(std::move(other)),
    m_delayTime(other.m_delayTime), m_feedback(other.m_feedback), m_wet(other.m_wet),
    m_delayHead(other.m_delayHead), m_buffer(std::move(other.m_buffer))
{
}

auto DelayEffect::init_(Uint64 delayTime, Float wet, Float feedback) -> Bool
{
    if (delayTime < 256) // minimum size = number of samples per WebAudio frame
        delayTime = 256;

    m_delayTime = delayTime;
    m_wet = wet;
    m_feedback = feedback;

    m_buffer.resize(delayTime * 2);
    memory::set(m_buffer.data(), 0, m_buffer.size() * sizeof(Float));

    return True;
}

auto DelayEffect::process(const Float *input, Float *output, const Int64 count) -> Bool
{
    const auto bufSize = m_buffer.size();
    const auto dry = 1.f - m_wet;
    const auto wet = m_wet;
    const auto feedback = m_feedback;

    for (Int processed = 0; processed < count;)
    {
        const auto delayHead = m_delayHead; ///< current delay head index in buffer
        const auto readThisFrame = std::min<size_t>(count - processed, bufSize - delayHead); ///< number of samples to process this call
        Int i = 0; ///< counter
        const auto end = static_cast<Int>(readThisFrame); ///< end buffer sample index to read (exclusive)

        auto bufferData = m_buffer.data(); ///< cached ptr to buffer's lowest address

#if   KAZE_CPU_SSE
        const auto dryVec = _mm_set1_ps(dry);
        const auto wetVec = _mm_set1_ps(wet);
        const auto feedbackVec = _mm_set1_ps(feedback);

        for (; i <= end - 16; i += 16)
        {
            const auto ioFrame = i + processed;
            const auto bufferVec0 = _mm_load_ps(bufferData + delayHead + i);
            const auto bufferVec1 = _mm_load_ps(bufferData + delayHead + i + 4);
            const auto bufferVec2 = _mm_load_ps(bufferData + delayHead + i + 8);
            const auto bufferVec3 = _mm_load_ps(bufferData + delayHead + i + 12);

            const auto inputVec0 = _mm_load_ps(input + ioFrame);
            const auto inputVec1 = _mm_load_ps(input + ioFrame + 4);
            const auto inputVec2 = _mm_load_ps(input + ioFrame + 8);
            const auto inputVec3 = _mm_load_ps(input + ioFrame + 12);

            const auto result0 = _mm_add_ps(_mm_mul_ps(inputVec0, dryVec), _mm_mul_ps(bufferVec0, wetVec));
            const auto result1 = _mm_add_ps(_mm_mul_ps(inputVec1, dryVec), _mm_mul_ps(bufferVec1, wetVec));
            const auto result2 = _mm_add_ps(_mm_mul_ps(inputVec2, dryVec), _mm_mul_ps(bufferVec2, wetVec));
            const auto result3 = _mm_add_ps(_mm_mul_ps(inputVec3, dryVec), _mm_mul_ps(bufferVec3, wetVec));

            _mm_store_ps(output + ioFrame, result0);
            _mm_store_ps(output + ioFrame + 4, result1);
            _mm_store_ps(output + ioFrame + 8, result2);
            _mm_store_ps(output + ioFrame + 12, result3);

            const auto bufferResult0 = _mm_mul_ps(inputVec0, feedbackVec);
            const auto bufferResult1 = _mm_mul_ps(inputVec1, feedbackVec);
            const auto bufferResult2 = _mm_mul_ps(inputVec2, feedbackVec);
            const auto bufferResult3 = _mm_mul_ps(inputVec3, feedbackVec);

            _mm_store_ps(bufferData + delayHead + i, bufferResult0);
            _mm_store_ps(bufferData + delayHead + i + 4, bufferResult1);
            _mm_store_ps(bufferData + delayHead + i + 8, bufferResult2);
            _mm_store_ps(bufferData + delayHead + i + 12, bufferResult3);
        }
#elif KAZE_CPU_WASM_SIMD
        const auto dryVec = wasm_f32x4_splat(dry);
        const auto wetVec = wasm_f32x4_splat(wet);
        const auto feedbackVec = wasm_f32x4_splat(feedback);

        for (; i <= end - 16; i += 16)
        {
            const auto ioFrame = i + processed;
            const auto bufferVec0 = wasm_v128_load(bufferData + delayHead + i);
            const auto bufferVec1 = wasm_v128_load(bufferData + delayHead + i + 4);
            const auto bufferVec2 = wasm_v128_load(bufferData + delayHead + i + 8);
            const auto bufferVec3 = wasm_v128_load(bufferData + delayHead + i + 12);

            const auto inputVec0 = wasm_v128_load(input + ioFrame);
            const auto inputVec1 = wasm_v128_load(input + ioFrame + 4);
            const auto inputVec2 = wasm_v128_load(input + ioFrame + 8);
            const auto inputVec3 = wasm_v128_load(input + ioFrame + 12);

            const auto result0 = wasm_f32x4_add(wasm_f32x4_mul(inputVec0, dryVec), wasm_f32x4_mul(bufferVec0, wetVec));
            const auto result1 = wasm_f32x4_add(wasm_f32x4_mul(inputVec1, dryVec), wasm_f32x4_mul(bufferVec1, wetVec));
            const auto result2 = wasm_f32x4_add(wasm_f32x4_mul(inputVec2, dryVec), wasm_f32x4_mul(bufferVec2, wetVec));
            const auto result3 = wasm_f32x4_add(wasm_f32x4_mul(inputVec3, dryVec), wasm_f32x4_mul(bufferVec3, wetVec));

            wasm_v128_store(output + ioFrame, result0);
            wasm_v128_store(output + ioFrame + 4, result1);
            wasm_v128_store(output + ioFrame + 8, result2);
            wasm_v128_store(output + ioFrame + 12, result3);

            const auto bufferResult0 = wasm_f32x4_mul(inputVec0, feedbackVec);
            const auto bufferResult1 = wasm_f32x4_mul(inputVec1, feedbackVec);
            const auto bufferResult2 = wasm_f32x4_mul(inputVec2, feedbackVec);
            const auto bufferResult3 = wasm_f32x4_mul(inputVec3, feedbackVec);

            wasm_v128_store(bufferData + delayHead + i, bufferResult0);
            wasm_v128_store(bufferData + delayHead + i + 4, bufferResult1);
            wasm_v128_store(bufferData + delayHead + i + 8, bufferResult2);
            wasm_v128_store(bufferData + delayHead + i + 12, bufferResult3);
        }
#elif KAZE_CPU_ARM_NEON
        const auto dryVec = vdupq_n_f32(dry);
        const auto wetVec = vdupq_n_f32(wet);
        const auto feedbackVec = vdupq_n_f32(feedback);

        for (; i <= end - 16; i += 16)
        {
            const auto ioFrame = i + processed;
            const auto bufferVec0 = vld1q_f32(bufferData + delayHead + i);
            const auto bufferVec1 = vld1q_f32(bufferData + delayHead + i + 4);
            const auto bufferVec2 = vld1q_f32(bufferData + delayHead + i + 8);
            const auto bufferVec3 = vld1q_f32(bufferData + delayHead + i + 12);

            const auto inputVec0 = vld1q_f32(input + ioFrame);
            const auto inputVec1 = vld1q_f32(input + ioFrame + 4);
            const auto inputVec2 = vld1q_f32(input + ioFrame + 8);
            const auto inputVec3 = vld1q_f32(input + ioFrame + 12);

            const auto result0 = vmlaq_f32(vmulq_f32(inputVec0, dryVec), bufferVec0, wetVec);
            const auto result1 = vmlaq_f32(vmulq_f32(inputVec1, dryVec), bufferVec1, wetVec);
            const auto result2 = vmlaq_f32(vmulq_f32(inputVec2, dryVec), bufferVec2, wetVec);
            const auto result3 = vmlaq_f32(vmulq_f32(inputVec3, dryVec), bufferVec3, wetVec);

            vst1q_f32(output + ioFrame, result0);
            vst1q_f32(output + ioFrame + 4, result1);
            vst1q_f32(output + ioFrame + 8, result2);
            vst1q_f32(output + ioFrame + 12, result3);

            const auto bufferResult0 = vmulq_f32(inputVec0, feedbackVec);
            const auto bufferResult1 = vmulq_f32(inputVec1, feedbackVec);
            const auto bufferResult2 = vmulq_f32(inputVec2, feedbackVec);
            const auto bufferResult3 = vmulq_f32(inputVec3, feedbackVec);

            vst1q_f32(bufferData + delayHead + i, bufferResult0);
            vst1q_f32(bufferData + delayHead + i + 4, bufferResult1);
            vst1q_f32(bufferData + delayHead + i + 8, bufferResult2);
            vst1q_f32(bufferData + delayHead + i + 12, bufferResult3);
        }
#else
        for (;i <= end - 4; i += 4)
        {
            const auto ioFrame = i + processed;
            output[ioFrame] = input[ioFrame] * dry + bufferData[(delayHead + i)] * wet;
            output[ioFrame + 1] = input[ioFrame + 1] * dry + bufferData[(delayHead + i + 1)] * wet;
            output[ioFrame + 2] = input[ioFrame + 2] * dry + bufferData[(delayHead + i + 2)] * wet;
            output[ioFrame + 3] = input[ioFrame + 3] * dry + bufferData[(delayHead + i + 3)] * wet;

            bufferData[(delayHead + i)] = input[ioFrame] * feedback;
            bufferData[(delayHead + i + 1)] = input[ioFrame+1] * feedback;
            bufferData[(delayHead + i + 2)] = input[ioFrame+2] * feedback;
            bufferData[(delayHead + i + 3)] = input[ioFrame+3] * feedback;
        }
#endif
        for (; i < end; ++i) // catch leftovers, if any
        {
            const auto ioFrame = i + processed;
            output[ioFrame] = input[ioFrame] * dry + bufferData[(delayHead + i)] * wet;
            bufferData[(delayHead + i)] = input[ioFrame] * feedback;
        }

        processed += (int)readThisFrame;

        if (bufSize > 0)
            m_delayHead = (delayHead + readThisFrame) % bufSize;
        else
            m_delayHead = 0;
    }

    return true;
}

auto DelayEffect::delayTime(Uint64 samples) -> void
{
    sendParam(DelayTime, samples); // TODO: enforce a maximum?
}

auto DelayEffect::feedback(const Float value) -> void
{
    sendParam(Feedback, value);
}

auto DelayEffect::wetDry(Float value) -> void
{
    sendParam(Wet, value);
}

auto DelayEffect::receiveParam(const Int index, const Param &value) -> void
{
    switch(index)
    {
    case Feedback:
        {
            KAZE_ASSERT(value.type() == Param::Float);
            m_feedback = value.get<Float>();
        } break;

    case Wet:
        {
            KAZE_ASSERT(value.type() == Param::Float);
            m_wet = value.get<Float>();
        } break;

    case DelayTime:
        {
            KAZE_ASSERT(value.type() == Param::Uint64);
            m_delayTime = value.get<Uint64>();
        } break;

        default:
        {
            KAZE_PUSH_ERR(Error::InvalidArgErr, "Unknown parameter index passed to DelayEffect::receiveParam");
        } break;
    }
}

KSND_NS_END
