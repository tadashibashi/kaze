#include "RstreamableMemory.h"
#include <kaze/internal/core/math/mathf.h>
#include <kaze/internal/core/memory.h>
#include <kaze/internal/core/io/io.h>

KAZE_NS_BEGIN

static auto defaultDealloc(void *mem, void *userptr)
{
    memory::free(mem);
}

auto RstreamableMemory::cleanupData() -> void
{
   if (m_data && m_deallocator)
    {
        try {
            m_deallocator(m_data, m_userptr);
        }
        catch(const std::exception &e)
        {
            KAZE_CORE_WARN("Error was thrown during RstreamableMemory deallocator callback. "
                "Data may have been leaked: {}", e.what());
        }
        catch(...)
        {
            KAZE_CORE_WARN("Error was thrown during RstreamableMemory deallocator callback. "
                "Data may have been leaked.");
        }
    }
}

auto RstreamableMemory::openFile(const String &path) -> Bool
{
    Ubyte *data;
    Size size;
    if ( !file::load(path, &data, &size) )
    {
        return False;
    }

    cleanupData();

    m_data = data;
    m_end = data + size;
    m_head = data;
    m_eof = False;
    m_deallocator = defaultDealloc;
    m_userptr = Null;
    return True;
}

auto RstreamableMemory::openMem(ManagedMem mem) -> Bool
{
    if ( !mem.data() )
    {
        KAZE_PUSH_ERR(Error::InvalidArgErr, "Null memory passed to RstreamableMemory::openMem");
        return False;
    }

    cleanupData();

    m_data = (Ubyte *)mem.data();
    m_end = m_data + mem.size();
    m_head = m_data;
    m_eof = False;
    m_deallocator = mem.deallocator();
    m_userptr = mem.userptr();
    return True;
}

auto RstreamableMemory::openConstMem(MemView<void> mem) -> Bool
{
    if ( !mem.data() )
    {
        KAZE_PUSH_ERR(Error::InvalidArgErr, "Null memory passed to RstreamableMemory::openMem");
        return False;
    }

    cleanupData();

    m_data = (Ubyte *)const_cast<void *>(mem.data());
    m_end = m_data + mem.size();
    m_head = m_data;
    m_eof = False;
    m_deallocator = Null;
    m_userptr = Null;
    return True;
}

auto RstreamableMemory::close() -> void
{
    cleanupData();

    m_data = Null;
    m_end = Null;
    m_head = Null;
    m_eof = False;
    m_deallocator = Null;
}

auto RstreamableMemory::isOpen() const -> Bool
{
    return m_data != Null;
}

auto RstreamableMemory::size() const -> Int64
{
    return static_cast<Int64>(m_end - m_data);
}

auto RstreamableMemory::tell() const -> Int64
{
    return static_cast<Int64>(m_head - m_data);
}

auto RstreamableMemory::isEof() const -> Bool
{
    return m_eof;
}

auto RstreamableMemory::read(void *buffer, Int64 bytes) -> Int64
{
    if ( !isOpen() )
    {
        KAZE_PUSH_ERR(Error::LogicErr, "RstreamableMemory::read attempted while unopened");
        return -1LL;
    }

    if (bytes < 0)
    {
        KAZE_PUSH_ERR(Error::InvalidArgErr, "Invalid bytes requested, must be >= 0, but got {}",
            bytes);
        return -1LL;
    }

    if (m_head < m_data) // It shouldn't happen, but just in case
    {
        KAZE_PUSH_ERR(Error::RuntimeErr, "RstreamableMemory position is invalid");
        return -1LL;
    }

    if (bytes == 0)
        return 0;

    if (m_eof)
        return 0;

    if (m_head >= m_end) // Read was in eof state
    {
        m_eof = True;
        m_head = m_end;
        return 0;
    }

    const Int64 bytesToRead = mathf::min(bytes, static_cast<Int64>(m_end - m_head));
    if ( !buffer ) // just seek if there's no buffer provided
    {
        if ( !seek(bytesToRead, SeekBase::Current) )
            return -1;
        return bytesToRead;
    }

    // Copy the bytes
    memory::copy(buffer, m_head, bytesToRead);
    if (bytesToRead < bytes)
    {
        m_eof = true;
    }

    m_head += bytesToRead;
    return bytesToRead;
}

auto RstreamableMemory::seek(Int64 position, SeekBase base) -> Bool
{
    if ( !isOpen() )
    {
        KAZE_PUSH_ERR(Error::LogicErr, "RstreamableMemory::seek attempted while unopened");
        return False;
    }

    const auto byteSize = size();

    Int64 finalPosition;
    switch(base)
    {
    case SeekBase::Begin:   finalPosition = 0; break;
    case SeekBase::Current: finalPosition = tell(); break;
    case SeekBase::End:     finalPosition = byteSize; break;
    default:
        KAZE_CORE_WARN("Invalid SeekBase passed to RstreamableMemory::seek, falling back "
            "to SeekBase::Begin");
        finalPosition = 0;
        break;
    }

    finalPosition += position;
    if (finalPosition > byteSize || finalPosition < 0)
    {
        KAZE_PUSH_ERR(Error::OutOfRange, "RstreamableMemory::seek position is out of range");
        return False;
    }

    m_eof = False;
    m_head = m_data + finalPosition;
    return True;
}

KAZE_NS_END
