#include "AudioTime.h"

KAUDIO_NS_BEGIN

auto AudioTime::convert(
    Double value,
    AudioTime::Unit sourceUnits,
    AudioTime::Unit targetUnits,
    const AudioSpec &spec) -> Double
{
    if (sourceUnits == targetUnits)
        return value;

    switch(sourceUnits)
    {
    case AudioTime::Micros: // microseconds to ================================
        {
            switch(targetUnits)
            {
            case AudioTime::Millis:
                return value / 1000.0;
            case AudioTime::PCMFrames:
                return spec.freq * (value / 1000000.0);
            case AudioTime::PCMBytes:
                return spec.bytesPerSecond() * (value / 1000000.0);
            default:
                KAZE_PUSH_ERR(Error::InvalidEnum,
                    "Invalid target `AudioTime::Unit`");
                return value;
            }
        } break;

    case AudioTime::Millis: // milliseconds to ================================
        {
            switch(targetUnits)
            {
            case AudioTime::Micros:
                return value * 1000.0;
            case AudioTime::PCMFrames:
                return spec.freq * (value / 1000.0);
            case AudioTime::PCMBytes:
                return spec.bytesPerSecond() * (value / 1000.0);
            default:
                KAZE_PUSH_ERR(Error::InvalidEnum,
                    "Invalid target `AudioTime::Unit`");
                return value;
            }
        } break;

    case AudioTime::PCMFrames: // pcm frames to ===============================
        {
            switch(targetUnits)
            {
            case AudioTime::Micros:
                return value / spec.freq * 1000000.0;
            case AudioTime::Millis:
                return value / spec.freq * 1000.0;
            case AudioTime::PCMBytes:
                return value * spec.bytesPerFrame();
            default:
                KAZE_PUSH_ERR(Error::InvalidEnum,
                    "Invalid target `AudioTime::Unit`");
                return value;
            }
        } break;

    case AudioTime::PCMBytes: // raw pcm bytes to =============================
        {
            switch(targetUnits)
            {
            case AudioTime::Micros:
                return value / spec.bytesPerFrame() * 1000000.0;
            case AudioTime::Millis:
                return value / spec.bytesPerFrame() * 1000.0;
            case AudioTime::PCMFrames:
                return value / spec.bytesPerFrame();
            default:
                KAZE_PUSH_ERR(Error::InvalidEnum,
                    "Invalid target `AudioTime::Unit`");
                return value;
            }
        } break;

    default: // In case of a bad enum cast
        KAZE_PUSH_ERR(Error::InvalidEnum,
            "Invalid source `AudioTime::Unit`");
        return value;
    }
}

KAUDIO_NS_END
