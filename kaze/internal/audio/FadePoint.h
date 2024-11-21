#pragma once

#include <kaze/internal/audio/lib.h>

KAUDIO_NS_BEGIN

struct FadePoint {
    FadePoint() : clock(), value() { }

    /// \param[in]  clock  pcm frame position
    /// \param[in]  value  fade value
    FadePoint(Uint clock, Float value) : clock(clock), value(value) { }

    Uint64 clock;   ///< pcm frame position
    Float value;  ///< value of the fade
};

KAUDIO_NS_END
