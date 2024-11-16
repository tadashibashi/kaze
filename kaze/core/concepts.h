#pragma once

#include <kaze/core/lib.h>
#include <kaze/core/traits.h>

KAZE_NS_BEGIN

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

template <typename T>
concept Hashable = requires(T a) {
    { std::hash<T>{}(a) } -> std::same_as<std::size_t>; // Must be hashable
    { a == a } -> std::convertible_to<bool>;            // Must be equality comparable
};

template <typename T, typename K>
concept LoadableAsset =
    std::is_default_constructible_v<T> &&
    std::is_same_v<Bool, decltype(std::declval<T>().load(std::declval<K>()))> &&
    std::is_same_v<void, decltype(std::declval<T>().release())>;

template <typename T>
concept ContainerItem =
    std::is_default_constructible_v<T> &&
    std::is_move_constructible_v<T> &&
    std::is_copy_constructible_v<T>;

/// Poolable types are used with Pool and MultiPool.
/// They must be default constructible, moveable, with the following member functions defined:
///     auto init(Args...) -> Bool;
///     auto release() -> void;
/// Poolable objects are created once, and must be released and reinitialized when taken
/// out again by the user. `init` and `release` effectively become the contructor and destructor
/// to this end, which are called when a user takes out and puts back a pool object, respectively.
///
/// You can perform optimizations like only creating buffers once without needing to
/// destroy and recreate them again each time.
template <typename T>
concept Poolable =
    std::is_default_constructible_v<T> &&
    std::is_move_constructible_v<T>;

template <typename T>
concept PlainFunctor =
    requires(T &funcObj) {
        funcObj();
    };
KAZE_NS_END
