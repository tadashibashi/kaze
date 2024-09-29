#include <doctest/doctest.h>

#include <kaze/kaze.h>
#include <kaze/graphics/Color.h>

USING_KAZE_NAMESPACE;

TEST_SUITE("Color")
{
    TEST_CASE("Default constructor sets white color")
    {
        Color c{};

        CHECK(c.r == 255);
        CHECK(c.g == 255);
        CHECK(c.b == 255);
        CHECK(c.a == 255);
    }

    TEST_CASE("Parameter constructor sets values as expected")
    {
        Color c(22, 33, 44, 55);

        CHECK(c.r == 22);
        CHECK(c.g == 33);
        CHECK(c.b == 44);
        CHECK(c.a == 55);
    }

    TEST_CASE("Using set sets values as expected")
    {
        Color c{};
        CHECK(&c.set(22, 33, 44, 55) == &c); // assert it returns a ref to itself

        CHECK(c.r == 22);
        CHECK(c.g == 33);
        CHECK(c.b == 44);
        CHECK(c.a == 55);
    }

    TEST_CASE("setGrayScale sets values as expected")
    {
        Color c{};
        CHECK(&c.setGrayScale(123, 44) == &c);

        CHECK(c.r == 123);
        CHECK(c.g == 123);
        CHECK(c.b == 123);
        CHECK(c.a == 44);
    }

    TEST_CASE("fromGrayScale sets values as expected")
    {
        constexpr auto c = Color::fromGrayScale(123, 44);

        CHECK(c.r == 123);
        CHECK(c.g == 123);
        CHECK(c.b == 123);
        CHECK(c.a == 44);
    }

    TEST_CASE("fromRGBA8 sets values as expected")
    {
        constexpr auto c = Color::fromRGBA8(0x11223344u);

        CHECK(c.r == 0x11);
        CHECK(c.g == 0x22);
        CHECK(c.b == 0x33);
        CHECK(c.a == 0x44);
    }

    TEST_CASE("fromABGR8 sets values as expected")
    {
        constexpr auto c = Color::fromABGR8(0x22334411u);

        CHECK(c.r == 0x11);
        CHECK(c.g == 0x44);
        CHECK(c.b == 0x33);
        CHECK(c.a == 0x22);
    }

    TEST_CASE("fromARGB8 sets values as expected")
    {
        constexpr auto c = Color::fromARGB8(0x22334411u);

        CHECK(c.r == 0x33);
        CHECK(c.g == 0x44);
        CHECK(c.b == 0x11);
        CHECK(c.a == 0x22);
    }

    TEST_CASE("fromBGRA8 sets values as expected")
    {
        constexpr auto c = Color::fromBGRA8(0x22334411u);

        CHECK(c.r == 0x44);
        CHECK(c.g == 0x33);
        CHECK(c.b == 0x22);
        CHECK(c.a == 0x11);
    }

    TEST_CASE("setR sets value as expected")
    {
        Color c;
        CHECK(&c.setR(5) == &c);
        CHECK(c.r == 5);
    }

    TEST_CASE("setG sets value as expected")
    {
        Color c;
        CHECK(&c.setG(7) == &c);
        CHECK(c.g == 7);
    }

    TEST_CASE("setB sets value as expected")
    {
        Color c;
        CHECK(&c.setB(9) == &c);
        CHECK(c.b == 9);
    }

    TEST_CASE("setA sets value as expected")
    {
        Color c;
        CHECK(&c.setA(11) == &c);
        CHECK(c.a == 11);
    }

    TEST_CASE("data() gets pointer to r")
    {
        Color c;
        CHECK(c.data() == &c.r);

        constexpr Color cc;
        CHECK(cc.data() == &cc.r);
    }

    TEST_CASE("operator[] gets values as expected")
    {
        Color c(10, 11, 12, 13);

        CHECK(c[0] == 10);
        CHECK(c[1] == 11);
        CHECK(c[2] == 12);
        CHECK(c[3] == 13);
    }

    TEST_CASE("operator[] gets refs to set values as expected")
    {
        Color c;

        c[0] = 10;
        c[1] = 11;
        c[2] = 12;
        c[3] = 13;

        CHECK(c[0] == 10);
        CHECK(c[1] == 11);
        CHECK(c[2] == 12);
        CHECK(c[3] == 13);
    }

    TEST_CASE("operator== correctly compares colors")
    {
        Color a(10, 11, 12, 13);
        Color b(10, 11, 12, 13);

        CHECK(a == b);

        a.r = 100;
        CHECK(!a.operator==(b));
    }

    TEST_CASE("operator!= correctly compares colors")
    {
        Color a(10, 11, 12, 13);
        Color b(10, 11, 12, 13);

        CHECK(!a.operator!=(b));

        a.r = 100;
        CHECK(a != b);
    }

    TEST_CASE("toRGBA8() sets values as expected")
    {
        constexpr Color c(10, 20, 30, 40);
        constexpr auto result = c.toRGBA8();

        CHECK(((result >> 24u) & 0xFF) == 10);
        CHECK(((result >> 16u) & 0xFF) == 20);
        CHECK(((result >> 8u) & 0xFF) == 30);
        CHECK((result & 0xFF) == 40);
    }

    TEST_CASE("toABGR8() sets values as expected")
    {
        constexpr Color c(10, 20, 30, 40);
        constexpr auto result = c.toABGR8();

        CHECK(((result >> 24u) & 0xFF) == 40);
        CHECK(((result >> 16u) & 0xFF) == 30);
        CHECK(((result >> 8u) & 0xFF) == 20);
        CHECK((result & 0xFF) == 10);
    }

    TEST_CASE("toARGB8() sets values as expected")
    {
        constexpr Color c(10, 20, 30, 40);
        constexpr auto result = c.toARGB8();

        CHECK(((result >> 24u) & 0xFF) == 40);
        CHECK(((result >> 16u) & 0xFF) == 10);
        CHECK(((result >> 8u) & 0xFF) == 20);
        CHECK((result & 0xFF) == 30);
    }

    TEST_CASE("toBGRA8() sets values as expected")
    {
        constexpr Color c(10, 20, 30, 40);
        constexpr auto result = c.toBGRA8();

        CHECK(((result >> 24u) & 0xFF) == 30);
        CHECK(((result >> 16u) & 0xFF) == 20);
        CHECK(((result >> 8u) & 0xFF) == 10);
        CHECK((result & 0xFF) == 40);
    }
}
