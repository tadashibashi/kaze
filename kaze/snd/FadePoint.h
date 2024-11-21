#pragma once

#include <kaze/snd/lib.h>

KSND_NS_BEGIN

struct FadePoint {
    FadePoint() : clock(), value() { }

    /// \param[in]  clock  pcm frame position
    /// \param[in]  value  fade value
    FadePoint(Uint clock, Float value) : clock(clock), value(value) { }

    Uint64 clock;   ///< pcm frame position
    Float value;  ///< value of the fade
};

KSND_NS_END
