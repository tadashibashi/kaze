#pragma once
#ifndef kaze_core_io_bufferview_h_
#define kaze_core_io_bufferview_h_
#include <kaze/core/lib.h>
#include <kaze/core/concepts.h>
#include <kaze/core/debug.h>
#include <kaze/core/endian.h>
#include <kaze/core/MemView.h>

#include <limits>
#include <stdexcept>


KAZE_NAMESPACE_BEGIN
    struct SeekBase {
    enum Enum {
        Start,
        Relative,
        End
    };
};

/// Wraps const binary buffer data to analyze it
class BufferView {
public:
    /// Null buffer with nothing in it
    BufferView();

    /// \param[in] mem       const memory to view
    /// \param[in] endian    expected endianness of integral types for arithmetic read overloads
    explicit BufferView(Memory mem, Endian::Type endian = Endian::Little) noexcept;
    ~BufferView() = default;

    template <Arithmetic T>
    auto operator >> (T &other) -> BufferView &
    {
        if ( !*this || read(&other, sizeof(T), m_endian) != sizeof(T))
            throw std::runtime_error("BufferView read failed");
        return *this;
    }

    /// Get a null-terminated string
    auto operator >>(String &string) -> BufferView &;

    /// Read string. It copies until null terminator or maxLength has been reached.
    /// \note String is assumed to be in big-endian byte order.
    /// \param[out] string      string to capture string.
    /// \param[in]  maxLength   maximum number of chars to read
    /// \returns size of string copied, not including null terminator
    auto readString(String *string, Int64 maxLength = std::numeric_limits<Int64>::max()) -> Int64;

    template <typename T>
    [[nodiscard]]
    auto read() -> T;

    /// Read an arithmetic type
    template <Arithmetic T>
    [[nodiscard]]
    auto read() -> T
    {
        T value;
        if ( !*this || read(&value, sizeof(T), m_endian) != sizeof(T))
            throw std::runtime_error("BufferView read failed");
        return value;
    }

    /// read String overload
    template <>
    [[nodiscard]]
    auto read<String>() -> String
    {
        String str;
        if ( !readString(&str) )
            throw std::runtime_error("BufferView readString failed");
        return str;
    }

    template <typename T>
    auto tryRead(T *outValue) -> Bool;

    /// Attempt to read an arithmetic value
    /// \param[out] outValue   retrieves the arithemtic type
    /// \returns whether read succeeded;
    ///          may fail if there are not enough bytes remaining in the buffer or eof flag is already set
    template <Arithmetic T>
    auto tryRead(T *outValue) -> Bool
    {
        if ( !outValue )
        {
            KAZE_CORE_ERRCODE(Error::NullArgErr, "Required param `outValue` was null");
            return KAZE_FALSE;
        }

        if ( !*this || read(outValue, sizeof(T), m_endian) != sizeof(T))
        {
            KAZE_CORE_ERRCODE(Error::FileReadErr, "Failed to read from BufferView");
            return KAZE_FALSE;
        }

        return KAZE_TRUE;
    }

    /// Attempt to read a string value
    /// \param[out] outValue   retreives the string value
    /// \returns whether read succeeded;
    ///          will fail the eof flag is alreay set, or `outValue` is nullptr
    template <>
    auto tryRead<String>(String *outValue) -> Bool
    {
        if ( !outValue )
        {
            KAZE_CORE_ERRCODE(Error::NullArgErr, "Required param `outValue` was null");
            return KAZE_FALSE;
        }

        if ( !*this || !readString(outValue) )
        {
            KAZE_CORE_ERRCODE(Error::FileReadErr, "Failed to read from BufferView");
            return KAZE_FALSE;
        }

        return KAZE_TRUE;
    }

    /// Read bytes with target endianness of binary data into account
    /// \param[in]  data    data buffer to copy data to
    /// \param[in]  bytes   number of bytes to read
    /// \param[in]  endian  expected endianness of the target data
    auto read(void *data, Int64 bytes, Endian::Type endian = Endian::Native) noexcept -> Int64;

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
    auto seek(Int64 offset, SeekBase::Enum base = SeekBase::Start) -> BufferView &;

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
    Endian::Type m_endian;
};

KAZE_NAMESPACE_END

#endif // kaze_core_io_bufferview_h_
