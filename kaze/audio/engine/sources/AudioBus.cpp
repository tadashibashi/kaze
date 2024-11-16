#include "AudioBus.h"
#include <kaze/core/intrinsics.h>

KAUDIO_NS_BEGIN

AudioBus::AudioBus(AudioBus &&other) noexcept :
    AudioSource(std::move(other)),
    m_sources(std::move(other.m_sources)),
    m_buffer(std::move(other.m_buffer)),
    m_parent(other.m_parent),
    m_isMaster(other.m_isMaster)
{
    other.m_sources.clear();
    other.m_parent = {};
    other.m_isMaster = False;
}

auto AudioBus::init_(AudioContext *context, const Handle<AudioBus> &parent, Bool paused) -> Bool
{
    if ( !AudioSource::init_(context, context && parent && parent.isValid() ? parent->getClock() : 0, paused) )
        return False;

    m_parent = parent;
    m_isMaster = !parent;
    return True;
}

auto AudioBus::release(const Bool recursive) -> void
{
    KAZE_HANDLE_GUARD();

    if (m_isMaster)
    {
        KAZE_PUSH_ERR(Error::LogicErr, "Cannot release master bus");
        return;
    }

    if (recursive)
    {
        // Recursively release all sources
        for (auto &handle : m_sources)
        {
            if ( !handle.isValid() )
                continue;

            const auto source = handle.get();
            if (const auto bus = dynamic_cast<AudioBus *>(source))
            {
                bus->release(True);
            }
            else
            {
                source->release();
            }
        }
    }
    else
    {
        const auto masterBus = context()->getMasterBus();
        if ( !masterBus.isValid() )
        {
            // Most likely, the context was closed or uninit state
            KAZE_PUSH_ERR(Error::RuntimeErr, "Failed to get master bus from AudioContext");
            return;
        }

        // Reconnect all children to the master bus
        for (auto &handle : m_sources)
        {
            if ( !handle.isValid() )
                continue;

            AudioBus::connect(masterBus, handle);
        }
    }

    AudioSource::release();
}

auto AudioBus::release_() -> void
{
    m_sources.clear();
    m_parent = {};
    AudioSource::release_();
}

auto AudioBus::connect(const Handle<AudioBus> &bus, const Handle<AudioSource> &source) -> Bool
{
    if ( !bus.isValid() )
    {
        KAZE_PUSH_ERR(Error::InvalidHandle,
            "AudioBus::connect: parameter `bus` was an invalid handle");
        return False;
    }

    if ( !source.isValid() )
    {
        KAZE_PUSH_ERR(Error::InvalidHandle,
            "AudioBus::connect: parameter `source` was an invalid handle");
        return False;
    }

    bus->context()->pushCommand(commands::BusConnectSource {
        .bus = bus,
        .source = source,
    });
    return True;
}

auto AudioBus::disconnect(const Handle<AudioBus> &bus, const Handle<AudioSource> &source) -> Bool
{
    if ( !bus.isValid() )
    {
        KAZE_PUSH_ERR(Error::InvalidHandle,
            "AudioBus::connect: parameter `bus` was an invalid handle");
        return False;
    }

    if ( !source.isValid() )
    {
        KAZE_PUSH_ERR(Error::InvalidHandle,
            "AudioBus::connect: parameter `source` was an invalid handle");
        return False;
    }

    bus->context()->pushCommand(commands::BusDisconnectSource {
        .bus = bus,
        .source = source,
    });

    return True;
}

