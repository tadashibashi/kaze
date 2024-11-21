#include <kaze/internal/core/platform/defines.h>

#if !KAZE_PLATFORM_EMSCRIPTEN
#error EmAudioDevice must only be compiled by Emscripten compiler
#endif

#include "EmAudioDevice.h"
#include <kaze/internal/audio/AudioSpec.h>
#include <kaze/internal/core/debug.h>
#include <emscripten/webaudio.h>

#include <atomic>

KAUDIO_NS_BEGIN

static constexpr int AudioStackSize = 1024 * 1024 * 2; // 2MB
static_assert(AudioStackSize % 16 == 0, "AudioStackSize must be a multiple of 16");

struct EmAudioDevice::Impl {
    Impl() : m_audioThreadStack(static_cast<uint8_t *>(std::aligned_alloc(16, AudioStackSize)))
    { }

    ~Impl()
    {
        std::free(m_audioThreadStack);
    }

    [[nodiscard]] auto isBufferReady() const -> Bool
    {
        return m_bufferReady.load();
    }

    [[nodiscard]] auto isRunning() const -> Bool
    {
        return m_isRunning.load();
    }

    // Opens device without creating the audio context. Waits for user interaction to create.
    // Currently not working, as the engine fails when attempting to create busses, add sounds, etc.
    auto open2(const int frequency, int sampleFrameBufferSize,
        const AudioCallback audioCallback, void *userdata) -> Bool
    {
        m_spec.channels = 2;
        m_spec.freq = frequency > 0 ? frequency : getDefaultSampleRate();
        m_spec.format = SampleFormat(sizeof(float) * CHAR_BIT, true, false, true);

        if (sampleFrameBufferSize < 128) // buffer should be >= WebAudio callback's minimum frame size
            sampleFrameBufferSize = 128;
        m_bufferSize = sampleFrameBufferSize * static_cast<int>(sizeof(float)) * 2;

        m_buffer.resize(m_bufferSize, 0);
        m_nextBuffer.resize(m_bufferSize, 0);

        m_context = 0;
        m_callback = audioCallback;
        m_userData = userdata;

        m_curBufferOffset = 0;
        m_bufferReady.store(false);
        m_isRunning.store(false);

        emscripten_set_click_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, 0, onWindowClick2);
        return true;
    }

    auto open(const int frequency, int sampleFrameBufferSize,
        const AudioCallback audioCallback, void *userdata) -> Bool
    {
        const auto defaultSampleRate = getDefaultSampleRate();

        // Create AudioContext
        EmscriptenWebAudioCreateAttributes attr;
        attr.sampleRate = frequency > 0 ? frequency : defaultSampleRate;
        attr.latencyHint = "interactive";
        const auto context = emscripten_create_audio_context(&attr); // todo: error handling here?

        // Initialize internals before starting worklet thread
        close();
        m_spec.channels = 2;
        m_spec.freq = frequency > 0 ? frequency : defaultSampleRate;
        m_spec.format = SampleFormat(sizeof(float) * CHAR_BIT, true, false, true);

        if (sampleFrameBufferSize < 128) // buffer should be >= WebAudio callback's minimum frame size
            sampleFrameBufferSize = 128;
        m_bufferSize = sampleFrameBufferSize * static_cast<int>(sizeof(float)) * 2;

        m_buffer.resize(m_bufferSize, 0);
        m_nextBuffer.resize(m_bufferSize, 0);

        m_context = context;
        m_callback = audioCallback;
        m_userData = userdata;

        m_curBufferOffset = 0;
        m_bufferReady = false;
        m_isRunning = false;

        emscripten_start_wasm_audio_worklet_thread_async(context, m_audioThreadStack, AudioStackSize,
            &audioThreadInitialized, this);
        return true;
    }

    auto close() -> void
    {
        if (m_context != 0)
        {
            emscripten_destroy_web_audio_node(m_audioWorkletNode);
            m_audioWorkletNode = 0;

            emscripten_destroy_audio_context(m_context);
            m_context = 0;
        }
    }

    auto suspend() -> void
    {
        if (isRunning() && m_userInteracted.load())
        {
            EM_ASM({
                var audioContext = emscriptenGetAudioObject($0);
                if (audioContext)
                {
                    audioContext.suspend();
                }
            }, m_context);

            setIsRunning(false);
        }
    }

    auto resumeAsync() -> void
    {
        if (m_userInteracted.load(std::memory_order_acquire))
            emscripten_resume_audio_context_async(m_context, audioContextResumed, this);
    }

    auto resume() -> void
    {
        if (m_userInteracted.load(std::memory_order_acquire))
        {
            emscripten_resume_audio_context_sync(m_context);
            setIsRunning(true);
        }

    }

    auto read(const Float **data, const Int length) -> void
    {
        if (!isRunning())
        {
            *data = reinterpret_cast<float *>(m_buffer.data());
            return;
        }

        if (!isBufferReady()) // process next buffer
        {
            m_callback(m_userData, &m_nextBuffer);
            setBufferReady(true);
        }

        if (m_curBufferOffset >= m_bufferSize)
        {
            m_buffer.swap(m_nextBuffer);
            m_curBufferOffset = 0;
            setBufferReady(false); // signal the mix thread to start processing next buffer
        }

        *data = reinterpret_cast<float *>(m_buffer.data() + m_curBufferOffset);
        m_curBufferOffset += length;
    }

    [[nodiscard]]
    auto getSpec() const -> const AudioSpec & { return m_spec; }

    [[nodiscard]]
    auto getId() const { return m_context; }

    [[nodiscard]]
    auto getBufferSize() const { return m_bufferSize; }

    [[nodiscard]]
    static auto getDefaultSampleRate() -> Int
    {
        return EM_ASM_INT({
            var AudioContext = window.AudioContext || window.webkitAudioContext;
            if (AudioContext)
            {
                var context = new AudioContext();
                var sampleRate = context.sampleRate;
                context.close();
                return sampleRate;
            }

            return -1;
        });
    }

