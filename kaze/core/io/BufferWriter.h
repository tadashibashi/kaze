#pragma once
#include <kaze/core/lib.h>
#include <kaze/core/concepts.h>
#include <kaze/core/endian.h>
#include <kaze/core/memory.h>
#include <kaze/core/MemView.h>

KAZE_NAMESPACE_BEGIN

struct BufferWriterInit {
    Size initialSize = 1024;
    Endian::Type arithmeticEndian = Endian::Little;
    Endian::Type stringEndian = Endian::Big;
};

struct BufferWriterStringOpts {
    /// If this is set, any length beyond the string will be zeroed in memory;
    /// If not, it will maintain the same size as the string + 1 for the null terminator.
    Size fixedBufSize   = std::numeric_limits<Size>::max();

    /// Endian override for this write
    Endian::Type endian = Endian::Unknown;
};

class BufferWriter {
public:
    explicit BufferWriter(const BufferWriterInit &initConfig = {});
    ~BufferWriter();

    /// Write a string to the buffer
    /// \param[in]  str          string to write
    /// \param[in]  strLength    number of characters from string to write
    /// \param[in]  opts         [optional, default: \see `BufferWriterStringOpts`]
    /// \returns number of bytes written
    auto writeString(const char *str, Size strLength, const BufferWriterStringOpts &opts = {}) -> Size;

    /// Write a number to the buffer
    /// \param[in]  num       pointer to numeric type to write to the buffer
    /// \param[in]  byteSize  size of numeric type
    /// \param[in]  endian    override endianness to write to the buffer [optional, default: `arithemticEndian` value set in ctor]
    auto writeNumber(const void *num, const Size byteSize, Endian::Type endian = Endian::Unknown) -> Size
    {
        endian = endian == Endian::Unknown ? m_arithmeticEndian : endian;
        return writeRaw(num, byteSize, endian != Endian::Native);
    }

    /// Write raw data to the buffer
    /// \param[in]  data       buffer with data to write
    /// \param[in]  byteCount  number of bytes from buffer to write
    /// \param[in]  reverse    whether to reverse endianness of write [optional, default: `false`]
    /// \returns number of bytes written. Should equal `byteCount`, if not, an error occurred.
    auto writeRaw(const void *data, Size byteCount,
                  Bool reverse = KAZE_FALSE) -> Size;

    /// Fill bytes with a number, similar to memset
    /// \param[in]  value      value to write in each byte
    /// \param[in]  byteCount  number of bytes `value` gets written to, starting at current head.
    auto writeFill(Ubyte value, Size byteCount) -> Size;

    /// Reserve memory in advance. If you can approximate the target size, this can help prevent reallocations.
    auto reserve(Size newSize) -> Bool;

    /// Move write head backward
    /// \param[in]  position  byte position in buffer to move to
    /// \returns false if position was greater or equal to the current position, true if write head was moved
    auto skipBackTo(Size position) -> Bool {
        if (position < size())
        {
            m_head = m_mem + position;
            return KAZE_TRUE;
        }

        return KAZE_FALSE;
    }

    /// Clear container
    auto clear() noexcept -> void { m_head = m_mem; }

    /// \returns pointer to the data buffer, which may be invalidated on reallocation.
    /// \note if checking data, make sure to get a fresh pointer after the last call to write.
    auto data() noexcept -> Ubyte * { return m_mem; }

    /// \returns pointer to the data buffer, which may be invalidated on reallocation.
    /// \note if checking data, make sure to get a fresh pointer after the last call to write.
    auto data() const noexcept -> const Ubyte * { return m_mem; }

    /// \returns current number of bytes written in the buffer
    auto size() const noexcept -> Size { return static_cast<Size>(m_head - m_mem); }

    /// \returns whether no bytes have been written to the buffer
    auto empty() const noexcept -> Bool { return m_mem == m_head; }

    /// \returns the full size of the internal buffer; it's likely to be larger than `size()` bytes written
    auto maxSize() const noexcept -> Size { return m_end - m_mem; }

    /// Write a string to the buffer
    /// \param[in]  str   string to write
    /// \returns this object.
    /// \throws std::runtime_error if an error occurred during the write
    auto operator<<(const StringView str) -> BufferWriter &
    {
        if (writeString(str.data(), str.size()) != str.size() + 1)
            throw std::runtime_error(getError().message);
        return *this;
    }

    /// Write a string to the buffer
    /// \param[in]  str   string to write
    /// \returns number of bytes written
    auto write(StringView str, const BufferWriterStringOpts &opts = {}) -> Size
    {
        return writeString(str.data(), str.size(), opts);
    }

    /// Write a string to the buffer
    /// \param[in]  str       string to write
    /// \param[in]  strLength length of the string to write
    /// \returns number of bytes written
    auto write(const char *str, Size strLength, const BufferWriterStringOpts &opts = {}) -> Size
    {
        return writeString(str, strLength, opts);
    }

    /// Write an arithmetic number type to the buffer
    /// \param[in]  n  number to write
    /// \returns this object.
    /// \throws std::runtime_error if an error occurred during the write
    template <Arithmetic T>
    auto operator<< (T n) -> BufferWriter &
    {
        if ( !writeNumber(&n, sizeof(T), m_arithmeticEndian) )
            throw std::runtime_error(getError().message);
        return *this;
    }

    /// Write an arithmetic number type to the buffer
    /// \param[in]  n  number to write
    /// \returns this object.
    /// \throws std::runtime_error if an error occurred during the write
    template <typename T> requires std::is_enum_v<T>
    auto operator<< (T n) -> BufferWriter &
    {
        using Underlying = std::underlying_type_t<T>;

        if ( !writeNumber((Underlying *)&n, sizeof(Underlying), m_arithmeticEndian) )
            throw std::runtime_error(getError().message);
        return *this;
    }

    /// Write an arithmetic number type to the buffer
    /// \param[in]  n       number to write
    /// \param[in]  endian  endianness to write [optional, default: arithmeticEndian value set in ctor]
    /// \returns number of bytes written.
    template <Arithmetic T>
    auto write(T n, Endian::Type endian = Endian::Unknown) -> Size
    {
        return writeNumber(&n, sizeof(T), endian);
    }

    /// Write an enum type to the buffer
    /// \param[in]  n       number to write
    /// \param[in]  endian  endianness to write [optional, default: arithmeticEndian value set in ctor]
    /// \returns number of bytes written.
    template <typename T> requires std::is_enum_v<T>
    auto write(T n, Endian::Type endian = Endian::Unknown) -> Size
    {
        return writeNumber(&n, sizeof(T), endian);
    }

    [[nodiscard]]
    auto defaultStringEndian() const noexcept -> Endian::Type { return m_stringEndian; }

    [[nodiscard]]
    auto defaultArithmeticEndian() const noexcept -> Endian::Type { return m_arithmeticEndian; }
private:
    Ubyte *m_mem{}, *m_head{}, *m_end{};
    Endian::Type m_arithmeticEndian = Endian::Little;
    Endian::Type m_stringEndian = Endian::Big;
};

KAZE_NAMESPACE_END
