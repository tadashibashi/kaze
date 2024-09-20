#pragma once
#ifndef nova_math_vec_vecbase_h_
#define nova_math_vec_vecbase_h_

#include <nova/nova.h>
#include <nova/math/mathf.h>
#include <nova/concepts.h>
#include <type_traits>

NOVA_NAMESPACE_BEGIN

/// Default Vector class if there are no specializations available (e.g. the non-specialized Vec<Float, 5>).
/// Since it is not derived from the VecBase class, it's essentially just a fixed-length C-array.
template <Arithmetic T, Size S>
struct Vec
{
    constexpr Vec() noexcept : data() {}

    constexpr T &operator[](Size i) { return data[i]; }
    constexpr const T &operator[](Size i) const { return data[i]; }

private:
    T data[S];
};

/// CRTP base class for vectors. Zero-cost inheritance.
template <Arithmetic T, Size S>
struct VecBase
{
private:
    constexpr Vec<T, S> &derived() noexcept { return static_cast<Vec<T, S> &>(*this); }
    constexpr const Vec<T, S> &derived() const noexcept { return static_cast<const Vec<T, S> &>(*this); }

public:
    using floating_t = std::conditional_t<std::is_floating_point_v<T>, T, Double>;

    template <Arithmetic T2>
    struct make_diff
    {
        using type = std::conditional_t<std::is_signed_v<T2>, T2, Int64>;
    };
    using diff_t = typename make_diff<T>::type;

    static constexpr T NaN = std::numeric_limits<T>::has_quiet_NaN ? std::numeric_limits<T>::quiet_NaN()
        : std::numeric_limits<T>::has_infinity ? std::numeric_limits<T>::infinity()
        : std::numeric_limits<T>::max();


    /// May throw if an invalid value
    constexpr T &operator[](Size i)
    {
        return derived()[i];
    }
    constexpr const T &operator[](Size i) const
    {
        return derived()[i];
    }

    // ===== Getters =====

    [[nodiscard]] constexpr bool isNaN() const noexcept
    {
        for (Size i = 0; i < S; ++i)
        {
            if constexpr (std::numeric_limits<T>::has_quiet_NaN)
            {
                if (std::isnan(operator[](i)))
                    return true;
            }
            else
            {
                if (operator[](i) == NaN)
                    return true;
            }
        }

        return false;
    }

    [[nodiscard]] constexpr auto magnitude() const noexcept
    {
        if constexpr (S == 0)
            return 0;
        else if constexpr (S == 1)
            return mathf::abs(operator[](0));
        else
        {
            diff_t total = 0;
            for (Size i = 0; i < S; ++i)
            {
                const auto currentValue = static_cast<diff_t>(operator[](i));
                total += currentValue * currentValue;
            }

            return mathf::sqrt(total);
        }
    }

    template <Arithmetic U>
    [[nodiscard]] constexpr auto distanceTo(const VecBase<U, S> &other) const noexcept
    {
        if constexpr (S == 0)
            return 0;
        else if constexpr (S == 1)
            return mathf::abs(static_cast<diff_t>(other[0]) - static_cast<diff_t>(operator[](0)));
        else
        {
            diff_t total = 0;
            for (Size i = 0; i < S; ++i)
                total += mathf::pow(static_cast<diff_t>(other[i]) - static_cast<diff_t>(operator[](i)), 2);
            return mathf::sqrt(total);
        }
    }

    template <Size S2 = S>
    [[nodiscard]] constexpr auto angle() const noexcept -> std::enable_if_t<S2 >= 2, floating_t>
    {
        return mathf::coordsToAngle<T>(operator[](0), operator[](1));
    }

    template <Size S2 = S>
    [[nodiscard]] constexpr auto angleTo(const VecBase<T, S> &other) const noexcept -> std::enable_if_t<S2 >= 2, floating_t>
    {
        return mathf::coordsToAngle<T>(operator[](0), operator[](1), other[0], other[1]);
    }

