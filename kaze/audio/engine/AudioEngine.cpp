#include "AudioEngine.h"
#include "sources/AudioBus.h"
#include "AudioContext.h"

#include <kaze/core/debug.h>

KAUDIO_NS_BEGIN

#if KAZE_DEBUG
#define INIT_GUARD_RET(ret) do { if ( !isOpen() ) { \
    KAZE_PUSH_ERR(Error::NotInitialized, "Attempted to use AudioEngine while uninit"); \
    return (ret); \
} } while(0)
#define INIT_GUARD(ret) do { if ( !isOpen() ) { \
    KAZE_PUSH_ERR(Error::NotInitialized, "Attempted to use AudioEngine while uninit"); \
    return; \
} } while(0)

#else
#define INIT_GUARD_RET(ret) KAZE_NOOP
#define INIT_GUARD() KAZE_NOOP
#endif

struct AudioEngine::Impl
{
    AudioContext context;
};

AudioEngine::AudioEngine() : m(new Impl) { }
AudioEngine::~AudioEngine() { delete m; }

auto AudioEngine::open(const AudioEngineInit &config) -> Bool
{
    return m->context.open({
        .frequency = config.samplerate,
        .samples = config.bufferFrameSize,
    });
}

auto AudioEngine::close() -> void
{
    m->context.close();
}

auto AudioEngine::isOpen() const -> Bool
{
    return m->context.isOpen();
}

auto AudioEngine::createSound(const String &filepath, const Sound::InitFlags flags) -> Handle<Sound>
{
    INIT_GUARD_RET(Handle<Sound>{});

    const auto sound = m->context.createObject<Sound>();
    if ( !sound )
    {
        return {};
    }

    if ( !sound->openFile(filepath, flags, m->context.getSpec()) )
    {
        m->context.releaseObject(sound);
        return {};
    }

    return sound;
}

auto AudioEngine::createSound(const MemView<void> mem, const Sound::InitFlags flags) -> Handle<Sound>
{
    INIT_GUARD_RET(Handle<Sound>{});

    const auto sound = m->context.createObject<Sound>();
    if ( !sound )
    {
        return {};
    }

    if ( !sound->openConstMem(mem, flags, m->context.getSpec()) )
    {
        m->context.releaseObject(sound);
        return {};
    }

    return sound;
}

auto AudioEngine::createSound(const ManagedMem &mem, const Sound::InitFlags flags) -> Handle<Sound>
{
    INIT_GUARD_RET(Handle<Sound>{});

    const auto sound = m->context.createObject<Sound>();
    if ( !sound )
    {
        return {};
    }

    if ( !sound->openMem(mem, flags, m->context.getSpec()) )
    {
        m->context.releaseObject(sound);
        return {};
    }

    return sound;
}

auto AudioEngine::releaseSound(const Handle<Sound> &sound) -> void
{
    INIT_GUARD();
    m->context.releaseObject(sound);
}

auto AudioEngine::playSound(
    const Handle<Sound> &sound,
    const Bool paused,
    const Handle<AudioBus> &bus) -> Handle<AudioSource>
{
    INIT_GUARD_RET(Handle<AudioSource>{});
    Handle<AudioSource> outHandle;
    if ( !sound->instantiate(&m->context, paused, bus, &outHandle) )
    {
        return {};
    }

    return outHandle;
}

auto AudioEngine::createBus(Bool paused, const Handle<AudioBus> &output) -> Handle<AudioBus>
{
    INIT_GUARD_RET(Handle<AudioBus>{});
    if (output && !output.isValid()) // Output was provided, but it's invalid => error
    {
        KAZE_PUSH_ERR(Error::InvalidHandle,
            "AudioEngine::createBus failed because bus `output` was invalid");
        return {};
    }

    Handle<AudioBus> outputBus;
    if (output.isValid())
    {
        outputBus = output;
    }
    else
    {
        if ( const auto masterBus = m->context.getMasterBus() )
        {
            outputBus = masterBus;
        }
        else
        {
            // This should never happen, because initialized AudioContext is guaranteed to have a valid master bus.
            KAZE_PUSH_ERR(Error::RuntimeErr, "Internal error, missing master bus");
            return {};
        }
    }

    const auto newBusHandle = m->context.createObject<AudioBus>(
        &m->context,
        outputBus,
        paused);

    if (outputBus)
        m->context.pushCommand(commands::BusConnectSource {
            .bus = outputBus,
            .source = newBusHandle.cast<AudioSource>()
        });

    return newBusHandle;
}

auto AudioEngine::getDeviceID() const -> Uint
{
    return m->context.getDeviceId();
}

auto AudioEngine::getSpec() const -> const AudioSpec &
{
    return m->context.getSpec();
}

auto AudioEngine::getBufferSize() const -> Uint
{
    return m->context.getBufferSize();
}

auto AudioEngine::getMasterBus() const -> Handle<AudioBus>
{
    return m->context.getMasterBus();
}

auto AudioEngine::setPaused(const Bool value) -> void
{
    if (value)
        m->context.m_device->suspend();
    else
        m->context.m_device->resume();
}

auto AudioEngine::getPaused() const -> Bool
{
    return !m->context.m_device->isRunning();
}

auto AudioEngine::update() -> void
{
    m->context.update();
}


KAUDIO_NS_END
