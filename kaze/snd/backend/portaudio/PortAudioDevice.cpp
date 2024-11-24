#include "PortAudioDevice.h"

#include <kaze/core/lib.h>
#include <kaze/core/debug.h>
#include <kaze/core/endian.h>
#include <kaze/core/platform/defines.h>
#include <kaze/core/memory.h>

#include <portaudio.h>
#include <mutex>

#if KAZE_PLATFORM_MACOS
#include <CoreAudio/CoreAudio.h>
#endif

#if KAZE_PLATFORM_WINDOWS
#include <mmdeviceapi.h>
#endif

KSND_NS_BEGIN

struct PortAudioDevice::Impl {
#if KAZE_PLATFORM_WINDOWS
    class DeviceChangeNotification : public IMMNotificationClient {
    public:
        explicit DeviceChangeNotification(Impl *m) : m(m) { }

    private:
        // Implement necessary methods from IMMNotificationClient interface
        STDMETHODIMP OnDefaultDeviceChanged(EDataFlow flow, ERole role, LPCWSTR pwstrDeviceId) override {
            // Handle default device change
            KAZE_CORE_LOG("Device changed");
            // Implement your logic here
            auto lock = std::lock_guard(m->mutex);
            m->id = -1;
            return S_OK;
        }

        // Implement other methods like OnDeviceAdded, OnDeviceRemoved as needed

        // Other methods like OnDeviceStateChanged, OnPropertyValueChanged can also be implemented

        PortAudioDevice::Impl *m;

        // Inherited via IMMNotificationClient
        HRESULT __stdcall QueryInterface(REFIID riid, void **ppvObject) override
        {
            return E_NOTIMPL;
        }
        ULONG __stdcall AddRef(void) override
        {
            return 0;
        }
        ULONG __stdcall Release(void) override
        {
            return 0;
        }
        HRESULT __stdcall OnDeviceStateChanged(LPCWSTR pwstrDeviceId, DWORD dwNewState) override
        {
            return 0;
        }
        HRESULT __stdcall OnDeviceAdded(LPCWSTR pwstrDeviceId) override
        {
            return 0;
        }
        HRESULT __stdcall OnDeviceRemoved(LPCWSTR pwstrDeviceId) override
        {
            return 0;
        }
        HRESULT __stdcall OnPropertyValueChanged(LPCWSTR pwstrDeviceId, const PROPERTYKEY key) override
        {
            return 0;
        }
    } devNotificationClient{this};
    IMMDeviceEnumerator *devEnumerator{};
#endif

    Bool paWasInit{};
    std::atomic<PaStream *> stream{};
    PaDeviceIndex id{};
    AudioCallback callback{};
    void *userdata{};
    AlignedList<Uint8, 16> buffer{};
    AudioSpec spec{};
    std::mutex mutex{};
    Uint requestedBufferFrames{};

    Impl()
    {
        if (const auto err = Pa_Initialize(); err != paNoError)
        {
            KAZE_PUSH_ERR(Error::RuntimeErr, "Pa_Initialize failed: {}", Pa_GetErrorText(err));
        }
        else
        {
            paWasInit = True;
#if KAZE_PLATFORM_WINDOWS
            auto result = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                __uuidof(IMMDeviceEnumerator), (void **)&devEnumerator);
            if (FAILED(result))
            {
                KAZE_PUSH_ERR(Error::RuntimeErr, "MMDevice enumerator failed to create");
                return;
            }

            result = devEnumerator->RegisterEndpointNotificationCallback(&devNotificationClient);
            if (FAILED(result))
            {
                KAZE_PUSH_ERR(Error::RuntimeErr, "MMDevice enumerator failed to register callback");
                devEnumerator->Release();
            }
#endif
        }
    }

    ~Impl()
    {
        if (paWasInit)
        {
            Pa_Terminate();
        }
    }

