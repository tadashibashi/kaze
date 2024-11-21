#include <doctest/doctest.h>
#include <kaze/core/math.h>

USING_KAZE_NAMESPACE;

TEST_SUITE("Geometry")
{
    TEST_CASE("overlaps(Rect, Circle)")
    {
        // Zero sized
        CHECK(!mathf::overlaps(Rectf(0, 0, 0, 0), Circf(0, 0, 10)));
        CHECK(!mathf::overlaps(Rectf(0, 0, 10, 10), Circf(0, 0, 0)));
        CHECK(!mathf::overlaps(Rectf(0, 0, 0, 0), Circf(0, 0, 0)));

        // From the bottom
        CHECK(!mathf::overlaps(Rectf(0, 0, 10, 10), Circf(0, 15, 5)));
        CHECK(mathf::overlaps(Rectf(0, 0, 10, 10), Circf(0, 15, 5.1f)));

        // From the right
        CHECK(!mathf::overlaps(Rectf(0, 0, 10, 10), Circf(15, 0, 5)));
        CHECK(mathf::overlaps(Rectf(0, 0, 10, 10), Circf(15, 0, 5.1f)));

        // From the top
        CHECK(!mathf::overlaps(Rectf(0, 0, 10, 10), Circf(0, -5, 5)));
        CHECK(mathf::overlaps(Rectf(0, 0, 10, 10), Circf(0, -5, 5.1f)));

        // From the left
        CHECK(!mathf::overlaps(Rectf(0, 0, 10, 10), Circf(-5, 0, 5)));
        CHECK(mathf::overlaps(Rectf(0, 0, 10, 10), Circf(-5, 0, 5.1f)));
    }

    TEST_CASE("overlaps(Circle, Rect)")
    {
        // Zero sized
        CHECK(!mathf::overlaps(Circf(0, 0, 10), Rectf(0, 0, 0, 0)));
        CHECK(!mathf::overlaps(Circf(0, 0, 0), Rectf(0, 0, 10, 10)));
        CHECK(!mathf::overlaps(Circf(0, 0, 0), Rectf(0, 0, 0, 0)));

        // From the bottom
        CHECK(!mathf::overlaps(Circf(0, 15, 5), Rectf(0, 0, 10, 10)));
        CHECK(mathf::overlaps(Circf(0, 15, 5.1f), Rectf(0, 0, 10, 10)));

        // From the right
        CHECK(!mathf::overlaps(Circf(15, 0, 5), Rectf(0, 0, 10, 10)));
        CHECK(mathf::overlaps(Circf(15, 0, 5.1f), Rectf(0, 0, 10, 10)));

        // From the top
        CHECK(!mathf::overlaps(Circf(0, -5, 5), Rectf(0, 0, 10, 10)));
        CHECK(mathf::overlaps(Circf(0, -5, 5.1f), Rectf(0, 0, 10, 10)));

        // From the left
        CHECK(!mathf::overlaps(Circf(-5, 0, 5), Rectf(0, 0, 10, 10)));
        CHECK(mathf::overlaps(Circf(-5, 0, 5.1f), Rectf(0, 0, 10, 10)));
    }

    TEST_CASE("overlaps(Rect, Rect)")
    {
        // No area
        CHECK(!mathf::overlaps(Rectf(0, 0, 10, 10), Rectf(0, 0, 0, 0)));
        CHECK(!mathf::overlaps(Rectf(0, 0, 0, 0), Rectf(0, 0, 10, 10)));
        CHECK(!mathf::overlaps(Rectf(0, 0, 0, 0), Rectf(0, 0, 0, 0)));

        // From the left
        CHECK(!mathf::overlaps(Rectf(0, 0, 10, 10), Rectf(-10, 0, 10, 10)));
        CHECK(mathf::overlaps(Rectf(0, 0, 10, 10), Rectf(-10, 0, 10.1f, 10)));

        // From the right
        CHECK(!mathf::overlaps(Rectf(0, 0, 10, 10), Rectf(10, 0, 5, 5)));
        CHECK(mathf::overlaps(Rectf(0, 0, 10, 10), Rectf(9.9f, 0, 5, 5)));

        // From the top
        CHECK(!mathf::overlaps(Rectf(0, 0, 10, 10), Rectf(0, -5, 5, 5)));
        CHECK(mathf::overlaps(Rectf(0, 0, 10, 10), Rectf(0, -5, 5, 5.1f)));

        // From the bottom
        CHECK(!mathf::overlaps(Rectf(0, 0, 10, 10), Rectf(0, 10, 5, 5)));
        CHECK(mathf::overlaps(Rectf(0, 0, 10, 10), Rectf(0, 9.9f, 5, 5.1f)));
    }

    TEST_CASE("overlaps(Rect, Vec2)")
    {
        // No area
        CHECK(!mathf::overlaps(Rectf(0, 0, 0, 0), Vec2f::Zero));

        // From the left
        CHECK(!mathf::overlaps(Rectf(0, 0, 10, 10), Vec2f(-1, 0)));
        CHECK(mathf::overlaps(Rectf(0, 0, 10, 10), Vec2f(0, 0)));

        // From the right
        CHECK(!mathf::overlaps(Rectf(0, 0, 10, 10), Vec2f(10.0f, 0)));
        CHECK(mathf::overlaps(Rectf(0, 0, 10, 10), Vec2f(9.9f, 0)));

        // From the top
        CHECK(!mathf::overlaps(Rectf(0, 0, 10, 10), Vec2f(0, -1)));
        CHECK(mathf::overlaps(Rectf(0, 0, 10, 10), Vec2f(0, 0)));

        // From the bottom
        CHECK(!mathf::overlaps(Rectf(0, 0, 10, 10), Vec2f(0, 10)));
        CHECK(mathf::overlaps(Rectf(0, 0, 10, 10), Vec2f(0, 9.9f)));
    }

    TEST_CASE("overlaps(Vec2, Rect)")
    {
        // No area
        CHECK(!mathf::overlaps(Vec2f::Zero, Rectf(0, 0, 0, 0)));

        // From the left
        CHECK(!mathf::overlaps(Vec2f(-1, 0), Rectf(0, 0, 10, 10)));
        CHECK(mathf::overlaps(Vec2f(0, 0), Rectf(0, 0, 10, 10)));

        // From the right
        CHECK(!mathf::overlaps(Vec2f(10.0f, 0), Rectf(0, 0, 10, 10)));
        CHECK(mathf::overlaps(Vec2f(9.9f, 0), Rectf(0, 0, 10, 10)));

        // From the top
        CHECK(!mathf::overlaps(Vec2f(0, -1), Rectf(0, 0, 10, 10)));
        CHECK(mathf::overlaps(Vec2f(0, 0), Rectf(0, 0, 10, 10)));

        // From the bottom
        CHECK(!mathf::overlaps(Vec2f(0, 10), Rectf(0, 0, 10, 10)));
        CHECK(mathf::overlaps(Vec2f(0, 9.9f), Rectf(0, 0, 10, 10)));
    }

    TEST_CASE("overlaps(Vec2, Vec2)")
    {
        CHECK(mathf::overlaps(Vec2f::Zero, Vec2f::Zero));

        // floors pixel check
        CHECK(mathf::overlaps(Vec2f::Zero, Vec2f(0, 0.1f)));
        CHECK(mathf::overlaps(Vec2f::Zero, Vec2f(.1f, 0))); // rounds to pixel
        CHECK(mathf::overlaps(Vec2f::Zero, Vec2f(0, -0.1f))); // rounds to pixel
        CHECK(mathf::overlaps(Vec2f::Zero, Vec2f(-0.1f, 0)));

        CHECK(!mathf::overlaps(Vec2f::Zero, Vec2f::One));
    }

    TEST_CASE("overlaps(Vec2, Circle)")
    {
        // Zero radius and area
        CHECK(!mathf::overlaps(Vec2f::Zero, Circf(0, 0, 0)));

        CHECK( !mathf::overlaps(Vec2f(1.f/mathf::sqrt(2.f), 1.f/mathf::sqrt(2.f)), Circf(0, 0, 1)) );
        CHECK( mathf::overlaps(Vec2f(1.f/mathf::sqrt(2.f), 1.f/mathf::sqrt(2.f)), Circf(0, 0, 1.1f)) );

        CHECK( !mathf::overlaps(Vec2f(1.f, 0), Circf(0, 0, 1.f)) );
        CHECK( mathf::overlaps(Vec2f(1.f, 0), Circf(0, 0, 1.1f)) );
        CHECK( !mathf::overlaps(Vec2f(1.f, 0), Circf(0, 0, 1.0f)) );
        CHECK( mathf::overlaps(Vec2f(-1.f, 0), Circf(0, 0, 1.1f)) );
    }

    TEST_CASE("overlaps(Circle, Vec2)")
    {
        // Zero radius and area
        CHECK(!mathf::overlaps(Circf(0, 0, 0), Vec2f::Zero));

        CHECK( !mathf::overlaps(Circf(0, 0, 1), Vec2f(1.f/mathf::sqrt(2.f), 1.f/mathf::sqrt(2.f))) );
        CHECK( mathf::overlaps(Circf(0, 0, 1.1f), Vec2f(1.f/mathf::sqrt(2.f), 1.f/mathf::sqrt(2.f))) );

        CHECK( !mathf::overlaps(Circf(0, 0, 1.f), Vec2f(1.f, 0)) );
        CHECK( mathf::overlaps(Circf(0, 0, 1.1f), Vec2f(1.f, 0)) );
        CHECK( !mathf::overlaps(Circf(0, 0, 1.0f), Vec2f(1.f, 0)) );
        CHECK( mathf::overlaps(Circf(0, 0, 1.1f), Vec2f(-1.f, 0)) );
    }

    TEST_CASE("overlaps(Circle, Circle)")
    {
        // Zero radius and area
        CHECK( !mathf::overlaps(Circf(0, 0, 0), Circf(0, 0, 1)) );
        CHECK( !mathf::overlaps(Circf(0, 0, 1), Circf(0, 0, 0)) );
        CHECK( !mathf::overlaps(Circf(0, 0, 0), Circf(0, 0, 0)) );

        // Perfect overlap
        CHECK( mathf::overlaps(Circf(0, 0, 1), Circf(0, 0, 1)) );

        // Sides
        CHECK( !mathf::overlaps(Circf(0, 0, 1), Circf(2, 0, 1)) );
        CHECK( mathf::overlaps(Circf(0, 0, 1), Circf(1.9f, 0, 1)) );
        CHECK( !mathf::overlaps(Circf(0, 0, 1), Circf(0, 2, 1)) );
        CHECK( mathf::overlaps(Circf(0, 0, 1), Circf(0, 1.9f, 1)) );
        CHECK( !mathf::overlaps(Circf(0, 0, 1), Circf(-2, 0, 1)) );
        CHECK( mathf::overlaps(Circf(0, 0, 1), Circf(-1.9f, 0, 1)) );
        CHECK( !mathf::overlaps(Circf(0, 0, 1), Circf(0, -2, 1)) );
        CHECK( mathf::overlaps(Circf(0, 0, 1), Circf(0, -1.9f, 1)) );
    }
}
