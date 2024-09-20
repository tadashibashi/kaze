#include <doctest/doctest.h>
#include <nova/nova.h>
#include <nova/math/Rect3D.h>

using namespace NOVA_NAMESPACE;

TEST_SUITE("Rect3D")
{
    TEST_CASE("Constructor")
    {
        SUBCASE("Default")
        {
            Rect3Df rect3D{};
            CHECK(rect3D.x == 0);
            CHECK(rect3D.y == 0);
            CHECK(rect3D.z == 0);
            CHECK(rect3D.w == 0);
            CHECK(rect3D.h == 0);
            CHECK(rect3D.d == 0);
            CHECK(rect3D.position == Vec3f::Zero);
            CHECK(rect3D.size == Vec3f::Zero);
        }

        SUBCASE("Parametered")
        {
            Rect3Df rect3D({10, 20, 30}, {40, 50, 60});
            CHECK(rect3D.x == 10);
            CHECK(rect3D.y == 20);
            CHECK(rect3D.z == 30);
            CHECK(rect3D.w == 40);
            CHECK(rect3D.h == 50);
            CHECK(rect3D.d == 60);
            CHECK(rect3D.position == Vec3f(10, 20, 30));
            CHECK(rect3D.size == Vec3f(40, 50, 60));
        }
    }

    TEST_CASE("contains 3D Point")
    {
        /// Zero area
        CHECK(!Rect3Df({0, 0, 0}, {0, 0, 0}).contains(Vec3f(0, 0, 0)));
        CHECK(!Rect3Df({0, 0, 0}, {1, 0, 0}).contains(Vec3f(0, 0, 0)));
        CHECK(!Rect3Df({0, 0, 0}, {1, 1, 0}).contains(Vec3f(0, 0, 0)));
        CHECK(!Rect3Df({0, 0, 0}, {1, 0, 1}).contains(Vec3f(0, 0, 0)));
        CHECK(!Rect3Df({0, 0, 0}, {0, 0, 1}).contains(Vec3f(0, 0, 0)));
        CHECK(!Rect3Df({0, 0, 0}, {0, 1, 0}).contains(Vec3f(0, 0, 0)));
        CHECK(Rect3Df({0, 0, 0}, {1, 1, 1}).contains(Vec3f(0, 0, 0)));

        CHECK(!Rect3Df({0, 0, 0}, {10, 10, 10}).contains(Vec3f{10, 10, 10}));
        CHECK(Rect3Df({0, 0, 0}, {10, 10, 10}).contains(Vec3f{9.9f, 9.9f, 9.9f}));
    }

    TEST_CASE("overlaps with another Rect3D")
    {
        CHECK(!Rect3Df({0, 0, 0}, {0, 0, 0}).overlaps(Rect3Df({0, 0, 0}, {1, 1, 1})));
        CHECK(!Rect3Df({0, 0, 0}, {1, 1, 1}).overlaps(Rect3Df({0, 0, 0}, {0, 0, 0})));
        CHECK(!Rect3Df({0, 0, 0}, {0, 0, 0}).overlaps(Rect3Df({0, 0, 0}, {0, 0, 0})));
        CHECK(Rect3Df({0, 0, 0}, {1, 1, 1}).overlaps(Rect3Df({0, 0, 0}, {1, 1, 1})));

        CHECK(!Rect3Df({0, 0, 0}, {1, 1, 1}).overlaps(Rect3Df({-1, 0, 0}, {1, 1, 1})));
        CHECK(Rect3Df({0, 0, 0}, {1, 1, 1}).overlaps(Rect3Df({-1, 0, 0}, {1.1f, 1, 1})));
        CHECK(!Rect3Df({0, 0, 0}, {1, 1, 1}).overlaps(Rect3Df({0, -1, 0}, {1, 1, 1})));
        CHECK(Rect3Df({0, 0, 0}, {1, 1, 1}).overlaps(Rect3Df({0, -1, 0}, {1, 1.1f, 1})));
        CHECK(!Rect3Df({0, 0, 0}, {1, 1, 1}).overlaps(Rect3Df({0, 0, -1}, {1, 1, 1})));
        CHECK(Rect3Df({0, 0, 0}, {1, 1, 1}).overlaps(Rect3Df({0, 0, -1}, {1, 1, 1.1f})));

        CHECK(!Rect3Df({0, 0, 0}, {1, 1, 1}).overlaps(Rect3Df({1, 0, 0}, {1, 1, 1})));
        CHECK(Rect3Df({0, 0, 0}, {1, 1, 1}).overlaps(Rect3Df({0.9f, 0, 0}, {1, 1, 1})));
        CHECK(!Rect3Df({0, 0, 0}, {1, 1, 1}).overlaps(Rect3Df({0, 1, 0}, {1, 1, 1})));
        CHECK(Rect3Df({0, 0, 0}, {1, 1, 1}).overlaps(Rect3Df({0, 0.9f, 0}, {1, 1, 1})));
        CHECK(!Rect3Df({0, 0, 0}, {1, 1, 1}).overlaps(Rect3Df({0, 0, 1}, {1, 1, 1})));
        CHECK(Rect3Df({0, 0, 0}, {1, 1, 1}).overlaps(Rect3Df({0, 0, 0.9f}, {1, 1, 1})));
    }
}
