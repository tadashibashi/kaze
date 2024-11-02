/// \file endian.h
/// Contains definitions for checking endianness and related functionality
#pragma once
#include <kaze/core/lib.h>
#include <bit>

KAZE_NS_BEGIN

struct Endian
{
    enum Type
    {
        Unknown = -1,
        Little = static_cast<int>(std::endian::little),
        Big = static_cast<int>(std::endian::big),
    };

    /// Native platform's endian value, may be either Endian::Little or Endian::Big
    static constexpr Type Native = static_cast<Type>(std::endian::native);

    /// \returns `true` if the system's endianness is big-endian
    static constexpr Bool isBig() { return Native == Big; }

    /// \returns if the system's endianness is little-endian
    static constexpr Bool isLittle() { return Native == Little; }

    /// Get the opposite of an `Endian::Type`
    /// \param[in] endian  endianness to get the opposite of
    /// \returns the swapped endianness from an Endian enum
    ///          or `Endian::Unknown` if value is unrecognized
    static constexpr auto opposite(Type endian) -> Type {
        switch(endian)
        {
        case Little: return Big;
        case Big:    return Little;
        default:     return Unknown;
        }
    }

    /// Reverse an object's byte order
    /// \tparam     T     type of the object to swap
    /// \param[in]  obj   object to swap endianness of
    /// \returns a copy of the object in reverse byte order
    template <typename T>
    static constexpr T swap(T obj)
    {
        union
        {
            T t;
            Ubyte u8[sizeof(T)];
        } source, dest;

        source.t = obj;

        Size k = 0;

        // Loop unrolled in 4's
        for (constexpr Size ChunkSize = 4; k + ChunkSize <= sizeof(T); k += ChunkSize)
        {
            dest.u8[k + 0] = source.u8[sizeof(T) - 1 - (k + 0)];
            dest.u8[k + 1] = source.u8[sizeof(T) - 1 - (k + 1)];
            dest.u8[k + 2] = source.u8[sizeof(T) - 1 - (k + 2)];
            dest.u8[k + 3] = source.u8[sizeof(T) - 1 - (k + 3)];
        }

        // Catch any leftovers
        for (; k < sizeof(T); ++k)
            dest.u8[k] = source.u8[sizeof(T) - 1 - k];


        return dest.t;
    }
};

KAZE_NS_END
