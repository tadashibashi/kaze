#include "AudioContext.h"
#include "sources/AudioBus.h"

KSND_NS_BEGIN

auto AudioContext::getBufferSize() const -> Int
{
    KAZE_ASSERT(isOpen());
    return m_device->getBufferSize();
}

auto AudioContext::getSpec() const -> const AudioSpec &
{
    KAZE_ASSERT(isOpen());
    return m_device->getSpec();
}

auto AudioContext::open(const AudioContextOpen &config) -> Bool
{
    if (m_device->isOpen()) // Currently only allows one open
    {
        return True;
    }

    const auto result = m_device->open({
        .frequency = config.frequency == 0 ? m_device->getDefaultSampleRate() : config.frequency,
        .frameBufferSize = config.samples,
        .audioCallback = &audioCallback,
        .userdata = this,
    });

    if ( !result )
    {
        return False;
    }

    const auto bus = createObject<AudioBus>(this, Handle<AudioBus>{}, False);
    if ( !bus )
    {
        m_device->close();
        return False;
    }

    m_masterBus = bus;
    m_device->resume();
    return True;
}

auto AudioContext::close() -> void
{
    auto lockGuard = std::lock_guard(m_mixMutex);
    if (isOpen())
    {
        if (m_masterBus.isValid())
        {
            m_masterBus->m_isMaster = False;
            m_masterBus->release();
            m_immediateCmds.processCommandsLocked();
            m_deferredCmds.processCommandsLocked();

            m_masterBus->processRemovals();
            releaseObjectImpl(m_masterBus);
            m_masterBus = {};
        }

        m_clock = 0;
        m_device->close();
    }
}

AudioContext::AudioContext()
{
    m_device = AudioDevice::create();
}

AudioContext::~AudioContext()
{
    close();
    delete m_device;
}

auto AudioContext::flagRemoveSource() -> void
{
    //const auto lockGuard = std::lock_guard(m_mixMutex);
    m_removeSourceFlag = True;
}

auto AudioContext::audioCallback(void *userptr, AlignedList<Ubyte, 16> *outBuffer) -> void
{
    const auto context = static_cast<AudioContext *>(userptr);
    if ( !context->isOpen() )
        return;

    const auto lockGuard = std::lock_guard(context->m_mixMutex);
    // Process commands that require sample-accurate immediacy
    // TODO: not sure why, but deferred command lock guard was set here in original repo.
    //       This may need to be added for some reason...
    //       Also processImmediateCommands was unlocked.
    context->m_immediateCmds.processCommandsLocked();

    if (context->m_removeSourceFlag)
    {
        context->m_masterBus->processRemovals();
        context->m_removeSourceFlag = False;
    }

    const auto bufSize = outBuffer->size();
    context->m_masterBus->read(Null, static_cast<Int64>(bufSize));
    context->m_clock += bufSize / (2 * sizeof(Float)); // we currently only support stereo float output
    context->m_masterBus->updateParentClock(context->m_clock);

    context->m_masterBus->swapBuffers(outBuffer);
}

auto AudioContext::update() -> void
{
    if ( !isOpen() )
    {
        KAZE_PUSH_ERR(Error::LogicErr,
            "AudioContext::update called in uninit state");
        return;
    }

    m_device->update();

    const auto lockGuard = std::lock_guard(m_mixMutex);
    m_deferredCmds.processCommandsLocked();
}

KSND_NS_END