private:
    std::mutex m_readMutex{};
    Ubyte *m_audioThreadStack{};
    AudioCallback m_callback{};
    void *m_userData{};

    AudioSpec m_spec{};
    AlignedList<Ubyte, 16> m_buffer{}, m_nextBuffer{};
    int m_bufferSize{};

    EMSCRIPTEN_WEBAUDIO_T m_context{0};
    EMSCRIPTEN_AUDIO_WORKLET_NODE_T m_audioWorkletNode{0};
    int m_curBufferOffset{0};

    std::atomic<Bool> m_isRunning{false};
    std::atomic<Bool> m_bufferReady{false};
    std::atomic<Bool> m_userInteracted{false};

    void setBufferReady(const Bool value)
    {
        m_bufferReady.store(value);
    }

    void setIsRunning(const Bool value)
    {
        m_isRunning.store(value);
    }

    // WebAudio-related callbacks
    static void audioWorkletProcessorCreated(EMSCRIPTEN_WEBAUDIO_T context, EM_BOOL success, void *userData);
    static void audioThreadInitialized(EMSCRIPTEN_WEBAUDIO_T context, EM_BOOL success, void *userData);
    static EM_BOOL onWindowClick(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData);
    static EM_BOOL onWindowClick2(int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData);
    static EM_BOOL emAudioCallback(int numInputs, const AudioSampleFrame *inputs,
        int numOutputs, AudioSampleFrame *outputs,
        int numParams, const AudioParamFrame *params,
        void *userData);
    static void audioContextResumed(EMSCRIPTEN_WEBAUDIO_T audioContext, AUDIO_CONTEXT_STATE state, void *userData1);
};

auto EmAudioDevice::read(const Float **data, Int length) -> void
{
    m->read(data, length);
}

EmAudioDevice::EmAudioDevice() : m(new Impl)
{
}

EmAudioDevice::~EmAudioDevice()
{
    delete m;
}

auto EmAudioDevice::open(const AudioDeviceOpen &config) -> Bool
{
    return m->open(config.frequency,
        config.frameBufferSize,
        config.audioCallback,
        config.userdata);
}

auto EmAudioDevice::close() -> void
{
    m->close();
}

auto EmAudioDevice::suspend() -> void
{
    m->suspend();
}

auto EmAudioDevice::resume() -> void
{
    m->resumeAsync();
}

auto EmAudioDevice::getBufferSize() const -> Int
{
    return m->getBufferSize();
}

auto EmAudioDevice::isRunning() const -> Bool
{
    return m->isRunning();
}

auto EmAudioDevice::isPlatformSupported() -> Bool
{
    return EM_ASM_INT({
        return !!(window.AudioContext || window.webkitAudioContext) &&
            typeof SharedArrayBuffer === "function";
    });
}

auto EmAudioDevice::isOpen() const -> Bool
{
    return m->getId() != 0;
}

auto EmAudioDevice::getId() const -> Uint
{
    return static_cast<Uint>(m->getId());
}

auto EmAudioDevice::getSpec() const -> const AudioSpec &
{
    return m->getSpec();
}

// ===== Static WebAudio Callbacks ================================================================================

/// Callback for user interaction workaround with web audio
auto EmAudioDevice::Impl::onWindowClick(
    [[maybe_unused]] int eventType,
    [[maybe_unused]] const EmscriptenMouseEvent *mouseEvent,
    void *userData) -> EM_BOOL
{
    const auto device = static_cast<Impl *>(userData);
    device->m_userInteracted.store(true, std::memory_order_release);
    device->resumeAsync();

    // Remove the call back so it isn't called every time
    emscripten_set_click_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, 0, nullptr);
    return EM_FALSE;
}

/// Create and set up the audio device when user clicks. Currently not working, but left here to resume dev.
auto EmAudioDevice::Impl::onWindowClick2(
    int eventType, const EmscriptenMouseEvent *mouseEvent, void *userData) -> EM_BOOL
{
    const auto device = static_cast<Impl *>(userData);
    // Create AudioContext
    EmscriptenWebAudioCreateAttributes attr;
    attr.sampleRate = device->m_spec.freq > 0 ? device->m_spec.freq : getDefaultSampleRate();
    attr.latencyHint = "interactive";
    const auto context = emscripten_create_audio_context(&attr); // todo: error handling here?
    emscripten_start_wasm_audio_worklet_thread_async(context, device->m_audioThreadStack, AudioStackSize,
        &audioThreadInitialized, device);

    emscripten_set_click_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, 0, nullptr);
    return EM_FALSE;
}


