#pragma once

#ifndef kaze_core_math_mathf_h_
#define kaze_core_math_mathf_h_

#include <kaze/core/lib.h>
#include <kaze/core/concepts.h>

#include <gcem.hpp>

KAZE_NAMESPACE_BEGIN
    namespace mathf {

    // ===== Basic Math =======================================================

    /// Return the greatest of two numbers
    template <Arithmetic T>
    constexpr auto max(T a, T b) noexcept -> T
    {
        return a > b ? a : b;
    }

    /// Return the greatest of a list of numbers.
    /// An empty list will result in std::numeric_limits<T>::min()
    template <Arithmetic T>
    constexpr auto max(std::initializer_list<T> list) noexcept -> T
    {
        auto greatest = std::numeric_limits<T>::min();
        for (auto n : list)
        {
            if (n > greatest)
                greatest = n;
        }

        return greatest;
    }

    /// Return the smallest of two numbers
    template <Arithmetic T>
    constexpr auto min(T a, T b) noexcept -> T
    {
        return a < b ? a : b;
    }

    /// Return the smallest number from a list.
    /// An empty list will result in std::numeric_limits<T>::max()
    template <Arithmetic T>
    constexpr auto min(std::initializer_list<T> list) noexcept -> T
    {
        T least = std::numeric_limits<T>::max();
        for (auto n : list)
        {
            if (n < least)
                least = n;
        }

        return least;
    }

    /// Compares two values returning a hard 0 if both values approximately are equal, a negative value if `a < b`,
    /// and a positive value if `a > b` beyond the approximated threshhold. You can set the epsilon and scale values.
    /// (I'm not sure how to describe these, other than they are used to determine the approximated threshhold.
    /// Equation referenced from doctest::Approx.)
    /// \param[in] a        first value to check
    /// \param[in] b        second value to check
    /// \param[in] epsilon  smallest differing value * 100
    /// \param[in] scale    scales epsilon
    template <FloatingPoint T>
    constexpr auto approxCompare(T a, T b, T epsilon = std::numeric_limits<T>::epsilon() * 100, T scale = 1.0) noexcept -> T
    {
        return gcem::fabs(a - b) < epsilon * (scale + mathf::max(gcem::fabs(a), gcem::fabs(b))) ? 0
            : a - b;
    }

    /// Subtract two numbers `a - b` in a way that ensures the calculation uses appropriate signed values
    /// Floats and signed integer types will remain untouched, but unsigned integers will be cast to Int64.
    /// Using 64-bit prevents undefined behavior / overflows from narrowing casts.
    template <Arithmetic T>
    constexpr auto sub(const T a, const T b) noexcept
    {
        if constexpr (std::is_signed_v<T>)
        {
            // Signed types can represent negative numbers, so there's no need for conversions
            return a - b;
        }
        else
        {
            // We can assume `T` is an unsigned integral type because floating point types are signed.
            // Use 64-bit integer to ensure conversion safety
            return static_cast<Int64>(a) - static_cast<Int64>(b);
        }
    }

    /// Simple swap for arithmetic types (simple std::swap replacement to keep `std` out of these functions)
    template <Arithmetic T>
    constexpr auto swap(T &a, T &b) noexcept -> void
    {
        const T temp = a;
        a = b;
        b = temp;
    }

    template<Arithmetic T>
    constexpr auto abs(const T x) noexcept -> T
    {
        return (x < 0) ? -x : x;
    }

    template <FloatingPoint T>
    constexpr auto round(T x) noexcept -> T
    {
        return gcem::round(x);
    }

    /// Get 1 if `x` is positive, or -1 if negative. 0 is returned if x is 0.
    template<Arithmetic T>
    constexpr T sign(const T x) noexcept
    {
        return (x > 0) ? T(1)
            : (x < 0) ? T(-1)
            : 0;
    }

    /// Get the square root of a number
    /// \param[in] x the number to perform square root operation on
    template <Arithmetic T>
    constexpr auto sqrt(const T x) noexcept
    {
        return gcem::sqrt(x);
    }

    /// Get `x` to the p of `p`

    /// \param[in] x the number to perform the operation on
    /// \param[in] p the power to raise `x` to
    /// \returns x to the power of p as an double if an integral, otherwise, type T.
    template <Arithmetic T, Arithmetic U>
    constexpr auto pow(const T x, const U p) noexcept
    {
        return gcem::pow(x, p);
    }

    /// A true modulus function that wraps without reflective behavior over 0.
    /// Positive values for `x` and `m` will behave exactly like `std::fmod` or `%`. A negative value for `x` and a
    /// positive value for `m`, will behave as if in positive territory, continuing the pattern below 0.
    /// Negative value for `m` results in inverted behavior, where positive `x` values do not reflect over 0. but
    /// continue the same wrapping pattern.
    ///
    /// If `m` is zero, the answer is undefined, but it looks like most compilers will result in 0
    template <Arithmetic T>
    constexpr auto mod(const T x, const T m) noexcept -> T
    {
        if constexpr (std::is_floating_point_v<T>)
            return gcem::fmod(gcem::fmod(x, m) + m, m);
        else // integral otherwise
            return (x % m + m) % m;
    }

    /// Floating point modulo remainder operation, wraps reflectively across 0.
    template <Arithmetic T>
    constexpr auto fmod(const T x, const T m) noexcept
    {
        return gcem::fmod(x, m);
    }

    /// Clamp a value between a min and max value inclusively.
    /// Use {@link clampBounds} if `min` may be greater than `max`.
    /// \param[in] x the value to clamp
    /// \param[in] min lower bounds (inclusive), must be <= `max`
    /// \param[in] max upper bounds (inclusive), must be >= `min`
    template <Arithmetic T>
    constexpr auto clamp(const T x, const T min, const T max) noexcept -> T
    {
        return (x < min) ? min
            : (x > max) ? max
            : x;
    }

    /// Clamp a value between two bounds. Either can be greater or lesser than the other.
    /// Use {@link clamp} if boundaries are guaranteed to be: `bounds0 <= bounds1`
    template <Arithmetic T>
    constexpr auto clampBounds(const T x, T bounds0, T bounds1) noexcept -> T
    {
        if (bounds1 < bounds0)
            mathf::swap(bounds0, bounds1);
        return clamp(x, bounds0, bounds1);
    }


    /// Wrap a value, meaning that when `x` crosses the `min` or `max` boundaries, it wraps around to the other side
    /// like modulus operator, except that it does not reflect over 0.
    template <Arithmetic T>
    constexpr auto wrap(const T x, const T min, const T max) noexcept -> T
    {
        const auto range = mathf::sub(max, min);
        return (range == 0) ? min
            : mathf::mod<T>( mathf::sub(x, min), range) + min;
    }

    template <Arithmetic T>
    constexpr auto wrapBounds(const T x, T bounds0, T bounds1) noexcept -> T
    {
        if (bounds1 < bounds0)
            mathf::swap(bounds0, bounds1);
        return wrap(x, bounds0, bounds1);
    }

    /// Get the distance between two points: (x1, y1) and (x2, y2). This is the floating point template version.
    /// @tparam T the floating point type of each parameter and the return value; if set explicitly, all args will
    ///           be cast to `T`; useful to set explicitly when various point value types are be provided.
    ///
    /// \param[in] x1   x value of the first point
    /// \param[in] y1   y value of the first point
    /// \param[in] x2   x value of the second point
    /// \param[in] y2   y value of the second point
    ///
    /// \returns the distance between points one and two
    template <Arithmetic T>
    constexpr auto distance(const T x1, const T y1, const T x2, const T y2) noexcept
    {
        T xDiff, yDiff;
        if constexpr (std::is_unsigned_v<T>)
        {
            xDiff = mathf::abs(mathf::sub(x1, x2)); // abs ensures its final value is signed, while sub returns a signed type
            yDiff = mathf::abs(mathf::sub(y1, y2));
        }
        else
        {
            xDiff = x1 - x2;
            yDiff = y1 - y2;
        }

        return mathf::sqrt(xDiff * xDiff + yDiff * yDiff);
    }

    // ===== Trigonometric Functions ==========================================

    constexpr auto TwoPi = GCEM_PI * 2.0L;
    constexpr auto Pi = GCEM_PI;
    constexpr auto HalfPi = GCEM_HALF_PI;
    constexpr auto FourthPi = GCEM_HALF_PI * .5;

    /// Convert degrees to radians. Integral types will be returned as a `Double`.
    template <Arithmetic T>
    constexpr auto toRadians(const T degrees) noexcept
    {
        if constexpr (std::is_integral_v<T>)
            return static_cast<Double>(degrees) * static_cast<Double>(Pi / 180.0);
        else
            return degrees * static_cast<T>(Pi / 180.0L);
    }

    /// Convert radians to degrees. Integral types will be returned as a `Double`.
    template <Arithmetic T>
    constexpr auto toDegrees(const T radians) noexcept
    {
        if constexpr (std::is_integral_v<T>)
            return static_cast<Double>(radians) * static_cast<Double>(180.0 / Pi);
        else
            return radians * static_cast<T>(180.L / Pi);
    }

    template <Arithmetic T>
    constexpr auto sin(const T x) noexcept {
        return gcem::sin(x);
    }

    template <Arithmetic T>
    constexpr auto cos(const T x) noexcept {
        return gcem::cos(x);
    }

    template <Arithmetic T>
    constexpr auto tan(const T x) noexcept {
        return gcem::tan(x);
    }

    template <Arithmetic T>
    constexpr auto asin(const T x) noexcept {
        return gcem::asin(x);
    }

    template <Arithmetic T>
    constexpr auto acos(const T x) noexcept {
        return gcem::acos(x);
    }

    template <Arithmetic T>
    constexpr auto atan(const T x) noexcept {
        return gcem::atan(x);
    }

    /// Calculate the angle between {0, 0} and the provided point.
    /// Answer will always be positive, from 0 to 2PI radians where 0 points rightward
    /// and moves clockwise.
    /// \param[in] x   x-coordinate of point to check
    /// \param[in] y   y-coordinate of point to check
    template <Arithmetic T>
    constexpr auto coordsToAngle(const T x, const T y) noexcept {
        return gcem::fmod(gcem::atan2(y, x) + TwoPi, TwoPi);
    }

    /// Calculate the angle between two provided points.
    /// Answer will always be positive, from 0 to 2PI radians where 0 points rightward
    /// and moves clockwise.
    /// \param[in] x1   x-coord of point 1
    /// \param[in] y1   y-coord of point 1
    /// \param[in] x2   x-coord of point 2
    /// \param[in] y2   y-coord of point 2
    /// \returns resultant angle from the line drawn between the two points, in radians
    template <Arithmetic T>
    constexpr auto coordsToAngle(const T x1, const T y1, const T x2, const T y2) noexcept {
        return coordsToAngle( sub(x2, x1), sub(y2, y1) );
    }

    /// Calculate a position
    template <Arithmetic T>
    constexpr auto angleToCoords(const T angle, const T distance, T *x, T *y) noexcept -> void
    {
        if (x)
            *x = mathf::cos(angle) * distance;
        if (y)
            *y = mathf::sin(angle) * distance;
    }

    /// Rotate 2d coordinates about the origin {0, 0}
    template <Arithmetic T, Arithmetic TOut>
    constexpr auto rotateCoords(const T x, const T y, const T angle, TOut *resultX, TOut *resultY) noexcept -> void
    {
        const auto sinAngle = mathf::sin(angle);
        const auto cosAngle = mathf::cos(angle);
        if (resultX)
            *resultX = x * cosAngle - static_cast<decltype(sinAngle)>(y) * sinAngle;
        if (resultY)
            *resultY = x * sinAngle + static_cast<decltype(cosAngle)>(y) * cosAngle;
    }

    template <Arithmetic T, Arithmetic TOut>
    constexpr auto rotateCoords(const T x, const T y, const T originX, const T originY, const T angle, TOut *resultX, TOut *resultY) noexcept -> void
    {
        // Alter x and y where its origin is {0, 0}
        auto xOriginZero = mathf::sub(x, originX);
        auto yOriginZero = mathf::sub(y, originY);

        T tempX, tempY;
        mathf::rotateCoords(xOriginZero, yOriginZero, angle, &tempX, &tempY); // non-origin overload

        // bring it back to the origin
        if (resultX)
            *resultX = tempX + originX;
        if (resultY)
            *resultY = tempY + originY;
    }

    template <Arithmetic T, Arithmetic TOut>
    constexpr auto rotateCoordsDegrees(const T x, const T y, const T degrees, TOut *resultX, TOut *resultY) noexcept -> void
    {
        mathf::rotateCoords(x, y, mathf::toRadians(degrees), resultX, resultY);
    }

    template <Arithmetic T, Arithmetic TOut>
    constexpr auto rotateCoordsDegrees(const T x, const T y, const T originX, const T originY, const T degrees, TOut *resultX, TOut *resultY) noexcept -> void
    {
        mathf::rotateCoords(x, y, originX, originY, mathf::toRadians(degrees), resultX, resultY);
    }
} // namespace mathf

KAZE_NAMESPACE_END

#endif
