#pragma once

#include <kaze/core/lib.h>
#include <kaze/core/concepts.h>
#include "Vec/Vec2.h"

KAZE_NS_BEGIN

template <Arithmetic T>
struct Rect
{
    using floating_t = std::conditional_t<std::is_floating_point_v<T>, T, Double>;
    using signed_t = std::conditional_t<std::is_unsigned_v<T>, Int64, T>;
    constexpr Rect() noexcept : x(), y(), w(), h() { }
    constexpr Rect(T x, T y, T w, T h) noexcept : x(x), y(y), w(w), h(h) { }
    constexpr Rect(const Rect &other) noexcept = default;
    constexpr Rect &operator=(const Rect &other) noexcept = default;

    union
    {
        struct
        {
            T x, y, w, h;
        };

        struct
        {
            Vec2<T> position;
            Vec2<T> size;
        };
    };

    constexpr T area() const noexcept { return mathf::abs(w * h); }

    constexpr floating_t diagonal() const noexcept { return mathf::sqrt(w * w + h * h); }
    constexpr Rect &expand(signed_t leftSide, signed_t topSide, signed_t rightSide, signed_t bottomSide) noexcept
    {
        this->left(signed_t(left()) - leftSide);
        this->top(signed_t(top()) - topSide);
        this->right(signed_t(right()) + rightSide);
        this->bottom(signed_t(bottom()) + bottomSide);
        return *this;
    }
    constexpr Rect &expand(signed_t horizontal, signed_t vertical) noexcept
    {
        this->left(signed_t(left()) - horizontal);
        this->top(signed_t(top()) - vertical);
        this->right(signed_t(right()) + horizontal);
        this->bottom(signed_t(bottom()) + vertical);
        return *this;
    }
    constexpr Rect &expand(signed_t allSides) noexcept
    {
        this->left(signed_t(left()) - allSides);
        this->top(signed_t(top()) - allSides);
        this->right(signed_t(right()) + allSides);
        this->bottom(signed_t(bottom()) + allSides);
        return *this;
    }

    [[nodiscard]] constexpr Rect expanded(signed_t leftSide, signed_t topSide, signed_t rightSide, signed_t bottomSide) const noexcept
    {
        return Rect(*this).expand(leftSide, topSide, rightSide, bottomSide);
    }

    [[nodiscard]] constexpr Rect expanded(signed_t horizontal, signed_t vertical) const noexcept
    {
        return Rect(*this).expand(horizontal, vertical);
    }

    [[nodiscard]] constexpr Rect expanded(signed_t allSides) const noexcept
    {
        return Rect(*this).expand(allSides);
    }

    constexpr T left() const noexcept { return x; }
    constexpr T right() const noexcept { return x + w; }
    constexpr T top() const noexcept { return y; }
    constexpr T bottom() const noexcept { return y + h; }
    constexpr Vec<T, 2> topLeft() const noexcept { return {left(), top()}; }
    constexpr Vec<T, 2> topRight() const noexcept { return {right(), top()}; }
    constexpr Vec<T, 2> bottomLeft() const noexcept { return {left(), bottom()}; }
    constexpr Vec<T, 2> bottomRight() const noexcept { return {right(), bottom()}; }

    constexpr Rect &left(T value) noexcept
    {
        const auto xdiff = mathf::sub(right(), value);
        w = mathf::max<decltype(xdiff)>(xdiff , 0);
        x = value;
        return *this;
    }

    constexpr Rect &right(T value) noexcept
    {
        if (const auto diff = mathf::sub( value, x ); diff > 0)
        {
            w = diff;
        }
        else
        {
            x = value;
            w = 0;
        }
        return *this;
    }

    constexpr Rect &top(T value) noexcept
    {
        const auto ydiff = mathf::sub(bottom(), value);
        h = mathf::max<decltype(ydiff)>(ydiff , 0 );
        y = value;
        return *this;
    }

    constexpr Rect &bottom(T value) noexcept
    {
        if (const auto diff = mathf::sub(value, y); diff > 0)
        {
            h = diff;
        }
        else
        {
            y = value;
            h = 0;
        }

        return *this;
    }

    template <Vec2Like U>
    [[nodiscard]] constexpr bool contains(U point) const noexcept
    {
        if (area() == 0) return false;
        return point.x >= position.x && point.x < position.x + size.x &&
            point.y >= position.y && point.y < position.y + size.y;
    }

    template <Arithmetic U>
    [[nodiscard]] constexpr bool overlaps(const Rect<U> &other) const noexcept
    {
        if (area() == 0 || other.area() == 0) return false;
        return !(other.position.x + other.size.x <= position.x || other.position.x >= position.x + size.x ||
            other.position.y + other.size.y <= position.y || other.position.y >= position.y + size.y);
    }

    template <Arithmetic U>
    [[nodiscard]]
    auto operator==(const Rect<U> &other) const noexcept
    {
        return x == other.x && y == other.y && w == other.w && h == other.h;
    }

    template <Arithmetic U>
    [[nodiscard]]
    auto operator !=(const Rect<U> &other) const noexcept
    {
        return !operator==(other);
    }
};

using Recti = Rect<Int>;
using Rectf = Rect<Float>;
using Rectd = Rect<Double>;

KAZE_NS_END
