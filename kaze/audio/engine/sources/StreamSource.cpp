#include "StreamSource.h"

#include <kaze/audio/conv/AudioDecoder.h>
#include <kaze/core/memory.h>

KAUDIO_NS_BEGIN
/// Macro to ensure that the StreamSource is open in a StreamSource function
#ifdef KAZE_DEBUG
#define INIT_GUARD() do { if (!isOpen()) { \
    KAZE_PUSH_ERR(Error::NotInitialized, "Attempted to access StreamSource in uninit state."); \
    return; \
} } while(0)
#define INIT_GUARD_RET(ret) do { if (!isOpen()) { \
    KAZE_PUSH_ERR(Error::NotInitialized, "Attempted to access StreamSource in uninit state."); \
    return (ret); \
} } while(0)
#else
#define INIT_GUARD_RET(ret) KAZE_NOOP
#define INIT_GUARD() KAZE_NOOP
#endif

struct StreamSource::Impl {
    Impl() = default;

    AudioDecoder decoder{};
    Bool looping{}, isOneShot{};
    Int bytesPerFrame{};
};

StreamSource::StreamSource() : m(new Impl)
{

}

StreamSource::StreamSource(StreamSource &&other) noexcept :
    AudioSource(std::move(other)), m(other.m)
{
    other.m = nullptr;
}


StreamSource::~StreamSource()
{
    delete m;
}

auto StreamSource::isOpen() const -> Bool
{
    return m != Null && m->decoder.isOpen();
}

auto StreamSource::openConstMem(const MemView<void> mem) -> Bool
{
    const auto &targetSpec = context()->getSpec();

    AudioDecoder decoder;
    if (!decoder.openConstMem(mem, targetSpec))
    {
        return False;
    }

    decoder.setLooping(m->looping);
    m->decoder = std::move(decoder);
    m->bytesPerFrame = static_cast<int>(targetSpec.bytesPerFrame());
    return True;
}

auto StreamSource::openMem(const ManagedMem mem) -> Bool
{
    const auto &targetSpec = context()->getSpec();

    AudioDecoder decoder;
    if (!decoder.openMem(mem, targetSpec))
    {
        return False;
    }

    decoder.setLooping(m->looping);
    m->decoder = std::move(decoder);
    m->bytesPerFrame = static_cast<Int>(targetSpec.bytesPerFrame());
    return True;
}

auto StreamSource::openFile(const String &filepath, const Bool inMemory) -> Bool
{
    const auto &targetSpec = context()->getSpec();

    // Init audio decoder by file type
    AudioDecoder decoder;
    if (!decoder.openFile(filepath, targetSpec, inMemory))
    {
        return False;
    }

    decoder.setLooping(m->looping);
    m->bytesPerFrame = static_cast<int>(targetSpec.bytesPerFrame());
    m->decoder = std::move(decoder);
    return True;
}

auto StreamSource::release_() -> void
{
    if (m)
    {
        m->decoder.close();
    }
}

auto StreamSource::readImpl(Ubyte *output, const Int64 length) -> Int64
{
    // Init guard
    if ( !isOpen() )
    {
        memory::set(output, 0, length);
        return length;
    }

    // Read the frames!
    const auto framesToRead = length / m->bytesPerFrame;
    const auto framesRead = m->decoder.readFrames(output, framesToRead);

    // Error check
    if (framesRead < 0)
    {
        release();
        std::memset(output, 0, length);
        return length;
    }

    // Ensure any remaining frame is filled with silence
    if (framesRead < framesToRead)
    {
        const auto bytesRead = framesRead * m->bytesPerFrame;
        memory::set(output + bytesRead, 0, length - bytesRead);
    }

    if (m->isOneShot && !m->decoder.isLooping())
    {
        if (m->decoder.isEnded())
        {
            release();
        }
    }

    return length;
}

auto StreamSource::getLooping() const -> Bool
{
    INIT_GUARD_RET(False);

    return m->looping;
}

auto StreamSource::setLooping(const Bool looping) -> Bool
{
    INIT_GUARD_RET(False);

    m->decoder.setLooping(looping);
    return True;
}

auto StreamSource::getPosition(const AudioTime::Unit units) const -> Double
{
    INIT_GUARD_RET(-1.0);
    return m->decoder.tell(units);
}

auto StreamSource::setPosition(
    const AudioTime::Unit units,
    const Uint64 position,
    const SeekBase base) -> Bool
{
    INIT_GUARD_RET(False);
    return m->decoder.seek(position, units, base);
}

auto StreamSource::init_(const StreamSourceInit &config) -> Bool
{
    if (!AudioSource::init_(config.context, config.parentClock, config.paused))
    {
        return False;
    }

    m->isOneShot = config.isOneShot;
    m->looping = config.isLooping;

    Bool result = False;
    if (config.pathOrMemory.index() == 0)
    {
        // Managed memory
        result = openMem(std::get<ManagedMem>(config.pathOrMemory));
    }
    else if (config.pathOrMemory.index() == 1)
    {
        // Const memory
        result = openConstMem(std::get<MemView<void>>(config.pathOrMemory));
    }
    else if (config.pathOrMemory.index() == 2)
    {
        // Filepath
        result = openFile(std::get<String>(config.pathOrMemory), config.inMemory);
    }

    return result;
}

KAUDIO_NS_END
