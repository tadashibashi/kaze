#include "SoundBuffer.h"
#include <kaze/internal/audio/conv/AudioDecoder.h>

#include <kaze/internal/core/debug.h>
#include <kaze/internal/core/io/io.h>
#include <kaze/internal/core/memory.h>

KAUDIO_NS_BEGIN

const Int64 BytesPerRead = 1024;

static auto loadAudio(
    const MemView<void> mem,
    const AudioSpec &targetSpec,
    Ubyte **outBuffer,
    Size *outByteLength) -> Bool
{
    AudioDecoder decoder{};
    if ( !decoder.openConstMem(mem, targetSpec) )
    {
        return False;
    }

    const auto size = decoder.size();
    const auto outMem = memory::alloc(size);

    for (Int curPosition = 0; curPosition < size; )
    {
        const auto bytesRead = decoder.read((Ubyte *)outMem + curPosition, BytesPerRead);
        if (bytesRead < 0)
        {
            memory::free(outMem);
            return False;
        }

        curPosition += bytesRead;
    }

    if (outBuffer)
    {
        *outBuffer = (Ubyte *)outMem;
    }
    else
    {
        memory::free(outMem);
    }

    if (outByteLength)
        *outByteLength = static_cast<Size>(size);

    return True;
}

static auto loadAudio(
    const String &filepath, const
    AudioSpec &targetSpec,
    Ubyte **outBuffer,
    Size *outByteLength) -> Bool
{
    Ubyte *fileData;
    Size fileSize;
    if ( !file::load(filepath, &fileData, &fileSize) )
        return False;
    return loadAudio({fileData, fileSize}, targetSpec, outBuffer, outByteLength);
}

SoundBuffer::SoundBuffer() : m_bufferSize(), m_buffer(), m_spec()
{
}

SoundBuffer::SoundBuffer(const String &filepath, const AudioSpec &targetSpec) :
    m_bufferSize(), m_buffer(), m_spec()
{
    load(filepath, targetSpec);
}

SoundBuffer::~SoundBuffer()
{
    unload();
}

SoundBuffer::SoundBuffer(SoundBuffer &&other) noexcept :
    m_bufferSize(other.m_bufferSize), m_buffer(other.m_buffer.load()), m_spec(other.m_spec)
{
    other.m_spec = {};
    other.m_bufferSize = 0;
    other.m_buffer.store(nullptr, std::memory_order_release);
}

SoundBuffer &SoundBuffer::operator=(SoundBuffer &&other) noexcept
{
    if (this != &other)
    {
        // Clean up existing data
        unload();

        // Move other SoundBuffer data over here
        m_spec = other.m_spec;
        m_bufferSize = other.m_bufferSize;
        other.m_buffer.store(
            other.m_buffer.load(std::memory_order_acquire),
            std::memory_order_release);

        // Invalidate other SoundBuffer
        other.m_spec = {};
        other.m_bufferSize = 0;
        other.m_buffer.store(nullptr, std::memory_order_release);
    }

    return *this;
}

auto SoundBuffer::load(const String &filepath, const AudioSpec &targetSpec) -> Bool
{
    Ubyte *buffer;
    Size byteLength;
    if (!loadAudio(filepath, targetSpec, &buffer, &byteLength))
    {
        return false;
    }

    emplace({buffer, byteLength}, targetSpec);
    return true;
}

auto SoundBuffer::load(MemView<void> mem, const AudioSpec &targetSpec) -> Bool
{
    Ubyte *buffer;
    Size byteLength;
    if (!loadAudio(mem, targetSpec, &buffer, &byteLength))
        return false;

    emplace({buffer, byteLength}, targetSpec);
    return true;
}

auto SoundBuffer::unload() -> void
{
    if (isLoaded())
    {
        auto oldBuffer = m_buffer.load();
        while(!m_buffer.compare_exchange_weak(oldBuffer, nullptr)) { }

        m_bufferSize = 0;
        memory::free(oldBuffer);
    }
}

auto SoundBuffer::emplace(MemView<void> mem, const AudioSpec &spec) -> void
{
    m_bufferSize = mem.size();

    const auto buffer = (Ubyte *)mem.data();
    auto oldBuffer = m_buffer.load(std::memory_order_relaxed);

    if (buffer == oldBuffer) return;

    while(!m_buffer.compare_exchange_weak(oldBuffer, buffer)) { }

    if (oldBuffer != nullptr)
    {
        memory::free(oldBuffer);
    }

    m_spec = spec;
}

KAUDIO_NS_END
