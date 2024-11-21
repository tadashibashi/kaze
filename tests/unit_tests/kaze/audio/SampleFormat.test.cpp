#include <doctest/doctest.h>

#include <kaze/internal/audio/SampleFormat.h>
#include <kaze/internal/audio/conv/extern/miniaudio/miniaudio.h>

#include <kaze/core/endian.h>

using namespace KAZE_NS;
using namespace KAUDIO_NS_INTERNAL;

TEST_SUITE("io/audio/SampleFormat")
{
    TEST_CASE("Default constructor")
    {
        SampleFormat format;
        CHECK(format.flags() == 0);
    }

    TEST_CASE("Regular constructor")
    {
        SUBCASE("64-bit float LE")
        {
            SampleFormat format(64u, True, False, True);
            CHECK(format.bits() == 64u);
            CHECK(format.bytes() == 64u / CHAR_BIT);
            CHECK(format.isBigEndian() == False);
            CHECK(format.isFloat() == True);
            CHECK(format.isSigned() == True);

            CHECK(format == SampleFormat::Float64LE);
        }

        SUBCASE("64-bit float BE")
        {
            SampleFormat format(64u, True, True, True);
            CHECK(format.bits() == 64u);
            CHECK(format.bytes() == 64u / CHAR_BIT);
            CHECK(format.isBigEndian() == True);
            CHECK(format.isFloat() == True);
            CHECK(format.isSigned() == True);

            CHECK(format == SampleFormat::Float64BE);
        }

        SUBCASE("32-bit float LE")
        {
            SampleFormat format(32u, True, False, True);
            CHECK(format.bits() == 32u);
            CHECK(format.bytes() == 32u / CHAR_BIT);
            CHECK(format.isBigEndian() == False);
            CHECK(format.isFloat() == True);
            CHECK(format.isSigned() == True);

            CHECK(format == SampleFormat::Float32LE);
        }

        SUBCASE("32-bit float BE")
        {
            SampleFormat format(32u, True, True, True);
            CHECK(format.bits() == 32u);
            CHECK(format.bytes() == 32u / CHAR_BIT);
            CHECK(format.isBigEndian() == True);
            CHECK(format.isFloat() == True);
            CHECK(format.isSigned() == True);

            CHECK(format == SampleFormat::Float32BE);
        }

        SUBCASE("24-bit signed LE")
        {
            SampleFormat format(24u, False, False, True);
            CHECK(format.bits() == 24u);
            CHECK(format.bytes() == 24u / CHAR_BIT);
            CHECK(format.isBigEndian() == False);
            CHECK(format.isFloat() == False);
            CHECK(format.isSigned() == True);

            CHECK(format == SampleFormat::Int24LE);
        }

        SUBCASE("24-bit signed BE")
        {
            SampleFormat format(24u, False, True, True);
            CHECK(format.bits() == 24u);
            CHECK(format.bytes() == 24u / CHAR_BIT);
            CHECK(format.isBigEndian() == True);
            CHECK(format.isFloat() == False);
            CHECK(format.isSigned() == True);

            CHECK(format == SampleFormat::Int24BE);
        }

        SUBCASE("16-bit signed LE")
        {
            SampleFormat format(16u, False, False, True);
            CHECK(format.bits() == 16u);
            CHECK(format.bytes() == 16u / CHAR_BIT);
            CHECK(format.isBigEndian() == False);
            CHECK(format.isFloat() == False);
            CHECK(format.isSigned() == True);

            CHECK(format == SampleFormat::Int16LE);
        }

        SUBCASE("16-bit signed BE")
        {
            SampleFormat format(16u, False, True, True);
            CHECK(format.bits() == 16u);
            CHECK(format.bytes() == 16u / CHAR_BIT);
            CHECK(format.isBigEndian() == True);
            CHECK(format.isFloat() == False);
            CHECK(format.isSigned() == True);

            CHECK(format == SampleFormat::Int16BE);
        }

        SUBCASE("8-bit unsigned")
        {
            SampleFormat format(8u, False, False, False);
            CHECK(format.bits() == 8u);
            CHECK(format.bytes() == 8u / CHAR_BIT);
            CHECK(format.isBigEndian() == False); // it's not either big or little endian, so this should always be False
            CHECK(format.isFloat() == False);
            CHECK(format.isSigned() == False);

            CHECK(format == SampleFormat::Uint8);
        }
    }

    TEST_CASE("create from ma_format")
    {
        CHECK(SampleFormat::fromMaFormat(ma_format_f32) == (endian::isBig() ? SampleFormat::Float32BE : SampleFormat::Float32LE));
        CHECK(SampleFormat::fromMaFormat(ma_format_u8) ==  SampleFormat::Uint8);
        CHECK(SampleFormat::fromMaFormat(ma_format_s16) == (endian::isBig() ? SampleFormat::Int16BE : SampleFormat::Int16LE));
        CHECK(SampleFormat::fromMaFormat(ma_format_s24) == (endian::isBig() ? SampleFormat::Int24BE : SampleFormat::Int24LE));
        CHECK(SampleFormat::fromMaFormat(ma_format_s32) == (endian::isBig() ? SampleFormat::Int32BE : SampleFormat::Int32LE));
    }

    TEST_CASE("convert to ma_format")
    {
        CHECK(SampleFormat::Float32LE.toMaFormat() == ma_format_f32);
        CHECK(SampleFormat::Float32BE.toMaFormat() == ma_format_f32);
        CHECK(SampleFormat::Int32LE.toMaFormat() == ma_format_s32);
        CHECK(SampleFormat::Int32BE.toMaFormat() == ma_format_s32);
        CHECK(SampleFormat::Int24LE.toMaFormat() == ma_format_s24);
        CHECK(SampleFormat::Int24BE.toMaFormat() == ma_format_s24);
        CHECK(SampleFormat::Int16LE.toMaFormat() == ma_format_s16);
        CHECK(SampleFormat::Int16BE.toMaFormat() == ma_format_s16);
        CHECK(SampleFormat::Uint8.toMaFormat() == ma_format_u8);
    }
}
