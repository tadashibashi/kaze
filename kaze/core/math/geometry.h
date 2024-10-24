/// Contains functions that pertain to math template classes, this prevents circular dependencies, since these
/// functions usually include the mathf header.
#pragma once
#ifndef kaze_core_math_geometry_h_
#define kaze_core_math_geometry_h_

#include <kaze/core/lib.h>
#include <kaze/core/math/mathf.h>
#include <kaze/core/math/Circle.h>
#include <kaze/core/math/Rect.h>
#include <kaze/core/math/Rect3D.h>
#include <kaze/core/math/Vec/Vec2.h>

KAZE_NAMESPACE_BEGIN

namespace mathf
{
    template <Arithmetic T, Arithmetic U>
    [[nodiscard]] constexpr bool overlaps(const Rect<T> &a, const Rect<U> &b) noexcept
    {
        return a.overlaps(b);
    }

    template <Arithmetic T, Arithmetic U>
    [[nodiscard]] constexpr bool overlaps(const Rect<T> &rect, const Vec<U, 2> &point) noexcept
    {
        return rect.contains(point);
    }

    template <Arithmetic T, Arithmetic U>
    [[nodiscard]] constexpr bool overlaps(const Vec<T, 2> &point, const Rect<U> &rect) noexcept
    {
        return rect.contains(point);
    }

    /// Truncates floating points for pixel accuracy
    template <Arithmetic T, Arithmetic U>
    [[nodiscard]] constexpr bool overlaps(const Vec<T, 2> &a, const Vec<U, 2> &b) noexcept
    {
        return static_cast<Int64>(a.x) == static_cast<Int64>(b.x) &&
               static_cast<Int64>(a.y) == static_cast<Int64>(b.y);
    }

    template <Arithmetic T, Arithmetic U>
    [[nodiscard]] constexpr bool overlaps(const Circle<T> &a, const Circle<U> &b) noexcept
    {
        if (a.radius == 0 || b.radius == 0) return false;
        return mathf::distance(a.x, a.y, b.x, b.y) < a.radius + b.radius;
    }

    template <Arithmetic T, Arithmetic U>
    [[nodiscard]] constexpr bool overlaps(const Circle<T> &a, const Vec<U, 2> &b) noexcept
    {
        if (a.radius == 0) return false;
        return mathf::distance(a.x, a.y, b.x, b.y) < a.radius;
    }

    template <Arithmetic T, Arithmetic U>
    [[nodiscard]] constexpr bool overlaps(const Vec<T, 2> &a, const Circle<U> &b) noexcept
    {
        return mathf::overlaps(b, a);
    }

    template <Arithmetic T, Arithmetic U>
    [[nodiscard]] constexpr bool overlaps(const Rect<T> &rect, const Circle<U> &circle) noexcept
    {
        if (circle.radius == 0 || rect.area() == 0) return false;

        U testX = circle.x;
        U testY = circle.y;

        if (circle.x < rect.left())        testX = rect.left();
        else if (circle.x > rect.right())  testX = rect.right();

        if (circle.y < rect.top())         testY = rect.top();
        else if (circle.y > rect.bottom()) testY = rect.bottom();

        return (mathf::distance(circle.x, circle.y, testX, testY) < circle.radius);
    }

    template <Arithmetic T, Arithmetic U>
    [[nodiscard]] constexpr bool overlaps(const Circle<T> &circle, const Rect<U> &rect) noexcept
    {
        return mathf::overlaps(rect, circle);
    }

    template <Arithmetic T, Arithmetic U>
    [[nodiscard]] constexpr bool overlaps(const Rect3D<T> &a, const Rect3D<U> &b) noexcept
    {
        return a.overlaps(b);
    }
}

KAZE_NAMESPACE_END

#endif // kaze_core_math_geometry_h_
