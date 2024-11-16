#include "AudioDevice.h"
#include <kaze/core/platform/defines.h>
#include <kaze/core/debug.h>

#if KAZE_PLATFORM_DESKTOP
#include <kaze/audio/backend/portaudio/PortAudioDevice.h>
#elif KAZE_PLATFORM_IOS

#elif KAZE_PLATFORM_ANDROID

#elif KAZE_PLATFORM_EMSCRIPTEN

#endif

KAUDIO_NS_BEGIN

auto AudioDevice::create() -> AudioDevice *
{
#if KAZE_PLATFORM_DESKTOP
    return new PortAudioDevice();
#elif KAZE_PLATFORM_IOS
    KAZE_PUSH_ERR(Error::NotImplemented, "AudioDevice platform is not implemented");
    return nullptr;
#elif KAZE_PLATFORM_ANDROID
    KAZE_PUSH_ERR(Error::NotImplemented, "AudioDevice platform is not implemented");
    return nullptr;
#elif KAZE_PLATFORM_EMSCRIPTEN
    KAZE_PUSH_ERR(Error::NotImplemented, "AudioDevice platform is not implemented");
    return nullptr;
#else
    KAZE_PUSH_ERR(Error::NotImplemented, "AudioDevice platform is not implemented");
    return nullptr;
#endif
}

KAUDIO_NS_END
