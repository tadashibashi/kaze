/// \file VecBase.h
/// VecBase is a zero-cost CRTP (curiously recurring template pattern) base class for vectors,
/// containing common shared utilities.
#pragma once

#include <kaze/internal/core/lib.h>
#include <kaze/internal/core/math/mathf.h>
#include <kaze/internal/core/concepts.h>

#include <cmath>
#include <type_traits>

KAZE_NS_BEGIN

/// Default Vector class if there are no specializations available (e.g. the non-specialized Vec<Float, 5>).
/// Since it is not derived from the VecBase class, it's essentially just a fixed-length C-array.
template <Arithmetic T, Size S>
struct Vec
{
    constexpr Vec() noexcept : data() {}

    [[nodiscard]]
    constexpr auto operator[](Size i) -> T &
    {
        KAZE_ASSERT(i < S, "index must be in range");
        return data[i];
    }

    [[nodiscard]]
    constexpr auto operator[](Size i) const -> const T &
    {
        KAZE_ASSERT(i < S, "index must be in range");
        return data[i];
    }

    [[nodiscard]]
    constexpr auto at(Size i) -> T &
    {
        if (i >= S)
            throw std::out_of_range(fmt_lib::format("index must be less than {}, but got {}", S, i));
        return data[i];
    }

    [[nodiscard]]
    constexpr auto at(Size i) const -> const T &
    {
        if (i >= S)
            throw std::out_of_range(fmt_lib::format("index must be less than {}, but got {}", S, i));
        return data[i];
    }

private:
    T data[S];
};

/// CRTP base class for vectors. Zero-cost inheritance.
template <Arithmetic T, Size S>
struct VecBase
{
private:
    /// \returns this object cast to the base class
    [[nodiscard]]
    constexpr auto derived() noexcept -> Vec<T, S> &
    {
        return static_cast<Vec<T, S> &>(*this);
    }

    /// \returns this object cast to the base class
    [[nodiscard]]
    constexpr auto derived() const noexcept -> const Vec<T, S> &
    {
        return static_cast<const Vec<T, S> &>(*this);
    }

    template <Arithmetic T2>
    struct make_diff
    {
        using type = std::conditional_t<std::is_signed_v<T2>, T2, Int64>;
    };

public:
    using floating_t = std::conditional_t<std::is_floating_point_v<T>, T, Double>;
    using diff_t = typename make_diff<T>::type;

    /// Sentinel type for not-a-number
    static constexpr T NaN = std::numeric_limits<T>::has_quiet_NaN ? std::numeric_limits<T>::quiet_NaN()
        : std::numeric_limits<T>::has_infinity ? std::numeric_limits<T>::infinity()
        : std::numeric_limits<T>::max();

    /// Get a component value
    /// \param[in]  i   index of the component to get
    /// \returns value of component at `i`
    [[nodiscard]]
    constexpr auto operator[](const Size i) ->  T &
    {
        KAZE_ASSERT(i < S, "index must be in range");
        return derived()[i];
    }

    /// Get a component value
    /// \param[in]  i   index of the component to get
    /// \returns value of component at `i`
    [[nodiscard]]
    constexpr auto operator[](const Size i) const -> const T &
    {
        KAZE_ASSERT(i < S, "index must be in range");
        return derived()[i];
    }

    /// Get component, with runtime check that throws if out of range
    /// \param[in]  i    index of component, must be less than size
    /// \returns value of component
    [[nodiscard]]
    constexpr auto at(const Size i) -> T &
    {
        if (i >= S)
            throw std::out_of_range(fmt_lib::format("index must be less than {}, but got {}", S, i));
        return derived()[i];
    }

    /// Get component, with runtime check that throws if out of range
    /// \param[in]  i    index of component, must be less than size
    /// \returns value of component
    [[nodiscard]]
    constexpr auto at(const Size i) const -> const T &
    {
        if (i >= S)
            throw std::out_of_range(fmt_lib::format("index must be less than {}, but got {}", S, i));
        return derived()[i];
    }

    /// \returns the number of components in this vector
    [[nodiscard]]
    constexpr auto size() const noexcept -> Size { return S; }

    // ===== Getters =====

    /// \returns whether this vector has any component set to NaN
    [[nodiscard]]
    constexpr auto isNaN() const noexcept -> bool
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

    /// \returns the collective magnitude or length of each component
    [[nodiscard]]
    constexpr auto magnitude() const noexcept
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
    [[nodiscard]]
    constexpr auto distanceTo(Vec<U, S> other) const noexcept
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

    /// \returns 2D angle in radians, using the first two components of the Vec
    template <Size S2 = S>
    [[nodiscard]]
    constexpr auto angle() const noexcept -> std::enable_if_t<S2 >= 2, floating_t>
    {
        return mathf::coordsToAngle<T>(operator[](0), operator[](1));
    }

