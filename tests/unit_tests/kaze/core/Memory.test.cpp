#include <doctest/doctest.h>
#include <kaze/core/MemView.h>

USING_KAZE_NAMESPACE;

TEST_SUITE("Memory")
{
    TEST_CASE("Default constructor")
    {
        const auto mem = Mem{};
        CHECK(mem.data() == nullptr);
        CHECK(mem.elemCount() == 0);
    }

    TEST_CASE("Manual data constructor")
    {
        const auto data = "abc";
        const auto mem = makeRef(data, std::strlen(data));
        CHECK(mem.data() == data);
        CHECK(mem.elemCount() == std::strlen(data));
    }

    TEST_CASE("Cstring constructor")
    {
        const auto data = "abc";
        const auto mem = makeRef(data, 3);
        CHECK(mem.data() == data);
        CHECK(mem.elemCount() == std::strlen(data));
    }

    TEST_CASE("Array constructor")
    {
        SUBCASE("Single byte-sized elements")
        {
            constexpr Uint8 arr[] = { 0, 1, 2, 3 };
            const auto mem = makeRef(arr);
            CHECK(mem.data() == arr);
            CHECK(mem.elemCount() == sizeof(arr));
        }

        SUBCASE("Multiple byte-sized elements")
        {
            constexpr Uint64 arr[] = { 0, 1, 2, 3 };
            const auto mem = makeRef(arr);
            CHECK(mem.data() == arr);
            CHECK(mem.elemCount() == std::size(arr));
            CHECK(mem.size() == sizeof(arr));
        }
    }

    TEST_CASE("Container constructor")
    {
        SUBCASE("Single byte-sized elements")
        {
            const List<Uint8> list = {0, 1, 2, 3};
            const auto mem = makeRef(list);
            CHECK(mem.data() == list.data());
            CHECK(mem.size() == list.size());
            CHECK(mem.elemCount() == list.size());
        }

        SUBCASE("String")
        {
            const String str = "abcdefg";
            const auto mem = makeRef(str);
            CHECK(mem.data() == str.data());
            CHECK(mem.size() == str.size());
        }

        SUBCASE("StringView")
        {
            constexpr StringView str = "abcdefghijklmnopqrstuvwxyz1234567890";
            const auto mem = makeRef(str);
            CHECK(mem.data() == str.data());
            CHECK(mem.size() == str.size());
        }

        SUBCASE("Multiple byte-sized elements")
        {
            constexpr Array<Uint64, 6> list = {0, 1, 2, 3, 4, 5};
            const auto mem = makeRef(list);
            CHECK(mem.data() == list.data());
            CHECK(mem.size() == list.size() * sizeof(Uint64));
            CHECK(mem.elemCount() == list.size());
        }
    }
}
