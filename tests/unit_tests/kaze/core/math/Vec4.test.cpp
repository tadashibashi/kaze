#include <doctest/doctest.h>

#include <kaze/core/math/Vec.hpp>

USING_KAZE_NAMESPACE;

TEST_SUITE("Vec4")
{
    TEST_CASE("Constructors")
    {
        Vec4i v;
        CHECK( v.x == 0 );
        CHECK( v.y == 0 );
        CHECK( v.z == 0 );
        CHECK( v.w == 0 );

        Vec4i vi(5, 4, 3, 2);
        CHECK( vi.x == 5 );
        CHECK( vi.y == 4 );
        CHECK( vi.z == 3 );
        CHECK( vi.w == 2 );

        Vec4i vc(vi); // copy constructor
        CHECK( vc.x == 5 );
        CHECK( vc.y == 4 );
        CHECK( vc.z == 3 );
        CHECK( vc.w == 2 );

        Vec4i va;
        va = vc;      // copy assignment
        CHECK( va.x == 5 );
        CHECK( va.y == 4 );
        CHECK( va.z == 3 );
        CHECK( va.w == 2 );
    }

    TEST_CASE("Direct component getter and setters")
    {
        Vec4i v;
        v.setX(10).setY(20).setZ(30).setW(40);
        CHECK( v.x == 10 );
        CHECK( v.y == 20 );
        CHECK( v.z == 30 );
        CHECK( v.w == 40 );

        v.set(30, 20, 10, 0);
        CHECK( v.w == 0 );
        CHECK( v.z == 10 );
        CHECK( v.y == 20 );
        CHECK( v.x == 30 );
    }

    TEST_CASE("Getter / Setter operator[]")
    {
        Vec4f v(0, 1, 2, 3);

        CHECK( v[0] == 0 );
        CHECK( v[1] == 1 );
        CHECK( v[2] == 2 );
        CHECK( v[3] == 3 );

        v[0] = 4.5f;
        v[1] = 6.7f;
        v[2] = 8.9f;
        v[3] = 10.11f;

        CHECK( v[0] == 4.5f );
        CHECK( v[1] == 6.7f );
        CHECK( v[2] == 8.9f );
        CHECK( v[3] == 10.11f );
    }

    TEST_CASE("Static factory functions")
    {
        auto v = Vec4f::createFilled(10);
        CHECK( v.x == 10 );
        CHECK( v.y == 10 );
        CHECK( v.z == 10 );
        CHECK( v.w == 10 );

        auto va = Vec4d::fromAngle(mathf::HalfPi);
        CHECK( va.x == 0 );
        CHECK( va.y == 1 );
        CHECK( va.z == 0 );
        CHECK( va.w == 0 );

        auto vd = Vec4d::fromDegrees(135);
        CHECK( vd.x == -mathf::sqrt(1.0/2.0) );
        CHECK( vd.y == mathf::sqrt(1.0/2.0) );
        CHECK( vd.z == 0 );
        CHECK( vd.w == 0 );
    }

    TEST_CASE("Equality operators")
    {
        CHECK( Vec4f(1, 2, 3) == Vec4f(1, 2, 3) ); // implicitly sets w to 1
        CHECK( Vec4f(1, 2, 3) != Vec4f(1, 5, 3) );
        CHECK( Vec4f(0, 0, 0, 0) == Vec4f::Zero );
        CHECK( Vec4f(1, 1, 1, 1) == Vec4f::One );
        CHECK( Vec4f(1, 5, 5) != Vec4f::One );
    }

    TEST_CASE("Math operators with other vectors, and isNaN check")
    {
        CHECK( (Vec4f(1, 2, 3) + Vec4f(4, 5, 6)) == Vec4f(5, 7, 9, 2) );
        CHECK( (Vec4f(1, 2, 3) - Vec4f(4, 5, 6)) == Vec4f(-3, -3, -3, 0) );
        CHECK( (Vec4f(1, 2, 3) * Vec4f(4, 5, 6)) == Vec4f(4, 10, 18, 1) );
        CHECK( (Vec4f(1, 2, 3) / Vec4f(4, 5, 6)) == Vec4f(.25f, .4f, .5f) );
        CHECK( (Vec4f(1, 2, 3) / Vec4f(4, 5, 0)).isNaN() ); // zero in divisor
        CHECK( !(Vec4f(1, 2, 3) / Vec4f(4, 5, 6)).isNaN());
    }

    TEST_CASE("Math assignment operators with other vectors, and isNaN check")
    {
        CHECK( (Vec4f(1, 2, 3) += Vec4f(4, 5, 6)) == Vec4f(5, 7, 9, 2) );
        CHECK( (Vec4f(1, 2, 3) -= Vec4f(4, 5, 6)) == Vec4f(-3, -3, -3, 0) );
        CHECK( (Vec4f(1, 2, 3) *= Vec4f(4, 5, 6)) == Vec4f(4, 10, 18, 1) );
        CHECK( (Vec4f(1, 2, 3) /= Vec4f(4, 5, 6)) == Vec4f(.25f, .4f, .5f) );
        CHECK( (Vec4f(1, 2, 3) /= Vec4f(4, 5, 0)).isNaN() ); // zero in divisor
        CHECK( !(Vec4f(1, 2, 3) /= Vec4f(4, 5, 6)).isNaN());
    }

    TEST_CASE("Math operators with scalars")
    {
        CHECK( (Vec4f(10, 20, 30) * 10) == Vec4f(100, 200, 300, 10) );
        CHECK( (Vec4f(10, 20, 30) * 0) == Vec4f::Zero );
        CHECK( (Vec4f(10, 20, 30) / 10.f) == Vec4f(1, 2, 3, .1f) );
        CHECK( (Vec4f(10, 20, 30) / 0).isNaN() );

        CHECK( (10 * Vec4f(10, 20, 30)) == Vec4f(100, 200, 300, 10) );
        CHECK( (0 * Vec4f(10, 20, 30)) == Vec4f::Zero );
        CHECK( (600 / Vec4f(10, 20, 30)) == Vec4f(60, 30, 20, 600) );
        CHECK( (100 / Vec4f::Zero ).isNaN() );
    }

    TEST_CASE("Casting operators between vectors")
    {
        CHECK( static_cast<Vec4i>(Vec4f(10.5f, 20.3f, 30.23f)) == Vec4i(10, 20, 30) ); // truncate to int
        constexpr auto v = static_cast<Vec4d>(Vec4f(10.5f, 20.3f, 30.23f));
        CHECK( v.x == doctest::Approx(10.5) );
        CHECK( v.y == doctest::Approx(20.3) );
        CHECK( v.z == doctest::Approx(30.23) );
        CHECK( v.w == 1 );

        // Different sized vectors
        CHECK( static_cast<Vec2i>(Vec4f(1.1f, 2.2f, 3.3f)) == Vec2i(1, 2) );
        CHECK( static_cast<Vec3i>(Vec4f(1.1f, 2.2f, 3.3f)) == Vec3i(1, 2, 3) );
    }

    TEST_CASE("Swizzle test")
    {
        Vec4i v3(1, 2, 3, 4);
        CHECK( v3.swizzle<'x', 'y', 'z', 'w'>() == Vec4i(1, 2, 3, 4) );
        CHECK( v3.swizzle<'w', 'x', 'y', 'z'>() == Vec4i(4, 1, 2, 3) );
        CHECK( v3.swizzle<'x', 'y', 'w', 'x'>() == Vec4i(1, 2, 4, 1) );
        CHECK( v3.swizzle<'z', 'y', 'x', 'w'>() == Vec4i(3, 2, 1, 4) );
        CHECK( v3.swizzle<'z', 'z', 'w', 'x'>() == Vec4i(3, 3, 4, 1) );
        CHECK( v3.swizzle<'y', 'y', 'y', 'y'>() == Vec4i(2, 2, 2, 2) );
        CHECK( v3.swizzle<'z', 'z', 'z', 'z'>() == Vec4i(3, 3, 3, 3) );

        CHECK( v3.swizzle<'x', 'y', 'z'>() == Vec3i(1, 2, 3) );
        CHECK( v3.swizzle<'x', 'w', 'y'>() == Vec3i(1, 4, 2) );
        CHECK( v3.swizzle<'x', 'y', 'w'>() == Vec3i(1, 2, 4) );
        CHECK( v3.swizzle<'z', 'y', 'x'>() == Vec3i(3, 2, 1) );
        CHECK( v3.swizzle<'w', 'z', 'x'>() == Vec3i(4, 3, 1) );
        CHECK( v3.swizzle<'y', 'y', 'y'>() == Vec3i(2, 2, 2) );
        CHECK( v3.swizzle<'z', 'z', 'z'>() == Vec3i(3, 3, 3) );

        CHECK( v3.swizzle<'x', 'w'>() == Vec2i(1, 4) );
        CHECK( v3.swizzle<'y', 'x'>() == Vec2i(2, 1) );
        CHECK( v3.swizzle<'z', 'y'>() == Vec2i(3, 2) );
        CHECK( v3.swizzle<'w', 'z'>() == Vec2i(4, 3) );
        CHECK( v3.swizzle<'x', 'z'>() == Vec2i(1, 3) );
        CHECK( v3.swizzle<'w', 'y'>() == Vec2i(4, 2) );

        CHECK( v3.swizzle<'z'>() == Vec1i(3) );
        CHECK( v3.swizzle<'w'>() == Vec1i(4) );
    }
}