#if KAZE_PLATFORM_MACOS
    static auto deviceChangedListener(
        AudioObjectID inObjectID,
        UInt32 inNumberAddresses,
        const AudioObjectPropertyAddress inAddresses[],
        void *inClientData) -> OSStatus
    {
        auto dev = static_cast<Impl *>(inClientData);
        auto lockGuard = std::lock_guard(dev->mutex);
        dev->id = -1;
        return noErr;
    }
#endif

    auto refreshDefaultDevice() -> Bool
    {
        const auto stream = this->stream.load(std::memory_order_acquire);
        if (stream)
        {
            if (auto result = Pa_StopStream(stream); result != paNoError && result != paStreamIsStopped)
            {
                result = Pa_AbortStream(stream);
                if (result != paNoError)
                    KAZE_PUSH_ERR(Error::RuntimeErr, "Pa_StopStream failed: {}",
                        Pa_GetErrorText(result));
            }
            this->stream.store(nullptr, std::memory_order_release);

            if (const auto result = Pa_Terminate(); result != paNoError) // Terminate auto-closes all opened streams
            {
                KAZE_PUSH_ERR(Error::RuntimeErr, "Pa_Terminate failed: {}", Pa_GetErrorText(result));
                return false;
            }

            if (const auto result = Pa_Initialize(); result != paNoError)
            {
                KAZE_PUSH_ERR(Error::RuntimeErr, "Pa_Initialize failed: {}", Pa_GetErrorText(result));
                return false;
            }
        }

        return open(Pa_GetDefaultOutputDevice(),
            spec.freq, static_cast<Int>(requestedBufferFrames),
            callback, userdata);
    }

    auto open(PaDeviceIndex devId, Int frequency, Int sampleFrameBufferSize, AudioCallback engineCallback, void *userdata) -> Bool
    {
        std::unique_lock lockGuard(this->mutex);

        frequency = frequency ? frequency : 48000;
        PaStream *stream;
        PaStreamParameters outParams{};
        outParams.device = devId;
        outParams.channelCount = 2;
        outParams.sampleFormat = paFloat32;
        outParams.suggestedLatency = 0;
        outParams.hostApiSpecificStreamInfo = nullptr;

        auto err = Pa_OpenStream(&stream, nullptr, &outParams, frequency, sampleFrameBufferSize, 0,
            Impl::paCallback, this);
        if (err != paNoError)
        {
            KAZE_PUSH_ERR(Error::RuntimeErr, "Pa_OpenStream failed: {}", Pa_GetErrorText(err));
            return False;
        }

        auto id = Pa_GetDefaultOutputDevice();

        if (err = Pa_StartStream(stream); err != paNoError)
        {
            KAZE_PUSH_ERR(Error::RuntimeErr, "Pa_StartStream failed: {}", Pa_GetErrorText(err));
            return False;
        }

        lockGuard.unlock();
        close();
        lockGuard.lock();

        this->requestedBufferFrames = sampleFrameBufferSize;
        this->spec.channels = 2;
        this->spec.freq = frequency;
        this->spec.format = SampleFormat(sizeof(float) * CHAR_BIT, true, Endian::isBig(), true);
        this->callback = engineCallback;
        this->userdata = userdata;
        this->stream = stream;
        this->buffer.resize(sampleFrameBufferSize * sizeof(float) * 2);
        this->id = id;

#if KAZE_PLATFORM_MACOS
        // Register device change listener
        AudioObjectPropertyAddress propAddress = {
            kAudioHardwarePropertyDefaultOutputDevice,
            kAudioObjectPropertyScopeGlobal,
            kAudioObjectPropertyElementMain
        };

        AudioObjectAddPropertyListener(kAudioObjectSystemObject, &propAddress, deviceChangedListener, this);
#endif
        return True;
    }

    void close()
    {
        std::lock_guard lockGuard(this->mutex);
        auto stream = this->stream.load(std::memory_order_acquire);
        if (stream)
        {
#if KAZE_PLATFORM_MACOS
            // Register device change listener
            AudioObjectPropertyAddress propAddress = {
                kAudioHardwarePropertyDefaultOutputDevice,
                kAudioObjectPropertyScopeGlobal,
                kAudioObjectPropertyElementMain
            };

            AudioObjectRemovePropertyListener(kAudioObjectSystemObject, &propAddress, deviceChangedListener, this);
#endif

#if KAZE_PLATFORM_WINDOWS
            if (devEnumerator)
            {
                devEnumerator->UnregisterEndpointNotificationCallback(&devNotificationClient);
                devEnumerator->Release();
                devEnumerator = nullptr;
            }
#endif
#if !KAZE_PLATFORM_LINUX
            Pa_StopStream(stream); // causes a lock with asound due to mixer lock prior to close
#endif
            Pa_CloseStream(stream);
            this->stream.store(nullptr, std::memory_order_release);
        }
    }

    static Int paCallback(const void *inputBuffer, void *outputBuffer,
                       unsigned long framesPerBuffer,
                       const PaStreamCallbackTimeInfo* timeInfo,
                       PaStreamCallbackFlags statusFlags,
                       void *userData)
    {
        const auto dev = static_cast<Impl *>(userData);

        auto lockGuard = std::lock_guard(dev->mutex);
        const auto bufferByteSize = framesPerBuffer * sizeof(float) * 2;

        if (dev->buffer.size() != bufferByteSize)
            dev->buffer.resize(bufferByteSize);
        dev->callback(dev->userdata, &dev->buffer);

        memory::copy(outputBuffer, dev->buffer.data(), bufferByteSize);

        return 0;
    }
};

