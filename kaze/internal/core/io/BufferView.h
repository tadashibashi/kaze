#pragma once
#include <kaze/internal/core/io/stream/SeekBase.h>
#include <kaze/internal/core/lib.h>
#include <kaze/internal/core/concepts.h>
#include <kaze/internal/core/debug.h>
#include <kaze/internal/core/endian.h>
#include <kaze/internal/core/MemView.h>

#include <limits>
#include <stdexcept>

KAZE_NS_BEGIN

struct BufferViewOpts {
    endian::Type arithmeticEndian = endian::Little;
    endian::Type stringEndian     = endian::Big;
};

struct BufferViewReadStringOpts {
    /// Maximum string length to read [optional, default: Int64 max]
    Int64 maxLength = std::numeric_limits<Int64>::max();

    /// Endian override [optional]
    endian::Type endian = endian::Unknown;
};

/// Wraps const binary buffer data to analyze it
class BufferView {
public:
    /// Create a null BufferView
    BufferView();

    /// \param[in] mem     memory to view
    /// \param[in] opts    initialization options
    explicit BufferView(MemView<void> mem, const BufferViewOpts &opts = {}) noexcept;
    BufferView(const void *mem, Size byteSize, const BufferViewOpts &opts = {}) noexcept;
    ~BufferView() = default;

    template <Arithmetic T>
    auto operator >> (T &other) -> BufferView &
    {
        if ( !*this || readNumber(&other, sizeof(T), m_arithmeticEndian) != sizeof(T))
            throw std::runtime_error("BufferView read failed");
        return *this;
    }

    /// Get a null-terminated string. String is assumed to be in big-endian order.
    auto operator >>(String &string) -> BufferView &;

    /// Read string. It copies until null terminator or maxLength has been reached.
    /// \param[out] string  string to populate
    /// \param[in]  opts    maximum number of chars to read [optional] (check opts struct for defaults)
    /// \returns size of string copied, not including null terminator
    auto read(String *string, const BufferViewReadStringOpts &opts = {}) -> Int64;

    /// Read a string with a fixed length
    auto read(String *string, Int64 length,
                   const BufferViewReadStringOpts &opts = {}) -> Int64;

    template <typename T, typename... Args>
    auto read(Args&&... args) -> T;

    /// Read an arithmetic type
    template <Arithmetic T>
    [[nodiscard]]
    auto read() -> T
    {
        T value;
        if ( !*this ||
          readNumber(&value, sizeof(T), m_arithmeticEndian) != sizeof(T) )
            throw std::runtime_error("BufferView read failed");
        return value;
    }

    [[nodiscard]]
    auto readString() -> String
    {
        String str;
        if ( !read(&str) )
            throw std::runtime_error("BufferView readString failed");
        return str;
    }

    [[nodiscard]]
    auto readString(const BufferViewReadStringOpts &opts) -> String
    {
        String str;
        if ( !read(&str, opts) )
            throw std::runtime_error("BufferView readString failed");
        return str;
    }

    /// Attempt to read an arithmetic value
    /// \param[out] outValue   retrieves the arithemtic type
    /// \returns whether read succeeded;
    ///          may fail if there are not enough bytes remaining in the buffer or eof flag is already set
    template <Arithmetic T>
    auto tryRead(T *outValue) -> Bool
    {
        if ( !outValue )
        {
            KAZE_PUSH_ERR(Error::NullArgErr, "Required param `outValue` was null");
            return KAZE_FALSE;
        }

        if ( !*this || readNumber(outValue, sizeof(T), m_arithmeticEndian) != sizeof(T))
        {
            KAZE_PUSH_ERR(Error::FileReadErr, "Failed to read from BufferView");
            return KAZE_FALSE;
        }

        return KAZE_TRUE;
    }

    /// Attempt to read a string value
    /// \param[out] outValue   retreives the string value
    /// \param[in]  opts       read options
    /// \returns whether read succeeded;
    ///          will fail the eof flag is alreay set, or `outValue` is nullptr
    auto tryRead(String *outValue, const BufferViewReadStringOpts &opts = {}) -> Bool
    {
        if ( !outValue )
        {
            KAZE_PUSH_ERR(Error::NullArgErr, "Required param `outValue` was null");
            return KAZE_FALSE;
        }

        if ( !*this || !read(outValue, opts) )
        {
            KAZE_PUSH_ERR(Error::FileReadErr, "Failed to read from BufferView");
            return KAZE_FALSE;
        }

        return KAZE_TRUE;
    }

    /// Read bytes
    /// \param[in]  data    data buffer to copy data to
    /// \param[in]  bytes   number of bytes to read
    /// \param[in]  reverse reverse byte order
    auto read(void *data, Int64 bytes, Bool reverse = KAZE_FALSE) noexcept -> Int64;

    /// Read numeric type
    /// \param[inout] data   buffer to populate with data
    /// \param[in]    bytes  number of bytes to read
    /// \param[in]    endian target endianness of the numeric type [optional, default: arithmeticEndian set in ctor]
    auto readNumber(void *data, Int64 bytes, endian::Type endian = endian::Unknown) noexcept -> Int64
    {
        return read(data, bytes, (endian == endian::Unknown ? m_arithmeticEndian : endian) != endian::Native);
    }

    /// Get the size of the const buffer
    [[nodiscard]]
    auto size() const noexcept { return m_end - m_begin; }

    /// Get the position of the read head relative to the buffer starting point
    [[nodiscard]]
    auto tell() const noexcept { return m_head - m_begin; }

    /// Check the end-of-file flag. It gets set if a read goes beyond the end of file.
    [[nodiscard]]
    auto eof() const noexcept -> Bool { return m_isEof; }

    /// Seek to a position in the data. Resets the eof flag if it was set before calling this function.
    auto seek(Int64 offset, SeekBase base = SeekBase::Begin) -> BufferView &;

    /// Checks the validity of the buffer. Returns false if internal buffer is null or eof flag was set.
    [[nodiscard]]
    operator bool();

    /// Get the current read head pointer
    [[nodiscard]]
    auto head() const -> const Ubyte * { return m_head; }
    [[nodiscard]]
    auto begin() const -> const Ubyte * { return m_begin; }
    [[nodiscard]]
    auto end() const -> const Ubyte * { return m_end; }

  private:
    const Ubyte *m_begin{}, *m_end{}, *m_head{};
    bool m_isEof {false};
    endian::Type m_arithmeticEndian;
    endian::Type m_stringEndian;
};

KAZE_NS_END
