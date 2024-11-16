#pragma once

#include <kaze/audio/lib.h>

KAUDIO_NS_BEGIN

struct AudioMarker {
    AudioMarker(): label(), position() { }
    AudioMarker(String label, const Uint64 samplePosition) :
        label(std::move(label)), position(samplePosition) { }

    String label;    ///< marker name
    Uint64 position; ///< position in samples, check sample rate retrieved from spec for conversion to seconds, etc.
};

KAUDIO_NS_END
