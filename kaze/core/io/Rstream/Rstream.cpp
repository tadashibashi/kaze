#include "Rstream.h"
#include "RstreamableFile.h"
#include "RstreamableMemory.h"

#include <kaze/core/platform/defines.h>

#if KAZE_PLATFORM_ANDROID
#include "RstreamableAAsset.h"
#endif

KAZE_NS_BEGIN

Rstream::~Rstream()
{
    delete m_stream;
}

Rstream::Rstream(Rstream &&other) : m_stream(other.m_stream)
{
    other.m_stream = Null;
}

auto Rstream::operator= (Rstream &&other) -> Rstream &
{
    if (this == &other) return *this;

    delete m_stream;

    m_stream = other.m_stream;
    other.m_stream = Null;

    return *this;
}

auto Rstream::openFile(const String &path, Bool inMemory) -> Bool
{
#if KAZE_PLATFORM_ANDROID
    if (path.starts_with("apk://") && path.size() > 6)
    {
        auto stream = new RstreamableAAsset();
        if ( !stream->openFile(path.substr(6), inMemory) )
        {
            delete stream;
            return False;
        }

        delete m_stream;
        m_stream = stream;
        return True;
    }
#endif

    if (inMemory)
    {
        auto stream = new RstreamableMemory();
        if ( !stream->openFile(path.c_str()) )
        {
            delete stream;
            return False;
        }

        delete m_stream;
        m_stream = stream;
        return True;
    }
    else
    {
        auto stream = new RstreamableFile();
        if ( !stream->openFile(path) )
        {
            delete stream;
            return False;
        }

        delete m_stream;
        m_stream = stream;
        return True;
    }
}

auto Rstream::openMem(MemView<void> mem, funcptr_t<void(void *mem)> deallocator) -> Bool
{
    const auto stream = new RstreamableMemory();
    if ( !stream->openMem(mem, deallocator))
    {
        delete stream;
        return False;
    }

    m_stream = stream;
    return True;
}

auto Rstream::openConstMem(MemView<const void> mem) -> Bool
{
    const auto stream = new RstreamableMemory();
    if ( !stream->openConstMem(mem) )
    {
        delete stream;
        return False;
    }

    delete m_stream;
    m_stream = stream;
    return True;
}


auto Rstream::close() -> void
{
    if (m_stream)
    {
        delete m_stream;
        m_stream = Null;
    }
}

auto Rstream::isOpen() const -> Bool
{
    return m_stream && m_stream->isOpen();
}

auto Rstream::size() const -> Int64
{
    if ( !m_stream )
    {
        KAZE_PUSH_ERR(Error::LogicErr, "Attempted to get size of unopened Rstream");
        return -1LL;
    }

    return m_stream->size();
}

auto Rstream::tell() const -> Int64
{
    if ( !m_stream )
    {
        KAZE_PUSH_ERR(Error::LogicErr, "Attempted to call `tell` on unopened Rstream");
        return -1LL;
    }

    return m_stream->tell();
}

auto Rstream::isEof() const -> Bool
{
    if ( !m_stream )
    {
        KAZE_PUSH_ERR(Error::LogicErr, "Attempted to get `size` of unopened Rstream");
        return False;
    }

    return m_stream->isEof();
}

auto Rstream::read(void *buffer, Int64 bytes) -> Int64
{
    if ( !m_stream )
    {
        KAZE_PUSH_ERR(Error::LogicErr, "Attempted to call `read` on unopened Rstream");
        return False;
    }

    return m_stream->read(buffer, bytes);
}

auto Rstream::seek(Int64 position, SeekBase base) -> Bool
{
    if ( !m_stream )
    {
        KAZE_PUSH_ERR(Error::LogicErr, "Attempted to call `seek` on unopened Rstream");
        return False;
    }

    return m_stream->seek(position, base);
}

KAZE_NS_END
