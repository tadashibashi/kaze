#include <doctest/doctest.h>
#include <kaze/core/io/BufferView.h>

#include <cstring>

USING_KAZE_NAMESPACE;

TEST_SUITE("BufferView")
{
    TEST_CASE("Empty initialization")
    {
        BufferView bv{};
        CHECK(bv.size() == 0);
        CHECK(!bv.eof());
        CHECK(bv.tell() == 0);

        char chars[4];
        CHECK(bv.read(chars, 4) == 0);
        CHECK(bv.eof());
        CHECK(bv.tell() == 0);
    }

    TEST_CASE("Buffer initialization")
    {
        auto message = "abcdefg";

        BufferView bv(makeRef(message, 7));
        CHECK(bv.size() == 7);

        char messageBuf[8] = {0};
        CHECK(bv.read(messageBuf, 7) == 7);
        CHECK(bv.tell() == 7);
        CHECK( std::strncmp(message, messageBuf, 7) == 0 );
    }

    TEST_CASE("Container initialialization")
    {
        SUBCASE("String")
        {
            String data = "abcdefg";
            BufferView bv(makeRef(data));
            CHECK(bv.size() == 7);

            char messageBuf[8] = {0};
            CHECK(bv.read(messageBuf, 7) == 7);
            CHECK(bv.tell() == 7);
            CHECK(messageBuf == data);
        }

        SUBCASE("List/vector")
        {
            List<Uchar> data = { 'a', 'b', 'c', 'd', 'e', 'f', 'g' };
            BufferView bv(makeRef(data));
            CHECK(bv.size() == 7);

            char messageBuf[8] = {0};
            CHECK(bv.read(messageBuf, 7) == 7);
            CHECK(bv.tell() == 7);
            CHECK(messageBuf[0] == data[0]);
            CHECK(messageBuf[1] == data[1]);
            CHECK(messageBuf[2] == data[2]);
            CHECK(messageBuf[3] == data[3]);
            CHECK(messageBuf[4] == data[4]);
            CHECK(messageBuf[5] == data[5]);
            CHECK(messageBuf[6] == data[6]);
        }
    }

    TEST_CASE("operator >>")
    {
        int ints[] = {0, 1, 2, 3, 4, 5, 6, 7};

        BufferView bv(makeRef(ints));
        CHECK(bv.size() == sizeof(ints));

        for (int i = 0; i < std::size(ints); ++i)
        {
            int result;
            CHECK( static_cast<bool>(bv >> result) );
            CHECK(result == i);
            CHECK(bv.tell() == sizeof(int) * (i + 1));
        }

        CHECK( !bv.eof() );

        int result;
        CHECK(bv.read(&result, sizeof(int)) == 0);
        CHECK(bv.eof());
    }

    TEST_CASE("tryRead, read templates")
    {
        int ints[] = {0, 1, 2, 3};

        BufferView bv(makeRef(ints));

        int result;
        CHECK(bv.tryRead(&result));
        CHECK(result == 0);

        CHECK(bv.read<int>() == 1);

        CHECK(bv.tryRead(&result));
        CHECK(result == 2);

        CHECK(bv.read<int>() == 3);

        CHECK_THROWS(result = bv.read<int>());
        CHECK(bv.eof());
        CHECK( !bv.tryRead(&result) );
        CHECK(bv.eof());
        CHECK(result == 2); // it's the same as the last successful callto tryRead
    }

    TEST_CASE("seek")
    {
        int ints[] = {0, 1, 2, 3};
        BufferView bv(makeRef(ints));

        bv.seek(sizeof(int) * 2, SeekBase::Start);
        CHECK(bv.read<int>() == 2);

        bv.seek(sizeof(int) * -3, SeekBase::End);
        CHECK(bv.read<int>() == 1);

        bv.seek(sizeof(int), SeekBase::Relative);
        CHECK(bv.read<int>() == 3);

        // Clamps on both ends
        bv.seek(100000, SeekBase::Start);
        CHECK(bv.tell() == sizeof(ints));

        bv.seek(-100, SeekBase::Start);
        CHECK(bv.tell() == 0);
    }

    TEST_CASE("endianness")
    {
        Endian::Type endianness = (Endian::Native == Endian::Big) ?
            Endian::Little : Endian::Big;

        int ints[] = {10, 11, 12, 13};
        BufferView bv(makeRef(ints));

        int value;
        bv.read(&value, sizeof(int), endianness);
        CHECK(value != 10);

        value = Endian::swap(value);
        CHECK(value == 10);

        bv.read(&value, sizeof(int), endianness);
        CHECK(Endian::swap(value) == 11);

        bv.read(&value, sizeof(int), endianness);
        CHECK(Endian::swap(value) == 12);

        bv.read(&value, sizeof(int), endianness);
        CHECK(Endian::swap(value) == 13);

        CHECK(bv.read(&value, sizeof(int), endianness) == 0);
        CHECK(bv.eof());
    }

    TEST_CASE("overreads")
    {
        SUBCASE("string")
        {
            auto data = "WAVElab";
            char header[4];
            BufferView bv(makeRef(data));

            CHECK(bv.read(header, 4) == 4);
            CHECK(std::strncmp(header, "WAVE", 4) == 0);

            CHECK(bv.read(header, 4) == 3);
            CHECK(std::strncmp(header, "lab", 3) == 0);
        }

        SUBCASE("numbers")
        {
            int32_t ints[] = {10, 20};
            BufferView bv(makeRef(ints));

            int value;
            CHECK(bv.read(&value, sizeof(int)) == sizeof(int));
            CHECK(value == 10);

            CHECK_THROWS(value = bv.read<int64_t>()); // reads over the bounds
        }
    }

    TEST_CASE("read string")
    {
        SUBCASE("regular string")
        {
            auto str = "abcdefghijklmnop";
            auto length = std::strlen(str);

            BufferView bv(makeRef(str, length));

            String readStr;
            CHECK(bv.readString(&readStr) == length);
            CHECK(readStr == str);
            CHECK(bv.tell() == length);
        }

        SUBCASE("maxLength clipping")
        {
            auto str = "abcdefghijklmnop";
            auto length = std::strlen(str);

            BufferView bv(makeRef(str, length));

            String value;
            CHECK(bv.readString(&value, 4) == 4);
            CHECK(value == "abcd");
            CHECK(bv.tell() == 4);
        }

        SUBCASE("multiple strings")
        {
            auto str = "abc\0def";
            BufferView bv(makeRef(str, 7));

            String value;
            CHECK(bv.readString(&value) == 3);
            CHECK(value == "abc");
            CHECK(bv.tell() == 4); // +1 due to null terminator

            CHECK(bv.readString(&value) == 3);
            CHECK(value == "def");
            CHECK(bv.tell() == 7);
        }
    }
}
