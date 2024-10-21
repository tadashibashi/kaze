#include "FileBuffer.h"

#include <kaze/core/io/io.h>

KAZE_NAMESPACE_BEGIN

FileBuffer::FileBuffer() : m_data(), m_dataSize() { }
FileBuffer::~FileBuffer()
{
    close();
}

FileBuffer::FileBuffer(const StringView filepath)  : m_data(), m_dataSize()
{
    open(filepath);
}

FileBuffer::FileBuffer(const FileBuffer &other) : m_data(memory::alloc<Ubyte>(other.m_dataSize)), m_dataSize(other.m_dataSize)
{
    if (m_data)
    {
        kaze::memory::copy(m_data, other.m_data, other.m_dataSize);
    }
}

auto FileBuffer::operator=(const FileBuffer &other) -> FileBuffer &
{
    close();
    m_data = memory::alloc<Ubyte>(other.m_dataSize);
    if (m_data)
    {
        memory::copy(m_data, other.m_data, other.m_dataSize);
        m_dataSize =other.m_dataSize;
    }

    return *this;
}

FileBuffer::FileBuffer(FileBuffer &&other) noexcept : m_data(other.m_data), m_dataSize(other.m_dataSize)
{
    other.m_data = nullptr;
    other.m_dataSize = 0;
}

auto FileBuffer::operator=(FileBuffer &&other) noexcept -> FileBuffer &
{
    if (this == &other) return *this;

    m_data = other.m_data;
    m_dataSize = other.m_dataSize;

    other.m_data = nullptr;
    other.m_dataSize = 0;

    return *this;
}

auto FileBuffer::open(const StringView filepath) -> Bool
{
    Ubyte *data;
    Size dataSize;
    if ( !loadFile(filepath, &data, &dataSize) )
    {
        return KAZE_FALSE;
    }

    close();
    m_data = data;
    m_dataSize = dataSize;
    return KAZE_TRUE;
}

auto FileBuffer::close() -> void
{
    if (m_data)
    {
        memory::free(m_data);
        m_data = nullptr;
        m_dataSize = 0;
    }
}

KAZE_NAMESPACE_END
