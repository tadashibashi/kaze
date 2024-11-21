#include "SampleFormat.h"
#include <kaze/core/endian.h>
#include "conv/extern/miniaudio/miniaudio.h"

KSND_NS_BEGIN

/// Get the number of bytes for an ma_format
static Uint maFormatToBytes(const ma_format format)
{
    switch(format)
    {
        case ma_format_f32: case ma_format_s32:
            return 4U;
        case ma_format_s24:
            return 3U;
        case ma_format_s16:
            return 2U;
        case ma_format_u8:
            return 1U;
        default:
            return 0;
    }
}

const SampleFormat SampleFormat::Uint8 = SampleFormat(8u, False, False, False);
const SampleFormat SampleFormat::Int16LE = SampleFormat(16u, False, False, True);
const SampleFormat SampleFormat::Int16BE = SampleFormat(16u, False, True, True);
const SampleFormat SampleFormat::Int24LE = SampleFormat(24u, False, False, True);
const SampleFormat SampleFormat::Int24BE = SampleFormat(24u, False, True, True);
const SampleFormat SampleFormat::Int32LE = SampleFormat(32u, False, False, True);
const SampleFormat SampleFormat::Int32BE = SampleFormat(32u, False, True, True);
const SampleFormat SampleFormat::Float32LE = SampleFormat(32u, True, False, True);
const SampleFormat SampleFormat::Float32BE = SampleFormat(32u, True, True, True);
const SampleFormat SampleFormat::Float64LE = SampleFormat(64u, True, False, True);
const SampleFormat SampleFormat::Float64BE = SampleFormat(64u, True, True, True);

SampleFormat::SampleFormat() : m_flags()
{
}

SampleFormat::SampleFormat(const Uint bits, const Bool isFloat, const Bool isBigEndian, const Bool isSigned) :
    m_flags()
{
    m_flags += bits;
    m_flags |= (isFloat << 8) | (isBigEndian << 12) | (isSigned << 15);
}

auto SampleFormat::fromMaFormat(Int maFormat) -> SampleFormat
{
    return {
        maFormatToBytes(static_cast<ma_format>(maFormat)) * static_cast<Uint>(CHAR_BIT),
        maFormat == ma_format_f32,
        Endian::isBig(),
        maFormat != ma_format_u8
    };
}

auto SampleFormat::toMaFormat() const -> Uint
{
    if (isFloat())
    {
        return ma_format_f32;
    }

    switch(bits())
    {
        case 8:
            return ma_format_u8;
        case 16:
            return ma_format_s16;
        case 24:
            return ma_format_s24;
        case 32:
            return ma_format_s32;
        default:
            return ma_format_unknown;
    }
}

auto SampleFormat::isFloat() const -> Bool
{
    return m_flags >> 8 & 1;
}

auto SampleFormat::isBigEndian() const -> bool
{
    return m_flags >> 12 & 1;
}

auto SampleFormat::isSigned() const -> bool
{
    return m_flags >> 15 & 1;
}

auto SampleFormat::bits() const -> Uint
{
    return m_flags & 0xFFu;
}

auto SampleFormat::bytes() const -> Uint
{
    return bits() / CHAR_BIT;
}

KSND_NS_END
