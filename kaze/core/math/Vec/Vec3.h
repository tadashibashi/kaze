/// \file Vec3.h
/// Vec3 math class specialization
#pragma once

#include <kaze/core/lib.h>
#include <kaze/core/math/mathf.h>
#include <kaze/core/concepts.h>
#include "VecBase.h"

KAZE_NS_BEGIN

template <Arithmetic T>
struct Vec<T, 3> : VecBase<T, 3>
{
    constexpr Vec() noexcept : ptr{} { }
    constexpr Vec(const T x, const T y, const T z) noexcept : ptr{x, y, z} { }
    constexpr Vec(Vec<T, 2> v, const T z) noexcept : ptr {v[0], v[1], z} { }
    constexpr Vec(const T x, Vec<T, 2> v) noexcept : ptr {x, v[0], v[1]} { }

    constexpr Vec &operator =(const Vec &other) noexcept = default;
    constexpr Vec(const Vec &other) noexcept = default;

    // ===== Member variables =====

    union
    {
        struct
        {
            T x, y, z;
        };

        T ptr[3];
    };

    template <Char C>
    struct is_valid_component { static constexpr bool value = C == 'x' || C == 'y' || C == 'z'; };

    auto set(const T px, const T py, const T pz) noexcept -> Vec &
    {
        this->x = px;
        this->y = py;
        this->z = pz;
        return *this;
    }

    auto setX(const T value) noexcept -> Vec &
    {
        this->x = value;
        return *this;
    }

    auto setY(const T value) noexcept -> Vec &
    {
        this->y = value;
        return *this;
    }

    auto setZ(const T value) noexcept -> Vec &
    {
        this->z = value;
        return *this;
    }

    [[nodiscard]]
    constexpr auto operator[](const Size index) noexcept -> T &
    {
        return ptr[index];
    }

    [[nodiscard]]
    constexpr auto operator[](const Size index) const noexcept -> const T &
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
    static constinit Vec Forward;
    static constinit Vec Back;

    // ===== Static factory functions =====

    [[nodiscard]]
    constexpr static auto fromAngle(
        const T angle,
        const T distance = static_cast<T>(1),
        const T zValue = 0) -> Vec
    {
        Vec v{};
        v.z = zValue;
        mathf::angleToCoords(angle, distance, &v.x, &v.y);

        return v;
    }

    [[nodiscard]]
    constexpr static auto fromDegrees(
        const T degrees,
        const T distance = static_cast<T>(1),
        const T zValue = 0) -> Vec
    {
        return Vec::fromAngle(mathf::toRadians(degrees), distance, zValue);
    }

    [[nodiscard]]
    constexpr static auto createFilled(const T value) -> Vec
    {
        return { value, value, value };
    }
};

template <Arithmetic T>
constinit Vec<T, 3> Vec<T, 3>::Zero = Vec(0, 0, 0);

template <Arithmetic T>
constinit Vec<T, 3> Vec<T, 3>::Left = Vec(-1, 0, 0);

template <Arithmetic T>
constinit Vec<T, 3> Vec<T, 3>::Right = Vec(1, 0, 0);

template <Arithmetic T>
constinit Vec<T, 3> Vec<T, 3>::Up = Vec(0, 1, 0);

template <Arithmetic T>
constinit Vec<T, 3> Vec<T, 3>::Down = Vec(0, -1, 0);

template <Arithmetic T>
constinit Vec<T, 3> Vec<T, 3>::One = Vec(1, 1, 1);

template <Arithmetic T>
constinit Vec<T, 3> Vec<T, 3>::Forward = Vec(0, 0, 1);

template <Arithmetic T>
constinit Vec<T, 3> Vec<T, 3>::Back = Vec(0, 0, -1);

using Vec3f = Vec<Float, 3>;
using Vec3d = Vec<Double, 3>;
using Vec3i = Vec<Int, 3>;

template <Arithmetic T>
using Vec3 = Vec<T, 3>;

KAZE_NS_END
