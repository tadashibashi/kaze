#pragma once
#ifndef kaze_core_math_cube_h_
#define kaze_core_math_cube_h_

#include <kaze/core/lib.h>
#include <kaze/core/concepts.h>
#include <kaze/core/math/Vec/Vec3.h>

KAZE_NAMESPACE_BEGIN

/// Axis-aligned 3D rectangular prism
template <Arithmetic T>
struct Rect3D
{
    constexpr Rect3D() noexcept : position(), size() { }
    constexpr Rect3D(const Rect3D &other) noexcept : position(other.position), size(other.size) { }

    constexpr Rect3D &operator=(const Rect3D &other) noexcept
    {
        position = other.position;
        size = other.size;
        return *this;
    }

    constexpr Rect3D(Vec<T, 3> position, Vec<T, 3> size) noexcept
        : position(position), size(size) {}

    union
    {
        struct
        {
            T x, y, z; ///< position
            T w, h, d; ///< size
        };
        struct
        {
            Vec<T, 3> position;
            Vec<T, 3> size;

        };
    };

    /// Get the 3-dimensional volume of the Rect3D
    [[nodiscard]] constexpr T volume() { return size.x * size.y * size.z; }

    template <Vec3Like U>
    [[nodiscard]] constexpr bool contains(const U point) const noexcept
    {
        if (size.x == 0 || size.y == 0 || size.z == 0) return false;
        return point.x >= position.x && point.x < position.x + size.x &&
            point.y >= position.y && point.y < position.y + size.y &&
            point.z >= position.z && point.z < position.z + size.z;
    }

    template <Arithmetic U>
    [[nodiscard]] constexpr bool overlaps(const Rect3D<U> &other) const noexcept
    {
        return !(other.x + other.w <= x || other.x >= x + w ||
            other.y + other.h <= y || other.y >= y + h ||
            other.z + other.d <= z || other.z >= z + d);
    }

    template <Arithmetic U>
    [[nodiscard]] constexpr bool operator==(const Rect3D<U> &other) const noexcept
    {
        return position == other.position && size == other.size;
    }

    template <Arithmetic U>
    [[nodiscard]] constexpr bool operator!=(const Rect3D<U> &other) const noexcept
    {
        return !operator==(other);
    }
};

using Rect3Di = Rect3D<Int>;
using Rect3Df = Rect3D<Float>;
using Rect3Dd = Rect3D<Double>;

KAZE_NAMESPACE_END

#endif // kaze_core_math_cube_h_
