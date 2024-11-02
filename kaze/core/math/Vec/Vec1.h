/// \file Vec1.h
/// Vec1 math class specialization
#pragma once
#include <kaze/core/lib.h>
#include <kaze/core/concepts.h>
#include "VecBase.h"

KAZE_NS_BEGIN

template <Arithmetic T>
struct Vec<T, 1> : VecBase<T, 1>
{
    Vec() : x() { }
    explicit Vec(T x) : x(x) { }

    Vec(const Vec &other) = default;
    Vec &operator=(const Vec &other) = default;

    // ===== Member variables =====

    T x;

    template <Char C>
    struct is_valid_component { static constexpr bool value = C == 'x'; };

    constexpr auto set(T x) -> Vec &
    {
        this->x = x;
        return *this;
    }

    constexpr auto setX(T x) -> Vec &
    {
        this->x = x;
        return *this;
    }

    [[nodiscard]]
    constexpr auto operator[](const Size index) -> T &
    {
        return x;
    }

    [[nodiscard]]
    constexpr auto operator[] (const Size index) const -> const T &
    {
        return x;
    }

    static constinit Vec Zero;
    static constinit Vec One;
};

template <Arithmetic T>
constinit Vec<T, 1> Vec<T, 1>::Zero = Vec(0);

template <Arithmetic T>
constinit Vec<T, 1> Vec<T, 1>::One = Vec(1);

using Vec1i = Vec<Int, 1>;
using Vec1f = Vec<Float, 1>;
using Vec1d = Vec<Double, 1>;

template <Arithmetic T>
using Vec1 = Vec<T, 1>;

KAZE_NS_END
