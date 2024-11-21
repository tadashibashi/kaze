#include "AudioDevice.h"
#include <kaze/internal/core/platform/defines.h>
#include <kaze/internal/core/debug.h>

#if KAZE_PLATFORM_DESKTOP
#include <kaze/internal/audio/backend/portaudio/PortAudioDevice.h>
#elif KAZE_PLATFORM_IOS
#include <kaze/internal/audio/backend/ios/iOSAudioDevice.h>
#elif KAZE_PLATFORM_ANDROID
#include <kaze/internal/audio/backend/android/AAudioDevice.h>
#elif KAZE_PLATFORM_EMSCRIPTEN
#include <kaze/internal/audio/backend/emscripten/EmAudioDevice.h>
#endif

KAUDIO_NS_BEGIN

auto AudioDevice::create() -> AudioDevice *
{
#if KAZE_PLATFORM_DESKTOP
    return new PortAudioDevice();
#elif KAZE_PLATFORM_IOS
    return new iOSAudioDevice();
#elif KAZE_PLATFORM_ANDROID
    return new AAudioDevice();
#elif KAZE_PLATFORM_EMSCRIPTEN
    return new EmAudioDevice();
#else
    KAZE_PUSH_ERR(Error::NotImplemented, "AudioDevice platform is not implemented");
    return nullptr;
#endif
}

KAUDIO_NS_END
