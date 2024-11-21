/// \file Vec4.h
/// Vec4 math class specialization
#pragma once
#include <kaze/internal/core/lib.h>
#include "VecBase.h"

KAZE_NS_BEGIN

template <Arithmetic T>
struct alignas(16) Vec<T, 4> : VecBase<T, 4>
{
    constexpr Vec() noexcept : ptr{} {  }
    constexpr Vec(const T x, const T y, const T z, const T w = static_cast<T>(1)) noexcept : ptr{x, y, z, w} { }
    constexpr Vec(const Vec<T, 3> v3, const T w) noexcept : ptr{v3[0], v3[1], v3[2], w} { }
    constexpr Vec(const T x, const Vec<T, 3> v3) noexcept : ptr{x, v3[0], v3[1], v3[2]} { }
    constexpr Vec(const Vec<T, 2> a, const Vec<T, 2> b) noexcept : ptr{a[0], a[1], b[0], b[1]} { }
    constexpr Vec(const Vec<T, 2> v, const T z, const T w) noexcept : ptr{v[0], v[1], z, w} { }
    constexpr Vec(const T x, const T y, const Vec<T, 2> v) noexcept : ptr{x, y, v[0], v[1]} { }
    constexpr Vec(const T x, const Vec<T, 2> v, const T w) noexcept : ptr{x, v[0], v[1], w} { }
    constexpr Vec &operator =(const Vec &other) noexcept = default;
    constexpr Vec(const Vec &other) noexcept = default;

    // ===== Member variables =====

    union
    {
        struct
        {
            T x, y, z, w;
        };

        T ptr[4];
    };

    template <Char C>
    struct is_valid_component { static constexpr bool value = C == 'x' || C == 'y' || C == 'z' || C == 'w'; };

    constexpr auto set(const T px, const T py, const T pz, const T pw) noexcept -> Vec &
    {
        this->x = px;
        this->y = py;
        this->z = pz;
        this->w = pw;
        return *this;
    }

    constexpr auto setX(const T value) noexcept -> Vec &
    {
        this->x = value;
        return *this;
    }

    constexpr auto setY(const T value) noexcept -> Vec &
    {
        this->y = value;
        return *this;
    }

    constexpr auto setZ(const T value) noexcept -> Vec &
    {
        this->z = value;
        return *this;
    }

    constexpr auto setW(const T value) noexcept -> Vec &
    {
        this->w = value;
        return *this;
    }

    [[nodiscard]] constexpr auto operator[](const Size index) noexcept -> T &
    {
        return ptr[index];
    }

    [[nodiscard]] constexpr auto operator[](const Size index) const noexcept -> const T &
    {
        return ptr[index];
    }

    // ===== Static convenience constants =====

    static constinit Vec Zero;
    static constinit Vec One;

    // ===== Static factory functions =====

    [[nodiscard]] constexpr static auto fromAngle(T angle, T distance = static_cast<T>(1)) noexcept
    {
        Vec v{};
        mathf::angleToCoords(angle, distance, &v.x, &v.y);

        return v;
    }

    [[nodiscard]] constexpr static auto fromDegrees(T degrees, T distance = static_cast<T>(1)) noexcept
    {
        return Vec::fromAngle(mathf::toRadians(degrees), distance);
    }

    [[nodiscard]] constexpr static auto createFilled(T value) noexcept -> Vec
    {
        return {value, value, value, value};
    }
};

template <Arithmetic T>
constinit Vec<T, 4> Vec<T, 4>::Zero = Vec(0, 0, 0, 0);

template <Arithmetic T>
constinit Vec<T, 4> Vec<T, 4>::One = Vec(
    static_cast<T>(1),
    static_cast<T>(1),
    static_cast<T>(1),
    static_cast<T>(1)
);

template <Arithmetic T, Arithmetic U>
constexpr auto operator *(U scalar, Vec<T, 4> v) -> Vec<T, 4>
{
    return v * scalar;
}

template <Arithmetic T, Arithmetic U>
constexpr auto operator /(U scalar, Vec<T, 4> v) -> Vec<T, 4>
{
    return Vec<T, 4>(scalar, scalar, scalar, scalar) / v;
}

using Vec4f = Vec<Float, 4>;
using Vec4d = Vec<Double, 4>;
using Vec4i = Vec<Int, 4>;

template <Arithmetic T>
using Vec4 = Vec<T, 4>;

KAZE_NS_END