    [[nodiscard]] constexpr auto degreesTo(const VecBase<T, S> &to) const noexcept
    {
        return mathf::toDegrees(this->angleTo(to));
    }

    /// Get angle (in degrees) between {0, 0} and this vec's x and y coordinates
    [[nodiscard]] constexpr auto degrees() const noexcept
    {
        return mathf::toDegrees(this->angle());
    }

    template <Arithmetic U = T>
    constexpr auto rotate(const T radians) noexcept -> std::enable_if_t<S >= 2 && std::is_floating_point_v<U>, Vec<T, S> &>
    {
        mathf::rotateCoords(operator[](0), operator[](1), radians, &operator[](0), &operator[](1));
        return derived();
    }

    template <Arithmetic U = T>
    constexpr auto rotateDegrees(const T degrees) noexcept -> std::enable_if_t<S >= 2 && std::is_floating_point_v<U>, Vec<T, S> &>
    {
        return this->rotate(mathf::toRadians(degrees));
    }

    template <Arithmetic U = T>
    [[nodiscard]] constexpr auto rotated(const T radians) const noexcept -> std::enable_if_t<S >= 2 && std::is_floating_point_v<U>, Vec<T, S>>
    {
        auto v = derived();
        mathf::rotateCoords(operator[](0), operator[](1), radians, &v[0], &v[1]);

        return v;
    }

    template <Arithmetic U = T>
    [[nodiscard]] constexpr auto rotatedDegrees(const T degrees) const noexcept -> std::enable_if_t<S >= 2 && std::is_floating_point_v<U>, Vec<T, S>>
    {
        return this->rotated(mathf::toRadians(degrees));
    }

    constexpr Vec<T, S> min(const VecBase<T, S> &other) const noexcept
    {
        Vec<T, S> result;
        for (Size i = 0; i < S; ++i)
        {
            const auto a = operator[](i);
            const auto b = other[i];
            result[i] = a < b ? a : b;
        }

        return result;
    }

    constexpr Vec<T, S> max(const VecBase<T, S> &other) const noexcept
    {
        Vec<T, S> result;
        for (Size i = 0; i < S; ++i)
        {
            constexpr auto a = operator[](i);
            constexpr auto b = other[i];
            result[i] = a > b ? a : b;
        }

        return result;
    }

    constexpr Vec<T, S> &clamp(const VecBase<T, S> &bounds0, const VecBase<T, S> &bounds1) noexcept
    {
        constexpr auto minBounds = bounds0->min(bounds1);
        constexpr auto maxBounds = bounds0->max(bounds1);

        Vec<T, S> result;
        for (Size i = 0; i < S; ++i)
        {
            constexpr auto curValue = operator[](i);
            constexpr auto curMin = minBounds[i];
            constexpr auto curMax = maxBounds[i];

            operator[](i) = (curValue > curMax) ? curMax :
                (curValue < curMin) ? curMin :
                curValue;
        }

        return *this;
    }

    // ===== Transformation functions =====

    constexpr Vec<T, S> &moveToward(const VecBase<T, S> &target, diff_t maxDistanceDelta) noexcept
    {
        Vec<diff_t, S> diff;
        if constexpr (std::is_signed_v<T>)
            diff = target - *this;
        else
            diff = static_cast<Vec<diff_t, S>>(target) - static_cast<Vec<diff_t, S>>(*this);

        if (!diff.tryNormalize())
            return derived();
        diff *= maxDistanceDelta;

        // handle clamping
        if (maxDistanceDelta > 0)
        {
            for (Size i = 0; i < S; ++i)
            {
                const auto addValue = diff[i];
                const auto curValue = operator[](i);
                const auto curTarget = target[i];
                if (addValue >= 0)
                {
                    if (addValue + curValue > curTarget)
                        diff[i] = curTarget - curValue;
                }
                else
                {
                    if (addValue + curValue < curTarget)
                        diff[i] = curTarget - curValue;
                }
            }
        }

        return operator+=(diff);
    }

