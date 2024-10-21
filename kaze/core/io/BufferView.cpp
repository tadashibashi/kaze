#include "BufferView.h"

#include <kaze/core/debug.h>
#include <kaze/core/math/mathf.h>
#include <kaze/core/memory.h>


KAZE_NAMESPACE_BEGIN

BufferView::BufferView() : m_begin(), m_end(), m_head(), m_isEof(false), m_endian(Endian::Native)
{ }

BufferView::BufferView(const Memory mem, const Endian::Type endian) noexcept :
    m_begin(static_cast<const Ubyte *>(mem.data())),
    m_end(static_cast<const Ubyte *>(mem.data()) + mem.size()),
    m_head(static_cast<const Ubyte *>(mem.data())),
    m_isEof(),
    m_endian(endian)
{ }

auto BufferView::operator >>(String &string) -> BufferView &
{
    readString(&string);
    return *this;
}

auto BufferView::readString(String *string, Int64 maxLength) -> Int64
{
    if ( !string )
    {
        KAZE_CORE_ERRCODE(Error::NullArgErr, "Required param `string` was null");
        return 0;
    }

    // find length
    auto cur = m_head;
    while (cur - m_head < maxLength && cur < m_end && *cur != 0)
    {
        ++cur;
    }

    const auto length = cur - m_head;
    string->assign(length, 0);
    if (length > 0)
        std::memcpy(string->data(), m_head, length);

    if (cur < m_end && *cur == '\0') // move past null terminator
        ++cur;
    m_head = cur;
    return static_cast<Int64>(length);
}

auto BufferView::read(void *data, Int64 bytes, Endian::Type endian) noexcept -> Int64
{
    if (m_isEof)
    {
        KAZE_CORE_ERRCODE(Error::FileReadErr, "Attempted to read from BufferView in eof state");
        return 0;
    }

    if (m_head >= m_end)
    {
        m_isEof = true;
        m_head = m_end;
        return 0;
    }

    // Calculate number of bytes to read
    Int64 bytesToRead = mathf::min<Int64>(m_end - m_head, bytes);
    if (bytesToRead <= 0)
    {
        return 0; // prevent a zero-byte copy for some reason
    }

    // Copy data depending on endianness
    if (endian != Endian::Native)
    {
        auto dest = static_cast<Ubyte *>(data);

        // reverse endian read
        for (auto curByte = m_head + (bytesToRead - 1); curByte >= m_head; --curByte, ++dest)
        {
            *dest = *curByte;
        }
    }
    else
    {
        KAZE_NAMESPACE::memory::copy(data, m_head, bytesToRead);
    }

    // Progress head
    if (bytesToRead < bytes)
    {
        m_isEof = true;
        m_head = m_end;
    }
    else
    {
        m_head += bytesToRead;
    }

    // Done
    return bytesToRead;
}

auto BufferView::seek(Int64 offset, SeekBase::Enum base) -> BufferView &
{
    // Seek by base
    switch (base)
    {
    case SeekBase::Start:     m_head = m_begin + offset; break;
    case SeekBase::Relative:  m_head = m_head + offset;  break;
    case SeekBase::End:       m_head = m_end + offset;   break;
    default:                  return *this;
    }

    // Set eof flag
    if (m_head >= m_end)
        m_isEof = true;
    else if (m_isEof && m_head < m_end)
        m_isEof = false;

    // Clamp head
    if (m_head < m_begin)
        m_head = m_begin;
    if (m_head > m_end)
        m_head = m_end;

    return *this;
}

BufferView::operator bool()
{
    return m_begin != nullptr && !m_isEof;
}

KAZE_NAMESPACE_END


