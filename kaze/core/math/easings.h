#pragma once

#include <kaze/core/lib.h>
#include <kaze/core/concepts.h>
#include <kaze/core/math/mathf.h>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunsequenced"

KAZE_NAMESPACE_BEGIN

namespace easings {
    template <FloatingPoint T>
    constexpr T linear(T x) { return x; }

    template <FloatingPoint T>
    constexpr T inSine(T x) { return T(1.0) - mathf::cos(x * T(mathf::Pi)) * T(.5); }

    template <FloatingPoint T>
    constexpr T outSine(T x) { return mathf::sin((T(x) * T(mathf::Pi)) * T(.5)); }

    template <FloatingPoint T>
    constexpr T inOutSine(T x) { return -(mathf::cos( T(mathf::Pi) * T(x) ) - T(1.0)) * T(.5); }

    template <FloatingPoint T>
    constexpr T inQuad(T x) { return x * x; }

    template <FloatingPoint T>
    constexpr T outQuad(T x) { return T(1.0) - (T(1.0) - x) * (T(1.0) - x); }

    template <FloatingPoint T>
    constexpr T inOutQuad(T x)
    {
        return (x < T(.5)) ?
            T(2.0) * x * x :
            T(1.0) - mathf::pow(T(-2.0) * x + T(2.0), T(2.0)) * T(.5);
    }

    template <FloatingPoint T>
    constexpr T inCubic(T x) { return x * x * x; }

    template <FloatingPoint T>
    constexpr T outCubic(T x) { return T(1.0) - mathf::pow(T(1.0) - x, T(3.0)); }

    template <FloatingPoint T>
    constexpr T inOutCubic(T x)
    {
        return (x < T(.5)) ?
            T(4.0) * x * x * x :
            T(1.0) - mathf::pow(T(-2.0) * x + T(2.0), T(3.0)) * T(.5);
    }

    template <FloatingPoint T>
    constexpr T inQuint(T x) { return x * x * x * x * x; }

    template <FloatingPoint T>
    constexpr T outQuint(T x) { return T(1.0) - mathf::pow(T(1.0) - x, T(5.0)); }

    template <FloatingPoint T>
    constexpr T inOutQuint(T x)
    {
        return (x < T(.5)) ?
            T(16.0) * x * x * x * x * x :
            T(1.0) - mathf::pow(T(-2.0) * x + T(2.0), T(5.0)) * T(.5);
    }

    template <FloatingPoint T>
    constexpr T inExpo(T x)
    {
        return (x == 0) ?
            0 :
            mathf::pow(T(2.0), T(10.0) * x - T(10.0));
    }

    template <FloatingPoint T>
    constexpr T outExpo(T x)
    {
        return (x == T(1.0)) ?
            T(1.0) :
            T(1.0) - mathf::pow(T(2.0), T(-10.0) * x);
    }

    template <FloatingPoint T>
    constexpr T inOutExpo(T x)
    {
        return (x == 0) ? 0 :
            (x == T(1.0)) ? T(1.0) :
            (x < T(.5)) ? mathf::pow(T(2.0), T(20.0) * x - T(10.0)) * T(.5) :
            ( T(2.0) - mathf::pow(T(2.0), T(-20.0) * x + T(10.0)) ) * T(.5);
    }

    template <FloatingPoint T>
    constexpr T inCirc(T x) { return T(1.0) - mathf::sqrt(T(1.0) - mathf::pow(x, T(2.0))); }

    template <FloatingPoint T>
    constexpr T outCirc(T x) { return mathf::sqrt(T(1.0) - mathf::pow(x - T(1.0), T(2.0))); }

    template <FloatingPoint T>
    constexpr T inOutCirc(T x)
    {
        return (x < T(.5)) ?
            (T(1.0) - mathf::sqrt(T(1.0) - mathf::pow(T(2.0) * x, T(2.0)))) * T(.5) :
            (mathf::sqrt(T(1.0) - mathf::pow(T(-2.0) * x + T(2.0), T(2.0))) + T(1.0)) * T(.5);
    }

