#pragma once
#include "AudioSpec.h"

#include <kaze/internal/audio/lib.h>

KAUDIO_NS_BEGIN

struct AudioTime {
    enum Unit {
        Micros,    ///< Microseconds: milliseconds * .001
        Millis,    ///< Milliseconds: seconds * .001
        PCMFrames, ///< PCM sample frames; time value is qualified by audio spec
        PCMBytes,  ///< Raw PCM bytes; time value is qualified by audio spec.
                   ///< This is distinguishable from raw file data bytes, since PCMBytes pertains to parsed PCM data.
    };

    /// Convert one time unit to another
    /// \param[in] value        quantity of `sourceUnits`
    /// \param[in] sourceUnits  type of source unit
    /// \param[in] targetUnits  type of units to convert to
    /// \param[in] spec         current audio output specs
    /// \returns the value converted as double precision float, allowing user to round or truncate as needed.
    ///          An error should not occur as long as the AudioTime::Unit values are valid enumerations. If it
    ///          does, the `value` will be returned unaltered and a message will be logged to the debug console.
    static auto convert(
        Double value,
        AudioTime::Unit sourceUnits,
        AudioTime::Unit targetUnits,
        const AudioSpec &spec) -> Double;

};

KAUDIO_NS_END
