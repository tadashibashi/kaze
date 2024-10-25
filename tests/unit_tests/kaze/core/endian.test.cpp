#include <doctest/doctest.h>
#include <kaze/core/endian.h>

USING_KAZE_NAMESPACE;

TEST_SUITE("Endian")
{
    TEST_CASE("opposite")
    {
        CHECK(Endian::opposite(Endian::Big) == Endian::Little);
        CHECK(Endian::opposite(Endian::Little) == Endian::Big);
        CHECK(Endian::opposite(Endian::Unknown) == Endian::Unknown);
        CHECK(Endian::opposite(
            static_cast<Endian::Type>(123456)) == Endian::Unknown);
    }

    TEST_CASE("isLittle/isBig")
    {
        CHECK(Endian::isLittle() != Endian::isBig());
    }

    TEST_CASE("swap")
    {
        union IntChecker {
            Uint u32;
            Ubyte u8[sizeof(Int)];
        };

        IntChecker a;
        a.u32 = 0x1234U;
        auto b = Endian::swap(a);

        CHECK(b.u32 != a.u32);
        CHECK(a.u8[0] == b.u8[3]);
        CHECK(a.u8[1] == b.u8[2]);
        CHECK(a.u8[2] == b.u8[1]);
        CHECK(a.u8[3] == b.u8[0]);
    }
}