auto AudioBus::readImpl(Ubyte *output, Int64 length) -> Int64
{
    // calculate mix
    Int sourcei = 0;
    for (const Int sourcemax = static_cast<Int>(m_sources.size()) - 4; sourcei <= sourcemax; sourcei += 4)
    {
        // Read data from sources (unrolled).
        // note: these should be guaranteed valid because invalidation won't take place until deferred commands
        const auto &sourceA = m_sources[sourcei].get();
        const auto &sourceB = m_sources[sourcei + 1].get();
        const auto &sourceC = m_sources[sourcei + 2].get();
        const auto &sourceD = m_sources[sourcei + 3].get();

        const Float *dataA, *dataB, *dataC, *dataD;
        sourceA->read(reinterpret_cast<const uint8_t **>(&dataA), length);
        sourceB->read(reinterpret_cast<const uint8_t **>(&dataB), length);
        sourceC->read(reinterpret_cast<const uint8_t **>(&dataC), length);
        sourceD->read(reinterpret_cast<const uint8_t **>(&dataD), length);

        // Sum each source together with output
        const auto sampleLength =static_cast<int>(length / sizeof(float));
        auto head = reinterpret_cast<float *>(output);
        Int i = 0;
#if KAZE_CPU_SSE
        for (; i <= sampleLength - 16; i += 16)
        {
            const auto a0 = _mm_load_ps(dataA + i);
            const auto a1 = _mm_load_ps(dataA + i + 4);
            const auto a2 = _mm_load_ps(dataA + i + 8);
            const auto a3 = _mm_load_ps(dataA + i + 12);
            const auto b0 = _mm_load_ps(dataB + i);
            const auto b1 = _mm_load_ps(dataB + i + 4);
            const auto b2 = _mm_load_ps(dataB + i + 8);
            const auto b3 = _mm_load_ps(dataB + i + 12);
            const auto c0 = _mm_load_ps(dataC + i);
            const auto c1 = _mm_load_ps(dataC + i + 4);
            const auto c2 = _mm_load_ps(dataC + i + 8);
            const auto c3 = _mm_load_ps(dataC + i + 12);
            const auto d0 = _mm_load_ps(dataD + i);
            const auto d1 = _mm_load_ps(dataD + i + 4);
            const auto d2 = _mm_load_ps(dataD + i + 8);
            const auto d3 = _mm_load_ps(dataD + i + 12);

            const auto sample0 = _mm_add_ps(_mm_add_ps(a0, b0), _mm_add_ps(c0, d0));
            const auto sample1 = _mm_add_ps(_mm_add_ps(a1, b1), _mm_add_ps(c1, d1));
            const auto sample2 = _mm_add_ps(_mm_add_ps(a2, b2), _mm_add_ps(c2, d2));
            const auto sample3 = _mm_add_ps(_mm_add_ps(a3, b3), _mm_add_ps(c3, d3));

            const auto head0 = _mm_load_ps(head + i);
            const auto head1 = _mm_load_ps(head + i + 4);
            const auto head2 = _mm_load_ps(head + i + 8);
            const auto head3 = _mm_load_ps(head + i + 12);

            _mm_store_ps(head + i, _mm_add_ps(head0, sample0));
            _mm_store_ps(head + i + 4, _mm_add_ps(head1, sample1));
            _mm_store_ps(head + i + 8, _mm_add_ps(head2, sample2));
            _mm_store_ps(head + i + 12, _mm_add_ps(head3, sample3));
        }
#elif KAZE_CPU_WASM_SIMD
        for (; i <= sampleLength - 16; i += 16)
        {
            const auto a0 = wasm_v128_load(dataA + i);
            const auto a1 = wasm_v128_load(dataA + i + 4);
            const auto a2 = wasm_v128_load(dataA + i + 8);
            const auto a3 = wasm_v128_load(dataA + i + 12);
            const auto b0 = wasm_v128_load(dataB + i);
            const auto b1 = wasm_v128_load(dataB + i + 4);
            const auto b2 = wasm_v128_load(dataB + i + 8);
            const auto b3 = wasm_v128_load(dataB + i + 12);
            const auto c0 = wasm_v128_load(dataC + i);
            const auto c1 = wasm_v128_load(dataC + i + 4);
            const auto c2 = wasm_v128_load(dataC + i + 8);
            const auto c3 = wasm_v128_load(dataC + i + 12);
            const auto d0 = wasm_v128_load(dataD + i);
            const auto d1 = wasm_v128_load(dataD + i + 4);
            const auto d2 = wasm_v128_load(dataD + i + 8);
            const auto d3 = wasm_v128_load(dataD + i + 12);

            const auto sample0 = wasm_f32x4_add(wasm_f32x4_add(a0, b0), wasm_f32x4_add(c0, d0));
            const auto sample1 = wasm_f32x4_add(wasm_f32x4_add(a1, b1), wasm_f32x4_add(c1, d1));
            const auto sample2 = wasm_f32x4_add(wasm_f32x4_add(a2, b2), wasm_f32x4_add(c2, d2));
            const auto sample3 = wasm_f32x4_add(wasm_f32x4_add(a3, b3), wasm_f32x4_add(c3, d3));

            const auto head0 = wasm_v128_load(head + i);
            const auto head1 = wasm_v128_load(head + i + 4);
            const auto head2 = wasm_v128_load(head + i + 8);
            const auto head3 = wasm_v128_load(head + i + 12);

            wasm_v128_store(head + i, wasm_f32x4_add(head0, sample0));
            wasm_v128_store(head + i + 4, wasm_f32x4_add(head1, sample1));
            wasm_v128_store(head + i + 8, wasm_f32x4_add(head2, sample2));
            wasm_v128_store(head + i + 12, wasm_f32x4_add(head3, sample3));
        }
#elif KAZE_CPU_ARM_NEON
        for (; i <= static_cast<Int>(sampleLength) - 16; i += 16)
        {
            const auto a0 = vld1q_f32(dataA + i);
            const auto a1 = vld1q_f32(dataA + i + 4);
            const auto a2 = vld1q_f32(dataA + i + 8);
            const auto a3 = vld1q_f32(dataA + i +  12);
            const auto b0 = vld1q_f32(dataB + i);
            const auto b1 = vld1q_f32(dataB + i + 4);
            const auto b2 = vld1q_f32(dataB + i + 8);
            const auto b3 = vld1q_f32(dataB + i + 12);
            const auto c0 = vld1q_f32(dataC + i);
            const auto c1 = vld1q_f32(dataC + i + 4);
            const auto c2 = vld1q_f32(dataC + i + 8);
            const auto c3 = vld1q_f32(dataC + i + 12);
            const auto d0 = vld1q_f32(dataD + i);
            const auto d1 = vld1q_f32(dataD + i + 4);
            const auto d2 = vld1q_f32(dataD + i + 8);
            const auto d3 = vld1q_f32(dataD + i + 12);

            const auto sample0 = vaddq_f32(vaddq_f32(a0, b0), vaddq_f32(c0, d0));
            const auto sample1 = vaddq_f32(vaddq_f32(a1, b1), vaddq_f32(c1, d1));
            const auto sample2 = vaddq_f32(vaddq_f32(a2, b2), vaddq_f32(c2, d2));
            const auto sample3 = vaddq_f32(vaddq_f32(a3, b3), vaddq_f32(c3, d3));

            const auto head0 = vld1q_f32(head + i);
            const auto head1 = vld1q_f32(head + i + 4);
            const auto head2 = vld1q_f32(head + i + 8);
            const auto head3 = vld1q_f32(head + i + 12);

            vst1q_f32(head + i, vaddq_f32(head0, sample0));
            vst1q_f32(head + i + 4, vaddq_f32(head1, sample1));
            vst1q_f32(head + i + 8, vaddq_f32(head2, sample2));
            vst1q_f32(head + i + 12, vaddq_f32(head3, sample3));
        }
#endif
        // Catch the leftover samples (should be divisible by 2 because of stereo)
        for (; i <= sampleLength - 2; i += 2)
        {
            const auto sample0 = dataA[i] + dataB[i] + dataC[i] + dataD[i];
            const auto sample1 = dataA[i + 1] + dataB[i + 1] + dataC[i + 1] + dataD[i + 1];
            head[i] += sample0;
            head[i + 1] += sample1;
        }
    }
    // Catch the leftover sources
    for (const int sourcecount = (int)m_sources.size(); sourcei < sourcecount; ++sourcei)
    {
        auto source = m_sources[sourcei].get();

        const float *data0;
        auto floatsToRead0 = source->read(reinterpret_cast<const Ubyte**>(&data0), length) / sizeof(float);

        auto head = reinterpret_cast<float *>(output);
        for (int i = 0; i < floatsToRead0; i += 4)
        {
            head[i] += data0[i];
            head[i + 1] += data0[i + 1];
            head[i + 2] += data0[i + 2];
            head[i + 3] += data0[i + 3];
        }
    }
    return length;
}

