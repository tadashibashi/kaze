#pragma once
#ifndef nova_math_vec_vec3_h_
#define nova_math_vec_vec3_h_

#include <nova/nova.h>
#include <nova/math/mathf.h>
#include <nova/concepts.h>
#include "VecBase.h"

NOVA_NAMESPACE_BEGIN

template <Arithmetic T>
struct Vec<T, 3> : VecBase<T, 3>
{
    constexpr Vec() noexcept : ptr{} { }
    constexpr Vec(T x, T y, T z) noexcept : ptr{x, y, z} { }

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

    Vec &set(const T px, const T py, const T pz) noexcept
    {
        this->x = px;
        this->y = py;
        this->z = pz;
        return *this;
    }

    Vec &setX(const T value) noexcept
    {
        this->x = value;
        return *this;
    }

    Vec &setY(const T value) noexcept
    {
        this->y = value;
        return *this;
    }

    Vec &setZ(const T value) noexcept
    {
        this->z = value;
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
    static constinit Vec Left;
    static constinit Vec Right;
    static constinit Vec Up;
    static constinit Vec Down;
    static constinit Vec One;
    static constinit Vec Forward;
    static constinit Vec Back;

    // ===== Static factory functions =====

    [[nodiscard]] constexpr static Vec fromAngle(const T angle, const T distance = static_cast<T>(1), const T zValue = 0)
    {
        Vec v{};
        v.z = zValue;
        mathf::angleToCoords(angle, distance, &v.x, &v.y);

        return v;
    }

    [[nodiscard]] constexpr static Vec fromDegrees(const T degrees, const T distance = static_cast<T>(1), const T zValue = 0)
    {
        return Vec::fromAngle(mathf::toRadians(degrees), distance, zValue);
    }

    [[nodiscard]] constexpr static Vec createFilled(const T value)
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

NOVA_NAMESPACE_END

#endif // nova_math_vec_vec3_h_
