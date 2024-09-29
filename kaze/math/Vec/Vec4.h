#pragma once
#ifndef kaze_math_vec_vec4_h_
#define kaze_math_vec_vec4_h_

#include <kaze/kaze.h>
#include "VecBase.h"

KAZE_NAMESPACE_BEGIN

template <Arithmetic T>
struct alignas(16) Vec<T, 4> : VecBase<T, 4>
{
    constexpr Vec() noexcept : ptr{} {  }
    constexpr Vec(const T x, const T y, const T z, const T w = static_cast<T>(1)) noexcept : ptr{x, y, z, w} { }
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

    constexpr Vec &set(const T px, const T py, const T pz, const T pw) noexcept
    {
        this->x = px;
        this->y = py;
        this->z = pz;
        this->w = pw;
        return *this;
    }

    constexpr Vec &setX(const T value) noexcept
    {
        this->x = value;
        return *this;
    }

    constexpr Vec &setY(const T value) noexcept
    {
        this->y = value;
        return *this;
    }

    constexpr Vec &setZ(const T value) noexcept
    {
        this->z = value;
        return *this;
    }

    constexpr Vec &setW(const T value) noexcept
    {
        this->w = value;
        return *this;
    }

    [[nodiscard]] constexpr T &operator[](const Size index) noexcept
    {
        return ptr[index];
    }

    [[nodiscard]] constexpr const T &operator[](const Size index) const noexcept
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

    [[nodiscard]] constexpr static Vec createFilled(T value) noexcept
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
constexpr Vec<T, 4> operator *(U scalar, Vec<T, 4> v)
{
    return v * scalar;
}

template <Arithmetic T, Arithmetic U>
constexpr Vec<T, 4> operator /(U scalar, Vec<T, 4> v)
{
    return Vec<T, 4>(scalar, scalar, scalar, scalar) / v;
}

using Vec4f = Vec<Float, 4>;
using Vec4d = Vec<Double, 4>;
using Vec4i = Vec<Int, 4>;

KAZE_NAMESPACE_END

#endif // kaze_math_vec_vec4_h_
