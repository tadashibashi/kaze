#pragma once
#ifndef kaze_core_endian_h_
#define kaze_core_endian_h_

#include <kaze/core/lib.h>
#include <bit>

KAZE_NAMESPACE_BEGIN

struct Endian
{
    enum Type
    {
        Unknown = 0,
        Little = static_cast<int>(std::endian::little),
        Big = static_cast<int>(std::endian::big),
    };

    /// Native platform's endian value, may be either Endian::Little or Endian::Big
    static constexpr Type Native = static_cast<Type>(std::endian::native);

    static constexpr bool isBig() { return Native == Big; }
    static constexpr bool isLittle() { return Native == Little; }
    static constexpr auto opposite(Type endian) -> Type {
        switch(endian)
        {
        case Little: return Big;
        case Big:    return Little;
        default:     return Unknown;
        }
    }

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

KAZE_NAMESPACE_END

#endif // kaze_core_endian_h_
