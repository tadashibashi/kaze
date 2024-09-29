#include <doctest/doctest.h>

#include <kaze/kaze.h>
#include <kaze/math/Circle.h>

USING_KAZE_NAMESPACE;

TEST_SUITE("Circle")
{
    TEST_CASE("Default Constructor")
    {
        constexpr Circf c;
        CHECK( c.x == 0 );
        CHECK( c.y == 0 );
        CHECK( c.radius == 0 );
    }

    TEST_CASE("Parameter Constructor(s)")
    {
        constexpr Circf c(10, 20, 4);
        CHECK( c.x == 10 );
        CHECK( c.y == 20 );
        CHECK( c.radius == 4 );
    }

    TEST_CASE("Set")
    {
        Circf c;
        CHECK( &c.set(10, 20, 30) == &c );
        CHECK( c.x == 10 );
        CHECK( c.y == 20 );
        CHECK( c.radius == 30 );

        CHECK( &c.setX(20).setY(50).setRadius(25) == &c );
        CHECK( c.x == 20 );
        CHECK( c.y == 50 );
        CHECK( c.radius == 25 );
    }

    TEST_CASE("Diameter")
    {
        CHECK( Circd(40, 50, 10).diameter() == 20.0 );
        CHECK( Circd(40, 50, 0).diameter() == 0 );
        CHECK( Circd(40, 50, -15.5).diameter() == -31.0 );
    }

    TEST_CASE("Area")
    {
        CHECK( Circi(10, 10, 10).area() == doctest::Approx( 10.L * 10.L * mathf::Pi ) );
        CHECK( Circi(10, 10, 0).area() == 0 );
        CHECK( Circd(10, 10, -5.5).area() == doctest::Approx( -5.5 * -5.5 * mathf::Pi) );
    }

    TEST_CASE("Position")
    {
        CHECK( Circf(15.f, 20.5f, 1.f).position == Vec2f(15.f, 20.5f) );
        CHECK( Circf(-1.23f, 4.32f, 1.f).position == Vec2f(-1.23f, 4.32f) );
    }
}
