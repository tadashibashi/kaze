#include <doctest/doctest.h>

#include <kaze/math/mathf.h>

USING_KAZE_NAMESPACE;

// Only mathf functions with aspects of own implementation are requried for testing.
// Functions that purely wrap gcem are not necessary to test here.

TEST_SUITE("mathf")
{
    TEST_CASE("mathf::max")
    {
        CHECK(mathf::max(0, 0) == 0);
        CHECK(mathf::max(1, 0) == 1);
        CHECK(mathf::max(.1, .8) == .8);
        CHECK(mathf::max(0.999, .998) == .999);

        CHECK(mathf::max({.001, .002, .003, .004, .005}) == .005);
        CHECK(mathf::max({.1, .002, .003, .004, .005}) == .1);
        CHECK(mathf::max({.1, .002, 10.003, .004, .005}) == 10.003);
        CHECK(mathf::max(std::initializer_list<double>{}) == std::numeric_limits<double>::min());
    }

    TEST_CASE("mathf::min")
    {
        CHECK(mathf::min(0, 0) == 0);
        CHECK(mathf::min(1, 0) == 0);
        CHECK(mathf::min(.1, .8) == .1);
        CHECK(mathf::min(0.999, .998) == .998);

        CHECK(mathf::min({.001, .002, .003, .004, .005}) == .001);
        CHECK(mathf::min({.1, .002, .003, .004, .005}) == .002);
        CHECK(mathf::min({.1, -.002, 10.003, .004, .005}) == -.002);
        CHECK(mathf::min(std::initializer_list<double>{}) == std::numeric_limits<double>::max());
    }

    TEST_CASE("mathf::approxCompare")
    {
        CHECK(mathf::approxCompare(0.0, -1.0) > 0);
        CHECK(mathf::approxCompare(-10.0, 1.0) < 0);
        CHECK(mathf::approxCompare(3.14159, static_cast<double>(mathf::Pi), .001) == 0);
        CHECK(mathf::approxCompare(0.99999, 1.0, .001) == 0);
        CHECK(mathf::approxCompare(0.99, 1.0, .001) < 0); // close, but no banana
    }

    TEST_CASE("mathf::sub")
    {
        static_assert(std::is_signed_v<decltype(mathf::sub(1u, 1u))>, "Unsigned overloads of mathf::sub return a signed type");
        static_assert(std::is_same_v<decltype(mathf::sub(10.f, 10.f)), float>, "Float overload return type is not altered");
        static_assert(std::is_same_v<decltype(mathf::sub(10., 10.)), double>, "Float overload return type is not altered");
        static_assert(std::is_same_v<decltype(mathf::sub(10, 10)), int>, "Signed overload return type is not altered");
        static_assert(std::is_same_v<decltype(mathf::sub(10L, 10L)), long>);
        static_assert(std::is_same_v<decltype(mathf::sub(10LL, 10LL)), long long>);

        SUBCASE("Signed integers")
        {
            CHECK(mathf::sub(10, 5) == 5);
            CHECK(mathf::sub(12, 4) == 8);
            CHECK(mathf::sub(15, 15) == 0);
            CHECK(mathf::sub(8, 0) == 8);
            CHECK(mathf::sub(1, 0) == 1);

            CHECK(mathf::sub(5, 10) == -5);
            CHECK(mathf::sub(4, 12) == -8);
            CHECK(mathf::sub(0, 8) == -8);
            CHECK(mathf::sub(0, 1) == -1);

            CHECK(mathf::sub(-10, -5) == -5);
            CHECK(mathf::sub(-12, -4) == -8);
            CHECK(mathf::sub(-15, -15) == 0);
            CHECK(mathf::sub(-8,  -0) == -8);
            CHECK(mathf::sub(-1,  -0) == -1);

            CHECK(mathf::sub(-5, -10) == 5);
            CHECK(mathf::sub(-4, -12) == 8);
            CHECK(mathf::sub(-0, -8) == 8);
            CHECK(mathf::sub(-0, -1) == 1);
        }

        SUBCASE("Unsigned integers")
        {
            CHECK(mathf::sub(10u, 5u) == 5);
            CHECK(mathf::sub(12u, 4u) == 8);
            CHECK(mathf::sub(15u, 15u) == 0);
            CHECK(mathf::sub(8u, 0u) == 8);
            CHECK(mathf::sub(1u, 0u) == 1);

            CHECK(mathf::sub(5u, 10u) == -5);
            CHECK(mathf::sub(4u, 12u) == -8);
            CHECK(mathf::sub(0u, 8u) == -8);
            CHECK(mathf::sub(0u, 1u) == -1);
        }
    }

    TEST_CASE("mathf::abs")
    {
        // Types are transparently passed for return values
        static_assert(std::is_same_v<decltype(mathf::abs(1)), int>);
        static_assert(std::is_same_v<decltype(mathf::abs(1.f)), float>);
        static_assert(std::is_same_v<decltype(mathf::abs(1.)), double>);
        static_assert(std::is_same_v<decltype(mathf::abs(1u)), unsigned int>);
        static_assert(std::is_same_v<decltype(mathf::abs(1ul)), unsigned long>);

        CHECK(mathf::abs(0) == 0);
        CHECK(mathf::abs(10) == 10);
        CHECK(mathf::abs(-10) == 10);
    }

    TEST_CASE("mathf::sign")
    {
        // Types are transparently passed for return values
        static_assert(std::is_same_v<decltype(mathf::sign(1)), int>);
        static_assert(std::is_same_v<decltype(mathf::sign(1.f)), float>);
        static_assert(std::is_same_v<decltype(mathf::sign(1.)), double>);
        static_assert(std::is_same_v<decltype(mathf::sign(1u)), unsigned int>);
        static_assert(std::is_same_v<decltype(mathf::sign(1ul)), unsigned long>);

        CHECK(mathf::sign(0) == 0);
        CHECK(mathf::sign(1) == 1);
        CHECK(mathf::sign(-1) == -1);
        CHECK(mathf::sign(100.f) == 1.f);
        CHECK(mathf::sign(1.1f) == 1.f);
        CHECK(mathf::sign(-100.0) == -1.0);
        CHECK(mathf::sign(-0.999) == -1.0);
        CHECK(mathf::sign(.00000001) == 1.0);
        CHECK(mathf::sign(-.00000001) == -1.0);
    }

    TEST_CASE("mathf::mod")
    {
        static_assert(std::is_same_v<decltype(mathf::mod(1, 1)), int>);
        static_assert(std::is_same_v<decltype(mathf::mod(1.f, 1.f)), float>);
        static_assert(std::is_same_v<decltype(mathf::mod(1., 1.)), double>);
        static_assert(std::is_same_v<decltype(mathf::mod(1u, 1u)), unsigned int>);
        static_assert(std::is_same_v<decltype(mathf::mod(1ul, 1ul)), unsigned long>);

        CHECK(mathf::mod(10, 4) == 2);
        CHECK(mathf::mod(-3, 4) == 1);  // wraps appropriately in negative territory
        CHECK(mathf::mod(4, -3) == -2); // negative second value exibits same behavior as positive, but inverted

        // Floating point types
        CHECK(mathf::mod(15.f, 7.f) == 1.f);
        CHECK(mathf::mod(11.5f, 5.5f) == .5f);
        CHECK(mathf::mod(-2.f, 4.f) == 2.f);
        CHECK(mathf::mod(-3.f, -4.f) == -3.f);
        CHECK(mathf::mod(5.f, -3.f) == -1.f);

        // According to cppreference, 0 in the second value may result in domain error with implementation-defined behavior
        // We won't test that here, so it should be avoided by the user.
        // e.g. mathf::mod(10, 0)

        CHECK(mathf::mod(0, 10) == 0);
    }

    TEST_CASE("mathf::clamp")
    {
        CHECK(mathf::clamp(4, 0, 10) == 4); // pass through in-range
        CHECK(mathf::clamp(75, 50, 100) == 75);
        CHECK(mathf::clamp(-4, -9, 10) == -4);
        CHECK(mathf::clamp(-1, -4, 0) == -1);
        CHECK(mathf::clamp(-15, -40, -1) == -15);
        CHECK(mathf::clamp(0, 0, 0) == 0);

        CHECK(mathf::clamp(4, 0, 3) == 3);  // clamped on upper bound
        CHECK(mathf::clamp(5, 4, 4) == 4);
        CHECK(mathf::clamp(100, -4, 4) == 4);
        CHECK(mathf::clamp(100, -4, -1) == -1);


        CHECK(mathf::clamp(-4, 0, 3) == 0); // clamped on lower bound
        CHECK(mathf::clamp(-4, -2, 3) == -2);
        CHECK(mathf::clamp(-4, -3, -1) == -3);
        CHECK(mathf::clamp(-4, -3, -3) == -3);
    }

    TEST_CASE("mathf::clampBounds")
    {
        // Behaves exactly like clamp, but where the order of the boundaries don't matter
        // Test ordering of bounds

        // Pass through
        CHECK(mathf::clampBounds(4, 18, -4) == 4);
        CHECK(mathf::clampBounds(4, -4, 18) == 4);
        CHECK(mathf::clampBounds(-50, -25, -100) == -50);
        CHECK(mathf::clampBounds(-50, -100, -25) == -50);
        CHECK(mathf::clampBounds(50, 25, 100) == 50);
        CHECK(mathf::clampBounds(50, 100, 25) == 50);

        // Lower bounds
        CHECK(mathf::clampBounds(-10, 4, 0) == 0);
        CHECK(mathf::clampBounds(-10, 0, 4) == 0);
        CHECK(mathf::clampBounds(-100, -50, -75) == -75);
        CHECK(mathf::clampBounds(-100, -50, -75) == -75);
        CHECK(mathf::clampBounds(25, 75, 50) == 50);
        CHECK(mathf::clampBounds(25, 50, 75) == 50);

        // Upper bounds
        CHECK(mathf::clampBounds(1000, 0, 4) == 4);
        CHECK(mathf::clampBounds(1000, 4, 0) == 4);
        CHECK(mathf::clampBounds(1000, 100, 200) == 200);
        CHECK(mathf::clampBounds(1000, 200, 100) == 200);
        CHECK(mathf::clampBounds(1000, -200, -100) == -100);
        CHECK(mathf::clampBounds(1000, -100, -200) == -100);
    }

    TEST_CASE("mathf::wrap")
    {
        // Pass through
        CHECK(mathf::wrap(0, -1000, 1000) == 0);
        CHECK(mathf::wrap(775, 750, 800) == 775);
        CHECK(mathf::wrap(-1000, -2000, -800) == -1000);
        CHECK(mathf::wrap(-1000, -1000, 1000) == -1000); // touch base
        CHECK(mathf::wrap(500, 500, 500) == 500);

        // Wrap upper bound
        CHECK(mathf::wrap(1500, 0, 1000) == 500);
        CHECK(mathf::wrap(2000, 500, 1500) == 1000);
        CHECK(mathf::wrap(2000, 500, 500) == 500);
        CHECK(mathf::wrap(500, -500, 500) == -500); // value on the upper bound wraps

        // Wrap lower bound
        CHECK(mathf::wrap(-501, -500, 500) == 499);
        CHECK(mathf::wrap(-1000, 0, 500) == 0);
        CHECK(mathf::wrap(150, 200, 400) == 350);
    }

    TEST_CASE("mathf::wrapBounds")
    {
        // Pass through
        CHECK(mathf::wrapBounds(0, -1000, 1000) == 0);
        CHECK(mathf::wrapBounds(0, 1000, -1000) == 0);
        CHECK(mathf::wrapBounds(775, 750, 800) == 775);
        CHECK(mathf::wrapBounds(775, 800, 750) == 775);
        CHECK(mathf::wrapBounds(-1000, -800, -2000) == -1000);
        CHECK(mathf::wrapBounds(-1000, -1000, 1000) == -1000); // touch base
        CHECK(mathf::wrapBounds(-1000, 1000, -1000) == -1000); // touch base
        CHECK(mathf::wrapBounds(500, 500, 500) == 500);

        // Wrap upper bound
        CHECK(mathf::wrapBounds(1500, 0, 1000) == 500);
        CHECK(mathf::wrapBounds(1500, 1000, 0) == 500);
        CHECK(mathf::wrapBounds(2000, 500, 1500) == 1000);
        CHECK(mathf::wrapBounds(2000, 1500, 500) == 1000);
        CHECK(mathf::wrapBounds(2000, 500, 500) == 500);
        CHECK(mathf::wrapBounds(500, -500, 500) == -500); // value on the upper bound wraps
        CHECK(mathf::wrapBounds(500, 500, -500) == -500); // value on the upper bound wraps

        // Wrap lower bound
        CHECK(mathf::wrapBounds(-501, -500, 500) == 499);
        CHECK(mathf::wrapBounds(-501, 500, -500) == 499);
        CHECK(mathf::wrapBounds(-1000, 0, 500) == 0);
        CHECK(mathf::wrapBounds(-1000, 500, 0) == 0);
        CHECK(mathf::wrapBounds(150, 200, 400) == 350);
        CHECK(mathf::wrapBounds(150, 400, 200) == 350);
    }

    TEST_CASE("mathf::distance")
    {
        CHECK(mathf::distance(0, 0, 0, 1) == 1.0);
        CHECK(mathf::distance(0, 0, 1, 1) == doctest::Approx(1.41421356));
        CHECK(mathf::distance(0, 0, -1, 1) == doctest::Approx(1.41421356));
        CHECK(mathf::distance(5, 2, 4, 1) == doctest::Approx(1.41421356));

    }

}
