#include <doctest/doctest.h>

#include <kaze/core/math/Vec.hpp>

USING_KAZE_NAMESPACE;

TEST_SUITE("Vec3")
{
    TEST_CASE("Constructors")
    {
        Vec3f v;
        CHECK(v.x == 0);
        CHECK(v.y == 0);
        CHECK(v.z == 0);

        Vec3i vi(5, 4, 3);
        CHECK(vi.x == 5);
        CHECK(vi.y == 4);
        CHECK(vi.z == 3);

        Vec3i vc(vi); // copy constructor
        CHECK(vc.x == 5);
        CHECK(vc.y == 4);
        CHECK(vc.z == 3);

        Vec3i va;
        va = vc;      // copy assignment
        CHECK(va.x == 5);
        CHECK(va.y == 4);
        CHECK(va.z == 3);
    }

    TEST_CASE("Direct component getter and setters")
    {
        Vec3i v;
        v.setX(10).setY(20).setZ(30);
        CHECK(v.x == 10);
        CHECK(v.y == 20);
        CHECK(v.z == 30);

        v.set(30, 20, 10);
        CHECK(v.z == 10);
        CHECK(v.y == 20);
        CHECK(v.x == 30);
    }

    TEST_CASE("Getter / Setter operator[]")
    {
        Vec3f v(0, 1, 2);

        CHECK(v[0] == 0);
        CHECK(v[1] == 1);
        CHECK(v[2] == 2);

        v[0] = 4.5f;
        v[1] = 6.7f;
        v[2] = 8.9f;

        CHECK(v[0] == 4.5f);
        CHECK(v[1] == 6.7f);
        CHECK(v[2] == 8.9f);
    }

    TEST_CASE("Static factory functions")
    {
        auto v = Vec3f::createFilled(10);
        CHECK(v.x == 10);
        CHECK(v.y == 10);
        CHECK(v.z == 10);

        auto va = Vec3d::fromAngle(mathf::HalfPi);
        CHECK(va.x == 0);
        CHECK(va.y == 1);
        CHECK(va.z == 0);

        auto vd = Vec3d::fromDegrees(135);
        CHECK(vd.x == -mathf::sqrt(1.0/2.0));
        CHECK(vd.y == mathf::sqrt(1.0/2.0));
        CHECK(vd.z == 0);
    }

    TEST_CASE("Equality operators")
    {
        CHECK(Vec3f(1, 2, 3) == Vec3f(1, 2, 3));
        CHECK(Vec3f(1, 2, 3) != Vec3f(1, 5, 3));
        CHECK(Vec3f() == Vec3f::Zero);
        CHECK(Vec3f(1, 1, 1) == Vec3f::One);
        CHECK(Vec3f(1, 5, 5) != Vec3f::One);
    }

    TEST_CASE("Math operators with other vectors, and isNaN check")
    {
        CHECK( (Vec3f(1, 2, 3) + Vec3f(4, 5, 6)) == Vec3f(5, 7, 9) );
        CHECK( (Vec3f(1, 2, 3) - Vec3f(4, 5, 6)) == Vec3f(-3, -3, -3) );
        CHECK( (Vec3f(1, 2, 3) * Vec3f(4, 5, 6)) == Vec3f(4, 10, 18) );
        CHECK( (Vec3f(1, 2, 3) / Vec3f(4, 5, 6)) == Vec3f(.25f, .4f, .5f) );
        CHECK( (Vec3f(1, 2, 3) / Vec3f(4, 5, 0)).isNaN() ); // zero in divisor
        CHECK( !(Vec3f(1, 2, 3) / Vec3f(4, 5, 6)).isNaN());
    }

    TEST_CASE("Math assignment operators with other vectors, and isNaN check")
    {
        CHECK( (Vec3f(1, 2, 3) += Vec3f(4, 5, 6)) == Vec3f(5, 7, 9) );
        CHECK( (Vec3f(1, 2, 3) -= Vec3f(4, 5, 6)) == Vec3f(-3, -3, -3) );
        CHECK( (Vec3f(1, 2, 3) *= Vec3f(4, 5, 6)) == Vec3f(4, 10, 18) );
        CHECK( (Vec3f(1, 2, 3) /= Vec3f(4, 5, 6)) == Vec3f(.25f, .4f, .5f) );
        CHECK( (Vec3f(1, 2, 3) /= Vec3f(4, 5, 0)).isNaN() ); // zero in divisor
        CHECK( !(Vec3f(1, 2, 3) /= Vec3f(4, 5, 6)).isNaN());
    }

    TEST_CASE("Math operators with scalars")
    {
        CHECK( (Vec3f(10, 20, 30) * 10) == Vec3f(100, 200, 300) );
        CHECK( (Vec3f(10, 20, 30) * 0) == Vec3f::Zero );
        CHECK( (Vec3f(10, 20, 30) / 10.f) == Vec3f(1, 2, 3) );
        CHECK( (Vec3f(10, 20, 30) / 0).isNaN() );

        CHECK( (10 * Vec3f(10, 20, 30)) == Vec3f(100, 200, 300) );
        CHECK( (0 * Vec3f(10, 20, 30)) == Vec3f::Zero );
        CHECK( (600 / Vec3f(10, 20, 30)) == Vec3f(60, 30, 20) );
        CHECK( (100 / Vec3f::Zero ).isNaN() );
    }

    TEST_CASE("Casting operators between vectors")
    {
        CHECK( static_cast<Vec3i>(Vec3f(10.5f, 20.3f, 30.23f)) == Vec3i(10, 20, 30) ); // truncate to int
        auto v = static_cast<Vec3d>(Vec3f(10.5f, 20.3f, 30.23f));
        CHECK(v.x == doctest::Approx(10.5));
        CHECK(v.y == doctest::Approx(20.3));
        CHECK(v.z == doctest::Approx(30.23));

        // Different sized vectors
        CHECK( static_cast<Vec2i>(Vec3f(1.1f, 2.2f, 3.3f)) == Vec2i(1, 2));
        CHECK( static_cast<Vec4i>(Vec3f(1.1f, 2.2f, 3.3f)) == Vec4i(1, 2, 3, 0));
    }

    TEST_CASE("Swizzle test")
    {
        Vec3i v3(1, 2, 3);
        CHECK(v3.swizzle<'x', 'y', 'z', 'x'>() == Vec4i(1, 2, 3, 1));
        CHECK(v3.swizzle<'x', 'x', 'y', 'z'>() == Vec4i(1, 1, 2, 3));
        CHECK(v3.swizzle<'x', 'y', 'y', 'x'>() == Vec4i(1, 2, 2, 1));
        CHECK(v3.swizzle<'z', 'y', 'x', 'y'>() == Vec4i(3, 2, 1, 2));
        CHECK(v3.swizzle<'z', 'z', 'x', 'x'>() == Vec4i(3, 3, 1, 1));
        CHECK(v3.swizzle<'y', 'y', 'y', 'y'>() == Vec4i(2, 2, 2, 2));
        CHECK(v3.swizzle<'z', 'z', 'z', 'z'>() == Vec4i(3, 3, 3, 3));

        CHECK(v3.swizzle<'x', 'y', 'z'>() == Vec3i(1, 2, 3));
        CHECK(v3.swizzle<'x', 'x', 'y'>() == Vec3i(1, 1, 2));
        CHECK(v3.swizzle<'x', 'y', 'y'>() == Vec3i(1, 2, 2));
        CHECK(v3.swizzle<'z', 'y', 'x'>() == Vec3i(3, 2, 1));
        CHECK(v3.swizzle<'z', 'z', 'x'>() == Vec3i(3, 3, 1));
        CHECK(v3.swizzle<'y', 'y', 'y'>() == Vec3i(2, 2, 2));
        CHECK(v3.swizzle<'z', 'z', 'z'>() == Vec3i(3, 3, 3));

        CHECK(v3.swizzle<'x', 'x'>() == Vec2i(1, 1));
        CHECK(v3.swizzle<'y', 'x'>() == Vec2i(2, 1));
        CHECK(v3.swizzle<'z', 'y'>() == Vec2i(3, 2));
        CHECK(v3.swizzle<'z', 'z'>() == Vec2i(3, 3));
        CHECK(v3.swizzle<'x', 'z'>() == Vec2i(1, 3));
        CHECK(v3.swizzle<'y', 'y'>() == Vec2i(2, 2));

        CHECK(v3.swizzle<'z'>() == Vec1i(3));
        CHECK(v3.swizzle<'x'>() == Vec1i(1));
    }
}