    /// Normalizes the vector if its magnitude is greater than 0. Returns whether operation was performed.
    template <Arithmetic U = T>
    constexpr auto tryNormalize() noexcept -> std::enable_if_t<std::is_floating_point_v<U>, bool>
    {
        if (auto mag = magnitude(); mag != 0)
        {
            for (Size i = 0; i < S; ++i)
                operator[](i) /= mag;
            return true;
        }

        return false;
    }

    template <Arithmetic U = T>
    constexpr auto normalize() noexcept -> std::enable_if_t<std::is_floating_point_v<U>, Vec<T, S> &>
    {
        if (auto mag = magnitude(); mag != 0)
        {
            for (Size i = 0; i < S; ++i)
                operator[](i) /= mag;
        }
        else
        {
            for (Size i = 0; i < S; ++i)
                operator[](i) = NaN;
        }
        return derived();
    }

    template <Arithmetic U = T>
    [[nodiscard]] constexpr auto normalized() const noexcept -> std::enable_if_t<std::is_floating_point_v<U>, Vec<T, S>>
    {
        Vec<T, S> result = derived();
        if (auto mag = magnitude(); mag != 0)
        {
            for (Size i = 0; i < S; ++i)
                result[i] /= mag;
        }
        else
        {
            for (Size i = 0; i < S; ++i)
                result[i] = NaN;
        }
        return result;
    }

    // ===== Equality operators =====

    [[nodiscard]] constexpr bool operator==(const VecBase &other) const noexcept
    {
        for (Size i = 0; i < S; ++i)
        {
            if (operator[](i) != other[i])
                return false;
        }

        return true;
    }

    [[nodiscard]] constexpr bool operator!=(const VecBase &other) const noexcept
    {
        return !operator==(other);
    }

    // ===== Unary sign operators =====
    [[nodiscard]] constexpr Vec<T, S> operator-() const noexcept
    {
        Vec<T, S> result = derived();
        for (Size i = 0; i < S; ++i)
            result[i] = -result[i];
        return result;
    }

    [[nodiscard]] constexpr Vec<T, S> operator+() const noexcept
    {
        return derived();
    }

    // ===== Casting operator =====

    template <Arithmetic U, Size S2>
    [[nodiscard]] constexpr explicit operator Vec<U, S2>() const noexcept
    {
        constexpr auto MaxIndex = (S2 > S) ? S : S2;
        Vec<U, S2> v{};
        for (Size i = 0; i < MaxIndex; ++i)
            v[i] = static_cast<U>(operator[](i));
        return v;
    }

    // ===== Vector math operators =====

    template <Arithmetic U>
    [[nodiscard]] constexpr Vec<T, S> operator +(const VecBase<U, S> &other) const noexcept
    {
        auto result = derived();
        for (Size i = 0; i < S; ++i)
            result[i] += static_cast<T>(other[i]);

        return result;
    }

    template <Arithmetic U>
    [[nodiscard]] constexpr Vec<diff_t, S> operator -(const VecBase<U, S> &other) const noexcept
    {
        Vec<diff_t, S> result = derived();
        for (Size i = 0; i < S; ++i)
            result[i] -= static_cast<diff_t>(other[i]);

        return result;
    }

    template <Arithmetic U>
    [[nodiscard]] constexpr Vec<T, S> operator *(const VecBase<U, S> &other) const noexcept
    {
        auto result = derived();
        for (Size i = 0; i < S; ++i)
            result[i] *= static_cast<T>(other[i]);

        return result;
    }

    template <Arithmetic U>
    [[nodiscard]] constexpr Vec<T, S> operator /(const VecBase<U, S> &other) const noexcept
    {
        auto result = derived();
        for (Size i = 0; i < S; ++i)
        {
            const auto current = result[i];
            const auto value = static_cast<T>(other[i]);
            result[i] = (value == 0) ? NaN : current / value;
        }

        return result;
    }

    template <Arithmetic U>
    constexpr Vec<T, S> &operator +=(const VecBase<U, S> &other) noexcept
    {
        for (Size i = 0; i < S; ++i)
            operator[](i) += static_cast<T>(other[i]);

        return derived();
    }

