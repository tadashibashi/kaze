#include "BufferView.h"

#include <kaze/internal/core/debug.h>
#include <kaze/internal/core/math/mathf.h>
#include <kaze/internal/core/memory.h>

KAZE_NS_BEGIN

BufferView::BufferView(const MemView<void> mem, const BufferViewOpts &opts) noexcept :
    m_begin(static_cast<const Ubyte *>(mem.data())),
    m_end(static_cast<const Ubyte *>(mem.data()) + mem.size()),
    m_head(static_cast<const Ubyte *>(mem.data())),
    m_isEof(),
    m_arithmeticEndian(opts.arithmeticEndian),
    m_stringEndian(opts.stringEndian)
{ }

BufferView::BufferView(const void *mem, Size byteSize, const BufferViewOpts &opts) noexcept :
    BufferView(makeRef(mem, byteSize), opts)
{ }

BufferView::BufferView() : BufferView(MemView<void>{}) { }

auto BufferView::operator >>(String &string) -> BufferView &
{
    read(&string);
    return *this;
}

auto BufferView::read(String *string, const BufferViewReadStringOpts &opts) -> Int64
{
    const auto endian = opts.endian == endian::Unknown ? m_stringEndian : opts.endian;

    if ( !string )
    {
        KAZE_PUSH_ERR(Error::NullArgErr, "Required param `string` was null");
        return 0;
    }

    // find length
    auto cur = m_head;
    while (cur - m_head < opts.maxLength && cur < m_end && *cur != 0)
    {
        ++cur;
    }

    const auto length = cur - m_head;
    string->assign(length, 0);

    if (length > 0)
    {
        if (endian == endian::Big)
        {
            std::memcpy(string->data(), m_head, length);
        }
        else
        {
            auto dest = (Ubyte *)string->data();
            for (auto src = cur - 1; src >= m_head; --src, ++dest)
            {
                *dest = *src;
            }
        }
    }

    if (cur < m_end && *cur == '\0') // move past null terminator
        ++cur;
    m_head = cur;
    return static_cast<Int64>(length);
}

auto BufferView::read(String *string, Int64 length,
                                 const BufferViewReadStringOpts &opts) -> Int64
{
    if ( !string )
    {
        KAZE_PUSH_ERR(Error::NullArgErr, "Required arg `string` was null");
        return 0;
    }

    length = length < opts.maxLength ? length : opts.maxLength;

    const auto endian = opts.endian == endian::Unknown ? m_stringEndian : opts.endian;
    const auto reverse = endian == endian::Little;

    return read(string->data(), length, reverse);
}

auto BufferView::read(void *data, Int64 bytes, Bool reverse) noexcept -> Int64
{
    if (m_isEof)
    {
        KAZE_PUSH_ERR(Error::FileReadErr, "Attempted to read from BufferView in eof state");
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
    if (reverse)
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
        memory::copy(data, m_head, bytesToRead);
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

auto BufferView::seek(Int64 offset, SeekBase base) -> BufferView &
{
    // Seek by base
    switch (base)
    {
    case SeekBase::Begin:     m_head = m_begin + offset; break;
    case SeekBase::Current:   m_head = m_head + offset;  break;
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

KAZE_NS_END
