#pragma once
#ifndef kaze_core_math_circle_h_
#define kaze_core_math_circle_h_

#include <kaze/core/concepts.h>
#include <kaze/core/math/mathf.h>
#include "Vec/Vec2.h"

KAZE_NAMESPACE_BEGIN

template <Arithmetic T>
struct Circle {
    using FloatType = std::conditional_t<std::is_floating_point_v<T>, T, Double>;

    constexpr Circle() noexcept : x(), y(), radius() { } // empty circle
    constexpr Circle(const T x, const T y, const T radius) noexcept : x(x), y(y), radius(radius) { }

    union {
        struct {
            T x, y, radius;
        };

        Vec<T, 2> position;
    };


    constexpr Circle &set(const T x, const T y, const T radius) noexcept
    {
        this->x = x;
        this->y = y;
        this->radius = radius;
        return *this;
    }

    constexpr Circle &setX(const T x) noexcept
    {
        this->x = x;
        return *this;
    }

    /// Set the y position of the circle
    /// \param[in] y  y-coord value to set
    /// @return
    constexpr Circle &setY(const T y) noexcept
    {
        this->y = y;
        return *this;
    }

    /// Set the radius component of the Circle
    /// \returns reference to this Circle for conenient chained calls
    constexpr Circle &setRadius(const T radius) noexcept
    {
        this->radius = radius;
        return *this;
    }

    [[nodiscard]] constexpr T diameter() const noexcept { return radius * 2; }
    constexpr Circle &diameter(T value)
    {
        radius = static_cast<T>(value / 2);
        return *this;
    }

    [[nodiscard]] constexpr FloatType circumference() const noexcept
    {
        return static_cast<FloatType>(2.0) *
            static_cast<FloatType>(mathf::Pi) *
            static_cast<FloatType>(radius);
    }

    [[nodiscard]] constexpr FloatType area() const noexcept
    {
        return static_cast<FloatType>(mathf::Pi) *
            static_cast<FloatType>(radius * radius);
    }
};

using Circd = Circle<Double>;
using Circf = Circle<Float>;
using Circi = Circle<Int>;

KAZE_NAMESPACE_END

#endif // kaze_core_math_circle_h_
