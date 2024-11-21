#include "AAudioDevice.h"
#include <kaze/core/platform/defines.h>

#if !KAZE_PLATFORM_ANDROID
#error AAudioDevice is only available on the Android Native platform
#endif

#include <kaze/core/debug.h>
#include <kaze/core/memory.h>
#include <kaze/core/platform/native/android/AndroidNative.h>
#include <aaudio/AAudio.h>
#include <mutex>

KSND_NS_BEGIN

struct AAudioDevice::Impl {
    AAudioStream *m_stream{};
    AudioCallback m_callback{};
    void *m_userData{};
    AudioSpec m_spec{};
    AlignedList<Ubyte, 16> m_buffer{};
    mutable std::recursive_mutex m_mutex{};

    auto open(const AudioDeviceOpen &config) -> Bool
    {
        std::lock_guard lockGuard(m_mutex);

        AAudioStreamBuilder *builder;
        if (const auto result = AAudio_createStreamBuilder(&builder); result != AAUDIO_OK)
        {
            KAZE_PUSH_ERR(Error::RuntimeErr, "AAudio_createStreamBuilder failed: {}",
                AAudio_convertResultToText(result));
            return False;
        }

        Int frequency = (frequency == 0) ? 48000 : config.frequency;

        AAudioStreamBuilder_setSampleRate(builder, frequency);
        AAudioStreamBuilder_setBufferCapacityInFrames(builder,
            config.frameBufferSize == 0 ? 512 : config.frameBufferSize);
        AAudioStreamBuilder_setSharingMode(builder, AAUDIO_SHARING_MODE_SHARED); // low latency
        AAudioStreamBuilder_setDirection(builder, AAUDIO_DIRECTION_OUTPUT);
        AAudioStreamBuilder_setChannelCount(builder, 2);
#if __ANDROID_MIN_SDK_VERSION__ >= 32 // channel mask only available in 32+
        AAudioStreamBuilder_setChannelMask(builder, AAUDIO_CHANNEL_STEREO);
#endif
        AAudioStreamBuilder_setFormat(builder, AAUDIO_FORMAT_PCM_FLOAT);
        AAudioStreamBuilder_setPerformanceMode(builder, AAUDIO_PERFORMANCE_MODE_LOW_LATENCY);
        AAudioStreamBuilder_setDataCallback(builder, aaudioCallback, this);

        m_spec.channels = 2;
        m_spec.freq = frequency;
        m_spec.format = SampleFormat(sizeof(Float) * CHAR_BIT, True, False, True);
        m_userData = config.userdata;
        m_callback = config.audioCallback;
        if (auto result = AAudioStreamBuilder_openStream(builder, &m_stream); result != AAUDIO_OK)
        {
            AAudioStreamBuilder_delete(builder);
            KAZE_PUSH_ERR(Error::RuntimeErr, "AAudioStreamBuilder_openStream failed: {}",
                          AAudio_convertResultToText(result));
            return False;
        }

        AAudioStreamBuilder_delete(builder);

        if (auto result = AAudioStream_requestStart(m_stream); result != AAUDIO_OK)
        {
            AAudioStream_close(m_stream);
            return False;
        }
        // TODO: double check that we're 2-channel Float?
        return True;
    }

    auto suspend() -> void
    {
        std::lock_guard lockGuard(m_mutex);
        if (m_stream)
            AAudioStream_requestPause(m_stream);
    }

    auto resume() -> void
    {
        std::lock_guard lockGuard(m_mutex);
        if (m_stream)
            AAudioStream_requestStart(m_stream);
    }

    auto close() -> void
    {
        std::lock_guard lockGuard(m_mutex);
        if (m_stream)
        {
            AAudioStream_requestStop(m_stream);
            AAudioStream_close(m_stream);
            m_stream = nullptr;
        }
    }

    [[nodiscard]]
    auto isRunning() const -> Bool
    {
        std::lock_guard lockGuard(m_mutex);
        if (!m_stream)
            return False;
        return AAudioStream_getState(m_stream) == AAUDIO_STREAM_STATE_STARTED;
    }

    [[nodiscard]]
    auto getId() const -> Uint
    {
        std::lock_guard lockGuard(m_mutex);
        if (!m_stream)
            return 0;
        return AAudioStream_getDeviceId(m_stream);
    }

    [[nodiscard]]
    auto getBufferSize() const -> Int
    {
        std::lock_guard lockGuard(m_mutex);
        if (!m_stream)
            return 0;
        return AAudioStream_getBufferCapacityInFrames(m_stream) *
               m_spec.channels *
               (m_spec.format.bits() / CHAR_BIT);
    }

    [[nodiscard]]
    auto isOpen() const -> Bool
    {
        std::lock_guard lockGuard(m_mutex);
        return m_stream != nullptr;
    }

    static auto aaudioCallback(
        AAudioStream *stream,
        void *userData,
        void *audioData,
        int32_t nFrames) -> aaudio_data_callback_result_t
    {
        auto device = static_cast<AAudioDevice::Impl *>(userData);
        std::lock_guard lockGuard(device->m_mutex);
        if (AAudioStream_getState(device->m_stream) != AAUDIO_STREAM_STATE_STARTED)
        {
            return AAUDIO_CALLBACK_RESULT_CONTINUE;
        }

        const auto byteSize = nFrames * sizeof(Float) * 2;
        if (device->m_buffer.size() != byteSize)
            device->m_buffer.resize(byteSize);

        device->m_callback(device->m_userData, &device->m_buffer);
        memory::copy(audioData, device->m_buffer.data(), byteSize);

        return AAUDIO_CALLBACK_RESULT_CONTINUE;
    }
};

AAudioDevice::AAudioDevice() : m(new Impl)
{ }

AAudioDevice::~AAudioDevice()
{
    delete m;
}


auto AAudioDevice::open(const AudioDeviceOpen &config) -> Bool
{
    return m->open(config);
}

auto AAudioDevice::suspend() -> void
{
    m->suspend();
}

auto AAudioDevice::resume() -> void
{
    m->resume();
}

auto AAudioDevice::close() -> void
{
    m->close();
}

auto AAudioDevice::isRunning() const -> Bool
{
    return m->isRunning();
}

auto AAudioDevice::getId() const -> Uint
{
    return m->getId();
}

auto AAudioDevice::getSpec() const -> const AudioSpec &
{
    return m->m_spec;
}

auto AAudioDevice::getBufferSize() const -> Int
{
    return m->getBufferSize();
}

auto AAudioDevice::isOpen() const -> Bool
{
    return m->isOpen();
}

auto AAudioDevice::getDefaultSampleRate() const -> Int
{
    return android::getDefaultSampleRate();
}


KSND_NS_END