auto AudioBus::updateParentClock(Uint64 parentClock) -> Bool
{
    if ( !AudioSource::updateParentClock(parentClock) )
        return False;

    const auto curClock = getClock();
    Bool result = True;

    for (const auto &source : m_sources)
    {
        if ( !source->updateParentClock(curClock) )
        {
            result = False;
        }
    }

    return result;
}

auto AudioBus::processRemovals() -> void
{
    const auto ctx = context();

    std::erase_if(m_sources, [ctx] (const Handle<AudioSource> &handle) {
        if ( !handle.isValid() )
            return True;

        const auto source = handle.get();

        if (const auto bus = dynamic_cast<AudioBus *>(source))
        {
            // Recursively remove sub-bus children
            // Note: if graph is huge, this could be a problem...
            bus->processRemovals();
        }

        if (source->shouldDiscard())
        {
            ctx->releaseObjectImpl(handle);
            return True;
        }

        return False;
    });
}

// Private Impl functions don't need to check for validity since it's the caller's responsibility to check

auto AudioBus::connectSourceImpl(const Handle<AudioBus> &bus, const Handle<AudioSource> &source) -> void
{
    // If source is a bus, remove itself from its parent first
    if (const auto sourceBus = source.getAs<AudioBus>())
    {
        if (const auto parent = sourceBus->getOutputBus(); parent.isValid())
        {
            disconnectSourceImpl(parent, source);
        }

        sourceBus->m_parent = bus;
    }

    bus->m_sources.emplace_back(source);
}

auto AudioBus::disconnectSourceImpl(const Handle<AudioBus> &bus, const Handle<AudioSource> &source) -> void
{
    auto &sources = bus->m_sources;
    for (auto it = sources.begin(); it != sources.end(); ++it)
    {
        if (*it == source)
        {
            sources.erase(it);
            break;
        }
    }
}

KAUDIO_NS_END
