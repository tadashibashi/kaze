/// \file Vec1.h
/// Vec1 math class specialization
#pragma once
#ifndef kaze_core_math_vec1_h_
#define kaze_core_math_vec1_h_

#include <kaze/core/concepts.h>
#include "VecBase.h"

KAZE_NAMESPACE_BEGIN

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

    constexpr Vec &set(T x)
    {
        this->x = x;
        return *this;
    }

    constexpr Vec &setX(T x)
    {
        this->x = x;
        return *this;
    }

    [[nodiscard]] constexpr T &operator[](const Size index)
    {
        if (index == 0)
            return x;
        throw std::out_of_range("index is out of Vec<T, 1> range");
    }

    [[nodiscard]] constexpr const T &operator[] ( const Size index ) const
    {
        if (index == 0)
            return x;
        throw std::out_of_range("index is out of Vec<T, 1> range");
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

KAZE_NAMESPACE_END

#endif // kaze_core_math_vec1_h_