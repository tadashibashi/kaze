#include <doctest/doctest.h>
#include <kaze/core/io/BufferWriter.h>
#include <kaze/core/io/BufferView.h>
USING_KAZE_NAMESPACE;

TEST_SUITE("BufferWriter")
{
    TEST_CASE("Default constructor")
    {
        BufferWriter writer{};
        CHECK(writer.empty());
        CHECK(writer.size() == 0);
        CHECK(writer.defaultArithmeticEndian() == Endian::Little);
        CHECK(writer.defaultStringEndian() == Endian::Big);
    }

    TEST_CASE("Write numeric data")
    {

        BufferWriter writer{};
        writer.write(10);
        writer.write(20);
        writer.write(30);

        CHECK(writer.size() == sizeof(int) * 3);

        BufferView view(writer.data(), writer.size());
        CHECK(view.read<int>() == 10);
        CHECK(view.read<int>() == 20);
        CHECK(view.read<int>() == 30);
    }

    TEST_CASE("Write string data")
    {
        String a = "abcdefg";
        String b = "hijklmn";
        BufferWriter writer{};
        writer.write(a);
        writer.write(b);

        CHECK(writer.size() == a.size() + 1 + b.size() + 1);

        BufferView view(writer.data(), writer.size());
        CHECK(view.readString() == a);
        CHECK(view.readString() == b);
    }

    TEST_CASE("Write fixed block string")
    {
        SUBCASE("fixed block exceeds string length")
        {
            String a = "abcdefg";
            BufferWriter writer{};
            writer.write(a, {.fixedBufSize = 27});

            CHECK(writer.size() == 27);
            for (auto i = a.size(); i < writer.size(); ++i)
            {
                CHECK(writer.data()[i] == 0);
            }
        }

        SUBCASE("string length clipped by fixed block")
        {
            String a = "abcdefg";
            BufferWriter writer{};
            writer.write(a, {.fixedBufSize = 3});

            CHECK(writer.size() == 3);

            BufferView view(writer.data(), writer.size());
            CHECK(view.readString() == "abc");
        }
    }

    TEST_CASE("Expansion")
    {
        BufferWriter writer({.initialSize = 4});


        CHECK(writer.maxSize() == 4);

        String str = "abcdefg";
        writer.write(str);
        CHECK(writer.maxSize() > 4);
        CHECK(writer.size() == str.length() + 1);
        BufferView view(writer.data(), writer.size());

        CHECK(view.readString() == str);
    }

    TEST_CASE("Combine string and numeric data using << operator")
    {
        BufferWriter writer{};
        String a = "abcdefghijklmnop";
        Int b = 12345;
        Float c = .1245f;
        String d = "!@#$!@#$!@#$";

        writer << a << b << c << d;

        CHECK(writer.size() == a.size() + 1 + sizeof(Int) + sizeof(Float) + d.size() + 1);

        BufferView view(writer.data(), writer.size());
        CHECK(view.readString() == a);
        CHECK(view.read<Int>() == b);
        CHECK(view.read<Float>() == c);
        CHECK(view.readString() == d);
    }

    TEST_CASE("Swap default endian")
    {
        BufferWriter writer({
            .arithmeticEndian = Endian::opposite(Endian::Native),
            .stringEndian     = Endian::Little,
        });

        String a = "01234"; String b = "43210";
        Int c = 12345;
        Int d =  Endian::swap(c);

        writer << a << b << c << d;

        BufferView view(writer.data(), writer.size());
        CHECK(view.readString() == b);
        CHECK(view.readString() == a);
        CHECK(view.read<Int>() == d);
        CHECK(view.read<Int>() == c);
    }

    TEST_CASE("Override endian")
    {
        BufferWriter writer{};

        String a = "01234"; String b = "43210";
        Int c = 12345;
        Int d =  Endian::swap(c);

        writer.write(a, {.endian = Endian::Little});
        writer.write(c, Endian::opposite(Endian::Native));

        BufferView view(writer.data(), writer.size());
        CHECK(view.readString() == b);
        CHECK(view.read<Int>() == d);
    }

    TEST_CASE("Clear")
    {
        BufferWriter writer{};
        String a = "abcdefghijklmnop";
        Int b = 12345;
        Float c = .1245f;
        String d = "!@#$!@#$!@#$";

        writer << a << b << c << d;

        CHECK( !writer.empty() );

        writer.clear();

        CHECK(writer.empty());
        CHECK(writer.size() == 0);
    }
}