    /// \returns angle in radians to another Vec, using the first two components of the Vec
    /// \param[in]  to  vector to measure angle to
    template <Size S2 = S>
    [[nodiscard]]
    constexpr auto angleTo(const Vec<T, S> to) const noexcept -> std::enable_if_t<S2 >= 2, floating_t>
    {
        return mathf::coordsToAngle<T>(operator[](0), operator[](1), to[0], to[1]);
    }

    /// \returns the difference of degrees from this Vec to another
    /// \param[in]  to   the other vector to measure the angle to
    [[nodiscard]]
    constexpr auto degreesTo(const Vec<T, S> to) const noexcept
    {
        return mathf::toDegrees(this->angleTo(to));
    }

    /// \returns the angle (in degrees) between {0, 0} and this vec's x and y coordinates
    [[nodiscard]]
    constexpr auto degrees() const noexcept
    {
        return mathf::toDegrees(this->angle());
    }

    /// Rotate the `Vec` on the x and y coordinates
    /// \param[in]  radians  the number of radians to rotate
    /// \returns this object
    template <Arithmetic U = T>
    constexpr auto rotate(const T radians) noexcept -> std::enable_if_t<S >= 2 && std::is_floating_point_v<U>, Vec<T, S> &>
    {
        mathf::rotateCoords(operator[](0), operator[](1), radians, &operator[](0), &operator[](1));
        return derived();
    }

    /// Rotate the `Vec` on the x and y coordinates
    /// \param[in]  degrees  the number of degrees to rotate
    /// \returns this object
    template <Arithmetic U = T>
    constexpr auto rotateDegrees(const T degrees) noexcept -> std::enable_if_t<S >= 2 && std::is_floating_point_v<U>, Vec<T, S> &>
    {
        return this->rotate(mathf::toRadians(degrees));
    }

    /// Make a rotated copy of this object on the x and y coordinates
    /// \param[in]  radians  the number of radians to rotate
    /// \returns a rotated copy
    template <Arithmetic U = T>
    [[nodiscard]] constexpr auto rotated(const T radians) const noexcept -> std::enable_if_t<S >= 2 && std::is_floating_point_v<U>, Vec<T, S>>
    {
        auto v = derived();
        mathf::rotateCoords(operator[](0), operator[](1), radians, &v[0], &v[1]);

        return v;
    }

    /// Make a rotated copy of this object on the x and y coordinates
    /// \param[in]  degrees  the number of degrees to rotate
    /// \returns a rotated copy
    template <Arithmetic U = T>
    [[nodiscard]] constexpr auto rotatedDegrees(const T degrees) const noexcept -> std::enable_if_t<S >= 2 && std::is_floating_point_v<U>, Vec<T, S>>
    {
        return this->rotated(mathf::toRadians(degrees));
    }

    /// Make a copy containing minimum values of each component between two Vectors
    /// \param[in]  other  other Vec to use
    /// \returns a minimized copy
    constexpr auto min(const Vec<T, S> other) const noexcept -> Vec<T, S>
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

    /// Make a copy containing maximum values of each component between two Vectors
    /// \param[in]  other  other VecBase to use
    /// \returns a maximized copy
    constexpr auto max(const Vec<T, S> other) const noexcept -> Vec<T, S>
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

    /// Clamp this VecBase between two bounds, mutating the internals
    /// \param[in]  bounds0  first bounds to clamp by
    /// \param[in]  bounds1  second bounds to clamp by
    /// \returns this object, clamped
    constexpr auto clamp(const Vec<T, S> bounds0, const Vec<T, S> bounds1) noexcept -> Vec<T, S> &
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

