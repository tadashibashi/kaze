#include <doctest/doctest.h>
#include <kaze/core/math.h>

USING_KAZE_NS;

TEST_SUITE("Vec2")
{
    TEST_CASE("Default constructor")
    {
        Vec2f v;
        CHECK(v.x == 0);
        CHECK(v.y == 0);
    }

    TEST_CASE("Basic constructor")
    {
        Vec2f v(10, 20);
        CHECK(v.x == 10.f);
        CHECK(v.y == 20.f);
    }

    TEST_CASE("Set functions")
    {
        Vec2f v;
        CHECK( &(v.set(10, 20)) == &v );
        CHECK(v.x == 10);
        CHECK(v.y == 20);

        CHECK( &(v.setX(5.5f)) == &v );
        CHECK(v.x == 5.5f);
        CHECK(v.y == 20.f);

        CHECK( &(v.setY(-15.25f)) == &v );
        CHECK(v.x == 5.5f);
        CHECK(v.y == -15.25f);
    }

    TEST_CASE("Directional static factories")
    {
        const auto zero = Vec2f::Zero;
        CHECK(zero.x == 0);
        CHECK(zero.y == 0);

        const auto left = Vec2f::Left;
        CHECK(left.x == -1);
        CHECK(left.y == 0);

        const auto right = Vec2f::Right;
        CHECK(right.x == 1);
        CHECK(right.y == 0);

        const auto up = Vec2f::Up;
        CHECK(up.x == 0);
        CHECK(up.y == -1);

        const auto down = Vec2f::Down;
        CHECK(down.x == 0);
        CHECK(down.y == 1);
    }

    TEST_CASE("Magnitude")
    {
        // Integer length is a double!
        Vec2i point{1, 1};
        CHECK(point.magnitude() == doctest::Approx(mathf::sqrt(2)));

        Vec2f vec{-1, 1};
        CHECK(vec.magnitude() == doctest::Approx(mathf::sqrt(2)));
    }

    TEST_CASE("Copy assignment and constructor")
    {
        Vec2d a(1, 2);
        Vec2d b(a);

        CHECK(a.x == b.x);
        CHECK(a.y == b.y);

        Vec2d c;

        c = b;

        CHECK(b.x == c.x);
        CHECK(b.y == c.y);
    }

    TEST_CASE("Normalize")
    {
        // Use 4 diagonal directions
        SUBCASE("Normalize on vector with {+, +} values")
        {
            Vec2d a(200, 200);
            auto b = a.normalized();

            CHECK(b.magnitude() == doctest::Approx(1));
        }

        SUBCASE("Normalize on vector with {-, -} values")
        {
            Vec2d a(-2000.50, -2000.50);
            auto b = a.normalized();

            CHECK(b.magnitude() == doctest::Approx(1));
        }

        SUBCASE("Normalize on vector with {+, -} values")
        {
            Vec2d a(.25, -.25);
            auto b = a.normalized();

            CHECK(b.magnitude() == doctest::Approx(1));
        }

        SUBCASE("Normalize on vector with {-, +} values")
        {
            Vec2d a(-1.25, 1.25);
            auto b = a.normalized();

            CHECK(b.magnitude() == doctest::Approx(1));
        }

        // Normalizing on 0
        SUBCASE("Normalization on 0-length vector, results in NaN")
        {
            Vec2d a;
            auto b = a.normalized();

            CHECK(b.isNaN());
        }

        // Use normalize
        SUBCASE("normalize should behave as normalized does")
        {
            Vec2d a(200, 200);
            CHECK(a.magnitude() == mathf::sqrt(200 * 200 + 200 * 200));
            a.normalize();

            CHECK(a.magnitude() == doctest::Approx(1));
        }
    }

    TEST_CASE("Rotate")
    {
        constexpr auto SqrtOneHalf = mathf::sqrt(.5);

        SUBCASE("12:00 to 1:30")
        {
            auto v = Vec2f::Up;
            auto rotated = v.rotated(mathf::FourthPi);

            CHECK(rotated.x == doctest::Approx(SqrtOneHalf));
            CHECK(rotated.y == doctest::Approx(-SqrtOneHalf));
        }

        SUBCASE("3:00 to 4:30")
        {
            auto v = Vec2f::Right;
            auto rotated = v.rotated(mathf::FourthPi);

            CHECK(rotated.x == doctest::Approx(SqrtOneHalf));
            CHECK(rotated.y == doctest::Approx(SqrtOneHalf));
        }

        SUBCASE("6:00 to 7:30")
        {
            auto v = Vec2f::Down;
            auto rotated = v.rotated(mathf::FourthPi);

            CHECK(rotated.x == doctest::Approx(-SqrtOneHalf));
            CHECK(rotated.y == doctest::Approx(SqrtOneHalf));
        }

        SUBCASE("9:00 to 10:30")
        {
            auto v = Vec2f::Left;
            auto rotated = v.rotated(mathf::FourthPi);

            CHECK(rotated.x == doctest::Approx(-SqrtOneHalf));
            CHECK(rotated.y == doctest::Approx(-SqrtOneHalf));
        }

        SUBCASE("rotate behaves as rotated")
        {
            auto v = Vec2f::Up;
            v.rotate(mathf::FourthPi);

            CHECK(v.x == doctest::Approx(SqrtOneHalf));
            CHECK(v.y == doctest::Approx(-SqrtOneHalf));
        }

        SUBCASE("rotateDegrees works as expected")
        {
            auto v = Vec2f::Up;
            v.rotateDegrees(45);

            CHECK(v.x == doctest::Approx(SqrtOneHalf));
            CHECK(v.y == doctest::Approx(-SqrtOneHalf));
        }
    }

    TEST_CASE("Equality operator")
    {
        auto a = Vec2f(10, 20);
        auto b = Vec2f(10, 20);

        CHECK(a == b);

        a.x = 50;

        CHECK(!a.operator==(b));
    }

    TEST_CASE("Inequality operator")
    {
        auto a = Vec2f(10, 20);
        auto b = Vec2f(10, 20);

        CHECK(!a.operator!=(b));

        a.x = 50;

        CHECK(a != b);
    }

    TEST_CASE("angleTo and degreesTo")
    {
        auto a = Vec2f(100, 100);
        auto b = Vec2f(400, 400);

        CHECK(a.angleTo(b) == doctest::Approx(mathf::FourthPi));
        CHECK(a.degreesTo(b) == doctest::Approx(45));
        CHECK(b.angleTo(a) == doctest::Approx(mathf::Pi + mathf::FourthPi));
        CHECK(b.degreesTo(a) == doctest::Approx(225));

        b = Vec2f(-400, -400);

        CHECK(a.angleTo(b) == doctest::Approx(mathf::Pi + mathf::FourthPi));
        CHECK(a.degreesTo(b) == doctest::Approx(225));
        CHECK(b.angleTo(a) == doctest::Approx(mathf::FourthPi));
        CHECK(b.degreesTo(a) == doctest::Approx(45));
    }

    TEST_CASE("angle() and degrees()")
    {
        CHECK(Vec2f(1, 0).angle() == doctest::Approx(0));
        CHECK(Vec2f(1, 0).degrees() == doctest::Approx(0));
        CHECK(Vec2f(100, 100).angle() == doctest::Approx(mathf::FourthPi));
        CHECK(Vec2f(100, 100).degrees() == doctest::Approx(45));
        CHECK(Vec2f(0, 1).angle() == doctest::Approx(mathf::HalfPi));
        CHECK(Vec2f(0, 1).degrees() == doctest::Approx(90));
        CHECK(Vec2f(-1, 1).angle() == doctest::Approx(mathf::HalfPi + mathf::FourthPi));
        CHECK(Vec2f(-1, 1).degrees() == doctest::Approx(135));
        CHECK(Vec2f(-1, 0).angle() == doctest::Approx(mathf::Pi));
        CHECK(Vec2f(-1, 0).degrees() == doctest::Approx(180));
        CHECK(Vec2f(-100, -100).angle() == doctest::Approx(mathf::Pi + mathf::FourthPi));
        CHECK(Vec2f(-100, -100).degrees() == doctest::Approx(225));
        CHECK(Vec2f(0, -1).angle() == doctest::Approx(mathf::Pi + mathf::HalfPi));
        CHECK(Vec2f(0, -1).degrees() == doctest::Approx(270));
        CHECK(Vec2f(1, -1).angle() == doctest::Approx(mathf::Pi + mathf::HalfPi + mathf::FourthPi));
        CHECK(Vec2f(1, -1).degrees() == doctest::Approx(315));
    }

    TEST_CASE("Casting between Vec2 types")
    {
        constexpr Vec2f vf(10.5f, 20.25f);
        constexpr auto vd = static_cast<Vec2d>(vf);
        constexpr auto vi = static_cast<Vec2i>(vd);

        CHECK(vd.x == 10.5);
        CHECK(vd.y == 20.25);
        CHECK(vi.x == 10); // truncated
        CHECK(vi.y == 20);
    }

    TEST_CASE("Math assignment operators between Vec")
    {
        Vec2f vf(1, -1);

        CHECK( &(vf += Vec2f(10, -10)) == &vf );

        CHECK(vf.x == 11);
        CHECK(vf.y == -11);

        CHECK( &(vf -= Vec2f(10, -10)) == &vf );

        CHECK(vf.x == 1);
        CHECK(vf.y == -1);

        CHECK( &(vf *= Vec2f(-1, -1)) == &vf );

        CHECK(vf.x == -1);
        CHECK(vf.y == 1);

        CHECK( &(vf /= Vec2f(.5f, .5f)) == &vf );

        CHECK(vf.x == -2);
        CHECK(vf.y == 2);
    }

    TEST_CASE("Math operators between Vectors")
    {
        CHECK(Vec2f(5, 5) + Vec2f(10, -10) == Vec2f(15, -5));
        CHECK(Vec2f(25, 100) - Vec2f(24, -50) == Vec2f(1, 150));
        CHECK(Vec2i(3, 2) * Vec2i(4, 9) == Vec2i(12, 18));
        CHECK(Vec2d(3.0, 2.4) / Vec2d(2.0, 0.6) == Vec2d(1.5, 4.0));
    }

    TEST_CASE("Casting between vectors of differing size")
    {
        auto v2 = Vec2f(1.23f, 4.56f);
        auto v3 = Vec3f(v2);
        auto v4 = Vec4f(v2);

        CHECK(v3.x == 1.23f);
        CHECK(v3.y == 4.56f);
        CHECK(v3.z == 0);

        CHECK(v4.x == 1.23f);
        CHECK(v4.y == 4.56f);
        CHECK(v4.z == 0);
        CHECK(v4.w == 0);
    }

    TEST_CASE("Swizzle Test")
    {
        auto v = Vec2i(1, 2);

        CHECK(v.swizzle<'x', 'y'>() == Vec2i(1, 2));
        CHECK(v.swizzle<'y', 'y'>() == Vec2i(2, 2));
        CHECK(v.swizzle<'y', 'x'>() == Vec2i(2, 1));
        CHECK(v.swizzle<'x', 'x'>() == Vec2i(1, 1));
        CHECK(v.swizzle<'x', 'x', 'x'>() == Vec3i(1, 1, 1));
        CHECK(v.swizzle<'x', 'y', 'x'>() == Vec3i(1, 2, 1));
        CHECK(v.swizzle<'y', 'y', 'x'>() == Vec3i(2, 2, 1));
        CHECK(v.swizzle<'y', 'y', 'y'>() == Vec3i(2, 2, 2));
        CHECK(v.swizzle<'y', 'y', 'y', 'y'>() == Vec4i(2, 2, 2, 2));
        CHECK(v.swizzle<'y', 'x', 'y', 'y'>() == Vec4i(2, 1, 2, 2));
        CHECK(v.swizzle<'x', 'x', 'y', 'x'>() == Vec4i(1, 1, 2, 1));
        CHECK(v.swizzle<'x', 'x', 'x', 'x'>() == Vec4i(1, 1, 1, 1));
    }

    TEST_CASE("moveToward test")
    {
        SUBCASE("Headed positively")
        {
            auto v = Vec2f(0, 0);
            auto target = Vec2f(100, 100);

            v.moveToward(target, 25.f);

            CHECK(v.x == doctest::Approx(25.f/mathf::sqrt(2.f)));
            CHECK(v.y == doctest::Approx(25.f/mathf::sqrt(2.f)));

            v.moveToward(target, 25.f);

            CHECK(v.x == doctest::Approx(50.f/mathf::sqrt(2.f)));
            CHECK(v.y == doctest::Approx(50.f/mathf::sqrt(2.f)));

            v.moveToward(target, 200.f);

            CHECK(v.x == 100.f);
            CHECK(v.y == 100.f);

            v.moveToward(target, 25.f);

            CHECK(v.x == 100.f);
            CHECK(v.y == 100.f);
        }

        SUBCASE("Headed negatively")
        {
            auto v = Vec2f(100, 100);
            auto target = Vec2f(0, 0);

            v.moveToward(target, 25.f);

            CHECK(v.x == doctest::Approx(100.f - 25.f/mathf::sqrt(2.f)));
            CHECK(v.y == doctest::Approx(100.f - 25.f/mathf::sqrt(2.f)));

            v.moveToward(target, 25.f);

            CHECK(v.x == doctest::Approx(100.f - 50.f/mathf::sqrt(2.f)));
            CHECK(v.y == doctest::Approx(100.f - 50.f/mathf::sqrt(2.f)));

            v.moveToward(target, 200.f);

            CHECK(v == Vec2f(0.f, 0.f));

            v.moveToward(target, 25.f);

            CHECK(v == Vec2f(0.f, 0.f));
        }

        SUBCASE("Negative maxDistanceDelta: headed positively")
        {
            auto v = Vec2f(100, 100);
            auto target = Vec2f(0, 0);

            v.moveToward(target, -25.f);

            CHECK(v.x == doctest::Approx(100.f + 25.f/mathf::sqrt(2.f)));
            CHECK(v.y == doctest::Approx(100.f + 25.f/mathf::sqrt(2.f)));

            v.moveToward(target, -25.f);

            CHECK(v.x == doctest::Approx(100.f + 50.f/mathf::sqrt(2.f)));
            CHECK(v.y == doctest::Approx(100.f + 50.f/mathf::sqrt(2.f)));

            v.moveToward(target, -75.f);

            CHECK(v.x == doctest::Approx(100.f + 125.f/mathf::sqrt(2.f)));
            CHECK(v.y == doctest::Approx(100.f + 125.f/mathf::sqrt(2.f)));

            v.moveToward(target, -25.f);

            CHECK(v.x == doctest::Approx(100.f + 150.f/mathf::sqrt(2.f)));
            CHECK(v.y == doctest::Approx(100.f + 150.f/mathf::sqrt(2.f)));
        }

        SUBCASE("Negative maxDistanceDelta: headed negatively")
        {
            auto v = Vec2f(0, 0);
            auto target = Vec2f(100, 100);

            v.moveToward(target, -25.f);

            CHECK(v.x == doctest::Approx(-25.f/mathf::sqrt(2.f)));
            CHECK(v.y == doctest::Approx(-25.f/mathf::sqrt(2.f)));

            v.moveToward(target, -25.f);

            CHECK(v.x == doctest::Approx(-50.f/mathf::sqrt(2.f)));
            CHECK(v.y == doctest::Approx(-50.f/mathf::sqrt(2.f)));

            v.moveToward(target, -75.f);

            CHECK(v.x == doctest::Approx(-125.f/mathf::sqrt(2.f)));
            CHECK(v.y == doctest::Approx(-125.f/mathf::sqrt(2.f)));

            v.moveToward(target, -25.f);
            CHECK(v.x == doctest::Approx(-150.f/mathf::sqrt(2.f)));
            CHECK(v.y == doctest::Approx(-150.f/mathf::sqrt(2.f)));
        }
    }
}
