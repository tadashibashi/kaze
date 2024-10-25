/// \file Vec2.h
/// Vec2 math class specialization
#pragma once

#include <kaze/core/lib.h>
#include <kaze/core/math/mathf.h>
#include <kaze/core/concepts.h>

#include "VecBase.h"

KAZE_NAMESPACE_BEGIN

template <Arithmetic T>
struct Vec<T, 2> : VecBase<T, 2>
{
    constexpr Vec() noexcept : ptr{} { }
    constexpr Vec(T x, T y) noexcept : ptr{x, y} { }
    constexpr Vec &operator =(const Vec &other) noexcept = default;
    constexpr Vec(const Vec &other) noexcept = default;

    // ===== Member variables =====

    union
    {
        struct
        {
            T x, y;
        };

        T ptr[2];
    };

    template <Char C>
    struct is_valid_component { static constexpr bool value = C == 'x' || C == 'y'; };

    constexpr Vec &set(T x, T y) noexcept
    {
        this->x = x;
        this->y = y;
        return *this;
    }

    constexpr Vec &setX(T value) noexcept
    {
        this->x = value;
        return *this;
    }

    constexpr Vec &setY(T value) noexcept
    {
        this->y = value;
        return *this;
    }

    [[nodiscard]] constexpr T &operator[](const Size index)
    {
        return ptr[index];
    }

    [[nodiscard]] constexpr const T &operator[](const Size index) const
    {
        return ptr[index];
    }

    // ===== Static convenience constants =====

    static constinit Vec Zero;
    static constinit Vec Left;
    static constinit Vec Right;
    static constinit Vec Up;
    static constinit Vec Down;
    static constinit Vec One;


    // ===== Static factory functions =====

    /// Creates a vec from an angle and distance, filling in its x and y fields.
    /// If the vec is of a size larger than 2 dimensions, the rest will be populated with 0's.
    /// For integral types, value precision will be truncated.
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
        return { value, value };
    }
};

// Vec constants defined
template <Arithmetic T>
constinit Vec<T, 2> Vec<T, 2>::Zero =  Vec(0, 0);

template <Arithmetic T>
constinit Vec<T, 2> Vec<T, 2>::Left =  Vec(-1, 0);

template <Arithmetic T>
constinit Vec<T, 2> Vec<T, 2>::Right = Vec(1, 0);

template <Arithmetic T>
constinit Vec<T, 2> Vec<T, 2>::Up =    Vec(0, -1);

template <Arithmetic T>
constinit Vec<T, 2> Vec<T, 2>::Down =  Vec(0, 1);

template <Arithmetic T>
constinit Vec<T, 2> Vec<T, 2>::One =  Vec(1, 1);

using Vec2f = Vec<Float, 2>;
using Vec2i = Vec<Int, 2>;
using Vec2d = Vec<Double, 2>;

template <Arithmetic T>
using Vec2 = Vec<T, 2>;

KAZE_NAMESPACE_END
