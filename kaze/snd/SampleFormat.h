#pragma once
#include <kaze/snd/lib.h>

KSND_NS_BEGIN

/// Description of sample data type (e.g. Int16, Float32, etc.)
/// It is an implementation of SDL audio format and used as intermediary object for cross-compatibility
class SampleFormat {
public:
    static const SampleFormat Uint8;
    static const SampleFormat Int16LE;
    static const SampleFormat Int16BE;
    static const SampleFormat Int24LE;
    static const SampleFormat Int24BE;
    static const SampleFormat Int32LE;
    static const SampleFormat Int32BE;
    static const SampleFormat Float32LE;
    static const SampleFormat Float32BE;
    static const SampleFormat Float64LE;
    static const SampleFormat Float64BE;

    /// Create a SampleFormat from an `ma_format` (miniaudio type)
    /// \param[in]  maFormat  the format enum
    /// \returns a SampleFormat of the corresponding type
    [[nodiscard]]
    static SampleFormat fromMaFormat(Int maFormat);

    /// Convert SampleFormat to MaFormat
    [[nodiscard]]
    auto toMaFormat() const -> Uint;

    /// Zero-ed null sample format object
    SampleFormat();
    SampleFormat(Uint bits, Bool isFloat, Bool isBigEndian, Bool isSigned);

    /// \returns whether format is floating point (on true), or integer data (on false)
    [[nodiscard]]
    auto isFloat() const -> Bool;

    /// \returns whether format is big endian (on true), or little endian (on false)
    [[nodiscard]]
    auto isBigEndian() const -> bool;

    /// \returns whether format is signed (on true), or unsigned (on false)
    [[nodiscard]]
    auto isSigned() const -> bool;

    /// \returns bits per sample
    [[nodiscard]]
    auto bits() const -> Uint;

    /// \returns bytes per sample
    [[nodiscard]]
    auto bytes() const -> Uint;

    /// \returns the raw flags
    [[nodiscard]]
    auto flags() const -> Uint16 { return m_flags; }

    [[nodiscard]]
    auto operator== (const SampleFormat &other) const -> Bool { return m_flags == other.m_flags; }

    [[nodiscard]]
    auto operator!= (const SampleFormat &other) const -> Bool { return !operator==(other); }
private:
    Uint16 m_flags;
};

KSND_NS_END
