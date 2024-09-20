#pragma once
#ifndef nova_concepts_h_
#define nova_concepts_h_

#include <nova/nova.h>
#include <type_traits>

NOVA_NAMESPACE_BEGIN

using std::declval;

template <typename T>
using math_return_t = std::conditional_t<std::is_integral_v<T>, float, T>;

/// Arithmetic primitive type, it can be an integral or floating point type of any precision.
template <typename T>
concept Arithmetic = std::is_arithmetic_v<T>;

template <typename T>
concept FloatingPoint = std::is_floating_point_v<T>;

template <typename T>
concept Integral = std::is_integral_v<T>;

template <typename T>
concept Vec2Like =
    std::is_arithmetic_v<decltype(T::x)> &&
    std::is_arithmetic_v<decltype(T::y)>;

template <typename T>
concept Vec3Like =
    std::is_arithmetic_v<decltype(T::x)> &&
    std::is_arithmetic_v<decltype(T::y)> &&
    std::is_arithmetic_v<decltype(T::z)>;

template <typename T>
concept Vec4Like =
    std::is_arithmetic_v<decltype(T::x)> &&
    std::is_arithmetic_v<decltype(T::y)> &&
    std::is_arithmetic_v<decltype(T::z)> &&
    std::is_arithmetic_v<decltype(T::w)>;

template <typename T>
concept Shape2DLike =
    std::is_arithmetic_v<decltype(T::x)> &&
    std::is_arithmetic_v<decltype(T::y)>;

template <typename T>
concept Concrete = !std::is_abstract_v<T>;




NOVA_NAMESPACE_END

#endif // nova_concepts_h_