/// Called when audio thread is initialized
/// @param context associated web audio context
/// @param success whether audio thread was created successfully
/// @param userData pointer castable to EmAudioDevice::Impl *
auto EmAudioDevice::Impl::audioThreadInitialized(
    const EMSCRIPTEN_WEBAUDIO_T context, const EM_BOOL success, void *userData) -> void
{
    const auto device = static_cast<Impl *>(userData);
    if (success == EM_FALSE)
    {
        KAZE_PUSH_ERR(Error::RuntimeErr, "Failed to init audio thread");
        device->close();
        return;
    }

    constexpr WebAudioWorkletProcessorCreateOptions opts = {
        .name = "KazeAudioMainOutput",
    };
    emscripten_create_wasm_audio_worklet_processor_async(context, &opts, &audioWorkletProcessorCreated, userData);
}

/// Called after audio worklet processor finishes creating.
/// Creates the WASM audio worklet node and sets up the user interaction workaround.
/// @param context associated audio context
/// @param success whether creation succeeded
/// @param userData pointer castable to EmAudioDevice::Impl *
void EmAudioDevice::Impl::audioWorkletProcessorCreated(const EMSCRIPTEN_WEBAUDIO_T context, const EM_BOOL success, void *userData)
{
    const auto device = static_cast<Impl *>(userData);
    if (success == EM_FALSE)
    {
        KAZE_PUSH_ERR(Error::RuntimeErr, "Failed to create audio worklet processor");
        device->close();
        return;
    }

    // Set up configurations for AudioWorklet
     int outputChannelCounts[] = {2};
    const EmscriptenAudioWorkletNodeCreateOptions options = {
        .numberOfInputs = 0,
        .numberOfOutputs = 1,
        .outputChannelCounts = outputChannelCounts
    };

    // Create the AudioWorklet node
    const auto wasmAudioWorklet = emscripten_create_wasm_audio_worklet_node(context, "KazeAudioMainOutput", &options,
        &emAudioCallback, userData);

    // Connect AudioWorklet to AudioContext's hardware output
    EM_ASM({
        emscriptenGetAudioObject($0).connect(emscriptenGetAudioObject($1).destination);
    }, wasmAudioWorklet, context);

    // Setup user interaction workaround
    emscripten_set_click_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, userData, 0, onWindowClick);

    // Done! Commit changes to the device
    if (device->m_audioWorkletNode != NULL)
        emscripten_destroy_web_audio_node(device->m_audioWorkletNode);
    device->m_audioWorkletNode = wasmAudioWorklet;
}

EM_BOOL EmAudioDevice::Impl::emAudioCallback(
    [[maybe_unused]] const int numInputs,
    [[maybe_unused]] const AudioSampleFrame *inputs,
                     const int numOutputs,
                     AudioSampleFrame *outputs,
    [[maybe_unused]] int numParams,
    [[maybe_unused]] const AudioParamFrame *params,
                     void *userData)
{
    /// Assert engine format requirement
    KAZE_ASSERT(numOutputs > 0 && outputs[0].numberOfChannels == 2);
    constexpr auto dataSize = 128 * 2 * sizeof(float);

    const auto device = static_cast<Impl *>(userData);
    if (!device->isRunning())
    {
        return EM_TRUE;
    }

    // Read samples from the mixer
    const float *samples{};
    device->read(&samples, dataSize);
    KAZE_ASSERT(samples);

    // Reformat the samples from interleaved to block
    // [LRLR...] -> [LL...RR...]
    const auto output = outputs[0].data;
    for (int i = 0; i < 128; i += 4)
    {
        const auto ix2 = i * 2;
        const auto ip128 = i + 128;
        output[i]         = samples[ix2];
        output[ip128]     = samples[ix2 + 1];
        output[i + 1]     = samples[ix2 + 2];
        output[ip128 + 1] = samples[ix2 + 3];
        output[i + 2]     = samples[ix2 + 4];
        output[ip128 + 2] = samples[ix2 + 5];
        output[i + 3]     = samples[ix2 + 6];
        output[ip128 + 3] = samples[ix2 + 7];
    }

    return EM_TRUE; // Keep the graph output going
}

// Callback handling async audio context resume
auto EmAudioDevice::Impl::audioContextResumed(
    const EMSCRIPTEN_WEBAUDIO_T audioContext, const AUDIO_CONTEXT_STATE state, void *userData1) -> void
{
    const auto device = static_cast<Impl *>(userData1);
    if (state == AUDIO_CONTEXT_STATE_RUNNING)
        device->setIsRunning(true);
}

auto EmAudioDevice::getDefaultSampleRate() const -> Int
{
    return Impl::getDefaultSampleRate();
}

KAUDIO_NS_END
