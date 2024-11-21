/// \file BufferIO.h
/// Contains static functions for serialization and deserialization of binary data
#pragma once

#include <kaze/internal/core/lib.h>
#include <kaze/internal/core/debug.h>
#include <kaze/internal/core/io/BufferWriter.h>
#include <kaze/internal/core/io/BufferView.h>
#include <kaze/internal/core/io/StructLayout.h>

KAZE_NS_BEGIN

/// For now this object just contains static functions for conversion
/// between binary and structs
class StructIO {
public:
    StructIO() { }
    ~StructIO() = default;

    /// Read a struct layout from binary data into memory
    /// \tparam       T       Type to read; must match type in `layout`.
    /// \param[inout] view    buffer to read from
    /// \param[in]    layout  layout mapping the data
    /// \throws std::runtime_error if an error occured during read
    template <typename T>
        requires std::is_default_constructible_v<T>
    static auto read(BufferView &view, const StructLayout &layout) -> T
    {
        auto t = T();

        clearError();
        read(&t, view, layout);
        if ( const auto err = getError(); err.code != Error::Ok )
        {
            throw std::runtime_error(err.message);
        }

        return t;
    }

    /// Read a struct layout from binary data into memory
    /// \param[inout] dest    data to populate, should be StructLayout class
    /// \param[inout] view    buffer to read from
    /// \param[in]    layout  layout mapping the data
    template <typename T>
    static auto read(T *dest, BufferView &view, const StructLayout &layout) -> Size
    {
        if (layout.getClassType() != typeid(T))
        {
            KAZE_PUSH_ERR(Error::InvalidArgErr, "BufferIO::read: type T must match the "
                "StructLayout type. Expected {}, but got {}",
                layout.getClassType().name(), typeid(T).name());
            return 0;
        }

        return readLayout(dest, view, layout);
    }

    template <typename T>
    static auto write(const T *src, BufferWriter &writer, const StructLayout &layout) -> Size
    {
        if (layout.getClassType() != typeid(T))
        {
            KAZE_PUSH_ERR(Error::InvalidArgErr, "BufferIO::read: type T must match the "
                "StructLayout type. Expected {}, but got {}",
                layout.getClassType().name(), typeid(T).name());
            return 0;
        }

        return writeLayout(src, writer, layout);
    }

private:
    static auto readLayout(void *dest, BufferView &view, const StructLayout &layout) -> Size;
    static auto writeLayout(const void *src, BufferWriter &writer, const StructLayout &layout) -> Size;
};

KAZE_NS_END