    /// Move toward a target vector by a specific delta, without moving beyond it
    /// \param[in]  target             goal to reach
    /// \param[in]  maxDistanceDelta   amount to move by without crossing the target boundary
    /// \returns this altered object
    constexpr auto moveToward(const Vec<T, S> target, diff_t maxDistanceDelta) noexcept -> Vec<T, S> &
    {
        Vec<diff_t, S> diff;
        if constexpr (std::is_signed_v<T>)
            diff = target - (Vec<T, S>)*this;
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

    /// Normalizes the vector if its magnitude is greater than 0.
    /// \returns whether operation was performed
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

    /// Normalizes the vector to a length of 1
    /// \returns this object
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

    /// \returns a normalized copy the vector to a length of 1
    template <Arithmetic U = T>
    [[nodiscard]]
    constexpr auto normalized() const noexcept -> std::enable_if_t<std::is_floating_point_v<U>, Vec<T, S>>
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

    [[nodiscard]]
    constexpr auto operator==(const Vec<T, S> other) const noexcept -> bool
    {
        for (Size i = 0; i < S; ++i)
        {
            if (operator[](i) != other[i])
                return false;
        }

        return true;
    }

    [[nodiscard]]
    constexpr auto operator!=(const Vec<T, S> other) const noexcept -> bool
    {
        return !operator==(other);
    }

    // ===== Unary sign operators =====
    [[nodiscard]]
    constexpr auto operator-() const noexcept -> Vec<T, S>
    {
        Vec<T, S> result = derived();
        for (Size i = 0; i < S; ++i)
            result[i] = -result[i];
        return result;
    }

    [[nodiscard]]
    constexpr auto operator+() const noexcept -> Vec<T, S>
    {
        return derived();
    }

    // ===== Casting operator =====

    template <Arithmetic U, Size S2>
    [[nodiscard]]
    constexpr explicit operator Vec<U, S2>() const noexcept
    {
        constexpr auto MaxIndex = (S2 > S) ? S : S2;
        Vec<U, S2> v{};
        for (Size i = 0; i < MaxIndex; ++i)
            v[i] = static_cast<U>(operator[](i));
        return v;
    }

    // ===== Vector math operators =====

    template <Arithmetic U>
    [[nodiscard]]
    constexpr auto operator +(const Vec<U, S> other) const noexcept -> Vec<T, S>
    {
        auto result = derived();
        for (Size i = 0; i < S; ++i)
            result[i] += static_cast<T>(other[i]);

        return result;
    }

    template <Arithmetic U>
    [[nodiscard]]
    constexpr auto operator -(const Vec<U, S> other) const noexcept -> Vec<diff_t, S>
    {
        Vec<diff_t, S> result = derived();
        for (Size i = 0; i < S; ++i)
            result[i] -= static_cast<diff_t>(other[i]);

        return result;
    }

    template <Arithmetic U>
    [[nodiscard]]
    constexpr auto operator *(const Vec<U, S> other) const noexcept -> Vec<T, S>
    {
        auto result = derived();
        for (Size i = 0; i < S; ++i)
            result[i] *= static_cast<T>(other[i]);

        return result;
    }

    template <Arithmetic U>
    [[nodiscard]]
    constexpr auto operator /(const Vec<U, S> other) const noexcept -> Vec<T, S>
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
    constexpr auto operator +=(const Vec<U, S> other) noexcept -> Vec<T, S> &
    {
        for (Size i = 0; i < S; ++i)
            operator[](i) += static_cast<T>(other[i]);

        return derived();
    }

    /// You should be careful when subtract-assigning with unsigned types
    template <Arithmetic U>
    constexpr auto operator -=(const Vec<U, S> other) noexcept -> Vec<T, S> &
    {
        for (Size i = 0; i < S; ++i)
            operator[](i) -= static_cast<diff_t>(other[i]);

        return derived();
    }

    template <Arithmetic U>
    constexpr auto operator *=(const Vec<U, S> other) noexcept -> Vec<T, S> &
    {
        for (Size i = 0; i < S; ++i)
            operator[](i) *= static_cast<T>(other[i]);

        return derived();
    }

    template <Arithmetic U>
    constexpr auto operator /=(const Vec<U, S> other) noexcept -> Vec<T, S> &
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
    [[nodiscard]]
    constexpr auto operator *(const U scalar) const noexcept ->  Vec<T, S>
    {
        auto result = derived();
        for (Size i = 0; i < S; ++i)
            result[i] *= scalar;
        return result;
    }

    template <Arithmetic U>
    [[nodiscard]] constexpr auto operator /(const U scalar) const noexcept -> Vec<T, S>
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
    constexpr auto operator *=(const U scalar) noexcept -> Vec<T, S> &
    {
        for (Size i = 0; i < S; ++i)
            operator[](i) *= scalar;
        return derived();
    }

    template <Arithmetic U>
    constexpr auto operator /=(const U scalar) noexcept -> Vec<T, S> &
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
    constexpr auto getValueFromChar() const noexcept -> T
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
    constexpr auto swizzle() const noexcept -> Vec<T, sizeof...(Cs)>
    {
        return Vec<T, sizeof...(Cs)>{getValueFromChar<Cs>()...};
    }
};

template <Arithmetic T, Arithmetic U, Size S>
[[nodiscard]] constexpr auto operator *(U scalar, Vec<T, S> v) noexcept -> Vec<T, S>
{
    return v * scalar; // associative property of multiplication
}

template <Arithmetic T, Arithmetic U, Size S>
[[nodiscard]] constexpr auto operator /(U scalar, Vec<T, S> v) noexcept -> Vec<T, S>
{
    Vec<T, S> result;
    for (Size i = 0; i < S; ++i)
    {
        result[i] = scalar;
    }
    return result / v;
}

KAZE_NS_END