PortAudioDevice::PortAudioDevice() : m(new Impl)
{ }

PortAudioDevice::~PortAudioDevice()
{
    delete m;
}

auto PortAudioDevice::open(const AudioDeviceOpen &config) -> Bool
{
    const auto device = Pa_GetDefaultOutputDevice();
    if (device == paNoDevice)
    {
        KAZE_PUSH_ERR(Error::RuntimeErr,
            "Pa_GetDefaultOutputDevice: no device could be retrieved");
        return False;
    }

    return m->open(device,
        config.frequency, config.frameBufferSize,
        config.audioCallback, config.userdata);
}

auto PortAudioDevice::close() -> void
{
    m->close();
}

auto PortAudioDevice::suspend() -> void
{
    Pa_StopStream(m->stream.load(std::memory_order_acquire));
}

auto PortAudioDevice::resume() -> void
{
    Pa_StartStream(m->stream.load(std::memory_order_acquire));
}

auto PortAudioDevice::isOpen() const -> Bool
{
    return m->stream.load(std::memory_order_acquire) != nullptr;
}

auto PortAudioDevice::isRunning() const -> Bool
{
    return Pa_IsStreamStopped(m->stream.load(std::memory_order_acquire));
}

auto PortAudioDevice::getId() const -> Uint
{
    return m->id;
}

const AudioSpec & PortAudioDevice::getSpec() const
{
    return m->spec;
}

auto PortAudioDevice::getBufferSize() const -> Int
{
    return static_cast<Int>(m->buffer.size());
}

auto PortAudioDevice::getDefaultSampleRate() const -> Int
{
    const auto dev = Pa_GetDefaultOutputDevice();
    if (dev == paNoDevice)
    {
        KAZE_PUSH_ERR(Error::RuntimeErr,
            "Pa_GetDefaultOutputDevice failed");
        return -1;
    }
    const auto info = Pa_GetDeviceInfo(dev);
    if ( !info )
    {
        KAZE_PUSH_ERR(Error::RuntimeErr,
            "Pa_GetDeviceInfo failed to get audio device info");
        return -1;
    }
    return static_cast<Int>(info->defaultSampleRate);
}

auto PortAudioDevice::update() -> void
{
    //auto lockGuard = std::lock_guard(m->mutex);
    if (m->id != Pa_GetDefaultOutputDevice())
    {
        m->refreshDefaultDevice();
    }
}

KSND_NS_END