    /// You should be careful when subtract-assigning with unsigned types
    template <Arithmetic U>
    constexpr Vec<T, S> &operator -=(const VecBase<U, S> &other) noexcept
    {
        for (Size i = 0; i < S; ++i)
            operator[](i) -= static_cast<diff_t>(other[i]);

        return derived();
    }

    template <Arithmetic U>
    constexpr Vec<T, S> &operator *=(const VecBase<U, S> &other) noexcept
    {
        for (Size i = 0; i < S; ++i)
            operator[](i) *= static_cast<T>(other[i]);

        return derived();
    }

    template <Arithmetic U>
    constexpr Vec<T, S> &operator /=(const VecBase<U, S> &other) noexcept
    {
        for (Size i = 0; i < S; ++i)
        {
            const auto current = operator[](i);
            const auto value = static_cast<T>(other[i]);
            operator[](i) = (value == 0) ? NaN : current / value;
        }

        return derived();
    }

    // ===== Scalar Math Operators =====

    template <Arithmetic U>
    [[nodiscard]] constexpr Vec<T, S> operator *(const U scalar) const noexcept
    {
        auto result = derived();
        for (Size i = 0; i < S; ++i)
            result[i] *= scalar;
        return result;
    }

    template <Arithmetic U>
    [[nodiscard]] constexpr Vec<T, S> operator /(const U scalar) const noexcept
    {
        auto result = derived();
        if (scalar == 0)
        {
            for (Size i = 0; i < S; ++i)
                result[i] = NaN;
        }
        else
        {
            for (Size i = 0; i < S; ++i)
                result[i] /= scalar;
        }

        return result;
    }

    template <Arithmetic U>
    constexpr Vec<T, S> &operator *=(const U scalar) noexcept
    {
        for (Size i = 0; i < S; ++i)
            operator[](i) *= scalar;
        return derived();
    }

    template <Arithmetic U>
    constexpr Vec<T, S> &operator /=(const U scalar) noexcept
    {
        if (scalar == 0)
        {
            for (Size i = 0; i < S; ++i)
                operator[](i) = NaN;
        }
        else
        {
            for (Size i = 0; i < S; ++i)
                operator[](i) /= scalar;
        }

        return derived();
    }

    // ===== Swizzle =====

private:
    template <Char C>
    constexpr T getValueFromChar() const noexcept
    {
        if constexpr (S >= 1 && C == 'x')
            return operator[](0);
        else if constexpr (S >= 2 && C == 'y')
            return operator[](1);
        else if constexpr (S >= 3 && C == 'z')
            return operator[](2);
        else if constexpr (S >= 4 && C == 'w')
            return operator[](3);
        else
            return NaN; // not a valid template overload!
    }

    template <Char... Cs>
    struct are_components_valid : std::true_type {};
    template <Char C, Char... Rest>
    struct are_components_valid<C, Rest...>
    {
        static constexpr bool value = Vec<T, S>::template is_valid_component<C>::value &&
            are_components_valid<Rest...>::value;
    };
public:
    template <Char... Cs> requires are_components_valid<Cs...>::value
    constexpr Vec<T, sizeof...(Cs)> swizzle() const noexcept
    {
        return Vec<T, sizeof...(Cs)>{getValueFromChar<Cs>()...};
    }
};

template <Arithmetic T, Arithmetic U, Size S>
[[nodiscard]] constexpr Vec<T, S> operator *(U scalar, Vec<T, S> v) noexcept
{
    return v * scalar; // associative property of multiplication
}

template <Arithmetic T, Arithmetic U, Size S>
[[nodiscard]] constexpr Vec<T, S> operator /(U scalar, Vec<T, S> v) noexcept
{
    Vec<T, S> result;
    for (Size i = 0; i < S; ++i)
    {
        result[i] = scalar;
    }
    return result / v;
}

NOVA_NAMESPACE_END

#endif // nova_math_vec_vecbase_h_
