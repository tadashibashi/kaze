#pragma once
#ifndef nova_endian_h_
#define nova_endian_h_

#include <nova/nova.h>
#include <bit>

NOVA_NAMESPACE_BEGIN

struct Endian
{
    enum Type
    {
        Unknown = 0,
        Little = static_cast<int>(std::endian::little),
        Big = static_cast<int>(std::endian::big),
        Native = static_cast<int>(std::endian::native),
    };

    static constexpr bool isBig() { return Native == Big; }
    static constexpr bool isLittle() { return Native == Little; }

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

NOVA_NAMESPACE_END

#endif // nova_endian_h_
