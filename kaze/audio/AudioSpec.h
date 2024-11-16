#pragma once
#include "SampleFormat.h"

#include <kaze/audio/lib.h>

KAUDIO_NS_BEGIN

/// Description for data in a buffer or stream of audio
struct AudioSpec {
    AudioSpec() : freq(), channels(), format() { }
    AudioSpec(Int freq, Int channels, SampleFormat format) :
        freq(freq), channels(channels), format(format) { }

    Int freq;             ///< number of sample frames per second
    Int channels;         ///< number of interleaved channels per frame
    SampleFormat format;  ///< sample type info (matches SDL2/3's audio format flags)

    /// \returns the bytes in one sample in one channel
    [[nodiscard]]
    auto bytesPerChannel() const -> Uint { return format.bytes(); }

    /// \returns bytes per sample frame (one sample slice with every audio channel combined)
    [[nodiscard]]
    auto bytesPerFrame() const -> Uint { return format.bytes() * channels; }

    /// \returns bytes to process per second of audio
    [[nodiscard]]
    auto bytesPerSecond() const -> Uint { return format.bytes() * channels * freq; }

    [[nodiscard]]
    auto operator== (const AudioSpec &other) const -> Bool
    {
        return format == other.format && freq == other.freq && channels == other.channels;
    }

    [[nodiscard]]
    auto operator!= (const AudioSpec &other) const -> Bool { return !operator==(other); }
};

KAUDIO_NS_END