    template <FloatingPoint T>
    constexpr T inBack(T x)
    {
        constexpr auto c1 = T(1.70158);
        constexpr auto c3 = c1 + T(1.0);

        return c3 * x * x * x - c1 * x * x;
    }

    template <FloatingPoint T>
    constexpr T outBack(T x)
    {
        constexpr auto c1 = T(1.70158);
        constexpr auto c3 = c1 + T(1.0);

        return T(1.0) + c3 * mathf::pow(x - T(1.0), T(3.0)) + c1 * mathf::pow(x - T(1.0), T(2.0));
    }

    template <FloatingPoint T>
    constexpr T inOutBack(T x)
    {
        constexpr auto c1 = T(1.70158);
        constexpr auto c2 = c1 * T(1.525);

        return (x < T(.5)) ?
            (mathf::pow(T(2.0) * x, T(2.0)) * ((c2 + T(1.0)) * T(2.0) * x - c2)) * T(.5) :
            (mathf::pow(T(2.0) * x - T(2.0), T(2.0)) * ((c2 + T(1.0)) * (x * T(2.0) - T(2.0)) + c2) + T(2.0)) * T(.5);
    }

    template <FloatingPoint T>
    constexpr T inElastic(T x)
    {
        constexpr T c4 = (T(2.0) * T(mathf::Pi)) / T(3.0);

        return (x == 0) ? 0 :
            (x == T(1.0)) ? T(1.0) :
            -mathf::pow(T(2.0), T(10.0) * x - T(10.0)) * mathf::sin((x * T(10) - T(10.75)) * c4);
    }

    template <FloatingPoint T>
    constexpr T outElastic(T x)
    {
        constexpr T c4 = (T(2.0) * T(mathf::Pi)) / T(3.0);

        return (x == 0) ? 0 :
            (x == T(1.0)) ? T(1.0) :
            mathf::pow(T(2.0), T(-10.0) * x) * mathf::sin((x * T(10.0) - T(.75)) * c4) + T(1.0);
    }

    template <FloatingPoint T>
    constexpr T inOutElastic(T x)
    {
        constexpr T c5 = (T(2.0) * T(mathf::Pi)) / T(4.5);

        return (x == 0) ? 0 :
            (x == T(1.0)) ? T(1.0) :
            (x < T(.5)) ? -(mathf::pow(T(2.0), T(20.0) * x - T(10.0)) * mathf::sin((T(20.0) * x - T(11.125)) * c5)) * T(.5) :
            (mathf::pow(T(2.0), T(-20.0) * x + T(10.0)) * mathf::sin((T(20.0) * x - T(11.125)) * c5)) * T(.5) + T(1.0);
    }


    template <FloatingPoint T>
    constexpr T outBounce(T x)
    {
        constexpr auto n1 = T(7.5625);
        constexpr auto d1 = T(2.75);

        if (x < T(1.0) / d1)
            return n1 * x * x;

        if (x < T(2.0) / d1)
            return n1 * (x -= T(1.5) / d1) * x + T(.75);

        if (x < T(2.5) / d1)
            return n1 * (x -= T(2.25) / d1) * x + T(0.9375);

        return n1 * (x -= T(2.625) / d1) * x + T(0.984375);
    }

    template <FloatingPoint T>
    constexpr T inBounce(T x)
    {
        return T(1.0) - easings::outBounce(T(1.0) - x);
    }

    template <FloatingPoint T>
    constexpr T inOutBounce(T x)
    {
        return (x < T(.5)) ?
            (T(1.0) - easings::outBounce(T(1.0) - T(2.0) * x)) * T(.5) :
            (T(1.0) + easings::outBounce(T(2.0) * x - T(1.0))) * T(.5);
    }
}

KAZE_NAMESPACE_END

#pragma clang diagnostic pop