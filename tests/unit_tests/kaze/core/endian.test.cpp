#include <doctest/doctest.h>
#include <kaze/core/endian.h>

USING_KAZE_NAMESPACE;

TEST_SUITE("Endian")
{
    TEST_CASE("opposite")
    {
        CHECK(endian::opposite(endian::Big) == endian::Little);
        CHECK(endian::opposite(endian::Little) == endian::Big);
        CHECK(endian::opposite(endian::Unknown) == endian::Unknown);
        CHECK(endian::opposite(
            static_cast<endian::Type>(123456)) == endian::Unknown);
    }

    TEST_CASE("isLittle/isBig")
    {
        CHECK(endian::isLittle() != endian::isBig());
    }

    TEST_CASE("swap")
    {
        union IntChecker {
            Uint u32;
            Ubyte u8[sizeof(Int)];
        };

        IntChecker a;
        a.u32 = 0x1234U;
        auto b = endian::swap(a);

        CHECK(b.u32 != a.u32);
        CHECK(a.u8[0] == b.u8[3]);
        CHECK(a.u8[1] == b.u8[2]);
        CHECK(a.u8[2] == b.u8[1]);
        CHECK(a.u8[3] == b.u8[0]);
    }
}
