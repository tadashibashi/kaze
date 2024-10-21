#include <doctest/doctest.h>

#include <kaze/core/math/Rect.h>

USING_KAZE_NAMESPACE;

TEST_SUITE("Rect")
{
    TEST_CASE("Constructors")
    {
        SUBCASE("Default Constructor")
        {
            constexpr Rectf r;
            CHECK(r.x == 0);
            CHECK(r.y == 0);
            CHECK(r.w == 0);
            CHECK(r.h == 0);
        }

        SUBCASE("Parameter Constructor")
        {
            constexpr Recti r(1, 2, 3, 4);
            CHECK(r.x == 1);
            CHECK(r.y == 2);
            CHECK(r.w == 3);
            CHECK(r.h == 4);
        }
    }

    TEST_CASE("area")
    {
        CHECK( Recti(1, 2, 3, 4).area() == 12 );
        CHECK( Rectf(1, 2, -3, -4).area() == 12 );
        CHECK( Rectf(1, 2, 3, -4).area() == 12 );
        CHECK( Rectf(1, 2, -3, 4).area() == 12 );
        CHECK( Rectf(1, 2, -3, 0).area() == 0 );
    }

    TEST_CASE("position")
    {
        CHECK( Rectd(10.5, 2.7, 1.9, 100.103).position == Vec2d(10.5, 2.7) );
        CHECK( Recti(4, 2, -19, 0).position == Vec2i(4, 2) );
        CHECK( Rectf(10.9f, -93.3f, 40.34f, -10.8f).position == Vec2f(10.9f, -93.3f) );

        CHECK( (Rectf().position = {10, 20}) == Vec2f(10, 20));
        CHECK( (Rectf().position = {10, 20}) == Vec2f(10, 20));
    }

    TEST_CASE("size")
    {
        CHECK( Rectd(10.5, 2.7, 1.9, 100.103).size == Vec2d(1.9, 100.103) );
        CHECK( Recti(4, 2, -19, 0).size == Vec2i(-19, 0) );
        CHECK( Rectf(10.9f, -93.3f, 40.34f, -10.8f).size == Vec2f(40.34f, -10.8f) );

        CHECK( (Rectf().size = {10, 20}) == Vec2f(10, 20));
        CHECK( (Rectf().size = {10, 20}) == Vec2f(10, 20));
    }

    TEST_CASE("diagonal")
    {
        CHECK( Rectd(10.5, 2.7, 1.9, 100.103).diagonal() == doctest::Approx( mathf::sqrt(1.9 * 1.9 + 100.103 * 100.103) ) );
        CHECK( Recti(4, 2, -19, 0).diagonal() == doctest::Approx( mathf::sqrt(-19 * -19)) );
        CHECK( Rectf(10.9f, -93.3f, 40.34f, -10.8f).diagonal() == doctest::Approx( mathf::sqrt(40.34f * 40.34f + -10.8f * -10.8f) ) );
    }

    TEST_CASE("expand")
    {
        SUBCASE("from zero")
        {
            Rectf r;
            CHECK( &r.expand(10, 10, 10, 10) == &r );
            CHECK( r.x == -10 );
            CHECK( r.y == -10 );
            CHECK( r.w == 20);
            CHECK( r.h == 20);
        }

        SUBCASE("to zero")
        {
            Rectf r(0, 0, 100, 100);
            r.expand(-200, -200, -200, -200);
            CHECK( r.x == 0 );
            CHECK( r.y == 0 );
            CHECK( r.w == 0 );
            CHECK( r.h == 0 );
        }

        SUBCASE("expand positively")
        {
            Rectf r(-50, -50, 100, 100);
            r.expand(200, 200, 200, 200);
            CHECK( r.x == -250 );
            CHECK( r.y == -250 );
            CHECK( r.w == 500 );
            CHECK( r.h == 500 );
        }

        SUBCASE("expand negatively")
        {
            Rectf r(-50, -50, 100, 100);
            r.expand(-25, -25, -25, -25);
            CHECK( r.x == -25 );
            CHECK( r.y == -25 );
            CHECK( r.w == 50 );
            CHECK( r.h == 50 );
        }

        SUBCASE("expand different values")
        {
            Rectf r(50, 50, 100, 500);
            r.expand(1, 2, 3, 4);

            CHECK( r.x == 49 );
            CHECK( r.y == 48 );
            CHECK( r.w == 104 );
            CHECK( r.h == 506 );
        }

        SUBCASE("expand all overload")
        {
            Rectf r(10, 10, 10, 10);
            CHECK( &r.expand(10) == &r );

            CHECK( r.x == 0 );
            CHECK( r.y == 0 );
            CHECK( r.w == 30 );
            CHECK( r.h == 30 );
        }

        SUBCASE("expand horizontal vertical overload")
        {
            Recti r(9, 9, 3, 3);
            CHECK( &r.expand(3, 6));

            CHECK( r.x == 6 );
            CHECK( r.y == 3 );
            CHECK( r.w == 9 );
            CHECK( r.h == 15 );
        }
    }

    TEST_CASE("left, right, up, down")
    {
        SUBCASE("Get left")
        {
            CHECK( Rectd(10, 20, 30, 40).left() == 10 );
            CHECK( Rectd(-20, 20, 30, 40).left() == -20 );
        }

        SUBCASE("Get right")
        {
            CHECK( Rectd(10, 20, 30, 40).right() == 40 );
            CHECK( Rectd(-20, 20, 30, 40).right() == 10 );
        }

        SUBCASE("Get top")
        {
            CHECK( Rectd(10, 20, 30, 40).top() == 20 );
            CHECK( Rectd(-20, 55, 30, 40).top() == 55 );
        }

        SUBCASE("Get bottom")
        {
            CHECK( Rectd(10, 20, 30, 40).bottom() == 60 );
            CHECK( Rectd(-20, 55, 30, 40).bottom() == 95 );
        }

        SUBCASE("Set left")
        {
            SUBCASE("Regular set")
            {
                Rectf r(0, 0, 10, 10);
                CHECK( &r.left(5) == &r );

                CHECK(r.left() == 5);
                CHECK(r.x == 5);
                CHECK(r.w == 5);
            }

            SUBCASE("Crossover")
            {
                Rectf r(0, 0, 10, 10);
                r.left(15);

                CHECK(r.left() == 15);
                CHECK(r.x == 15);
                CHECK(r.w == 0);
            }
        }

        SUBCASE("Set right")
        {
            SUBCASE("Regular set")
            {
                Rectf r(10, 10, 10, 10);
                CHECK( &r.right(15) == &r );

                CHECK(r.right() == 15);
                CHECK(r.x == 10);
                CHECK(r.w == 5);
            }

            SUBCASE("Crossover")
            {
                Rectf r(10, 10, 10, 10);
                r.right(5);

                CHECK(r.right() == 5);
                CHECK(r.x == 5);
                CHECK(r.w == 0);
            }
        }

        SUBCASE("Set top")
        {
            SUBCASE("Regular set")
            {
                Rectf r(10, 10, 10, 10);
                CHECK( &r.top(15) == &r );

                CHECK(r.top() == 15);
                CHECK(r.y == 15);
                CHECK(r.h == 5);
            }

            SUBCASE("Crossover")
            {
                Rectf r(10, 10, 10, 10);
                r.top(25);

                CHECK(r.top() == 25);
                CHECK(r.y == 25);
                CHECK(r.h == 0);
            }
        }

        SUBCASE("Set bottom")
        {
            SUBCASE("Regular set")
            {
                Rectf r(10, 10, 10, 10);
                CHECK( &r.bottom(15) == &r );

                CHECK( r.bottom() == 15 );
                CHECK( r.y == 10 );
                CHECK( r.h == 5 );
            }

            SUBCASE("Crossover")
            {
                Rectf r(10, 10, 10, 10);
                r.bottom(5);

                CHECK( r.bottom() == 5 );
                CHECK( r.y == 5 );
                CHECK( r.h == 0 );
            }
        }
    }
}
