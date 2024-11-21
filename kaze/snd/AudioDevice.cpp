#include "AudioDevice.h"
#include <kaze/core/platform/defines.h>
#include <kaze/core/debug.h>

#if KAZE_PLATFORM_DESKTOP
#include <kaze/snd/backend/portaudio/PortAudioDevice.h>
#elif KAZE_PLATFORM_IOS
#include <kaze/snd/backend/ios/iOSAudioDevice.h>
#elif KAZE_PLATFORM_ANDROID
#include <kaze/snd/backend/android/AAudioDevice.h>
#elif KAZE_PLATFORM_EMSCRIPTEN
#include <kaze/snd/backend/emscripten/EmAudioDevice.h>
#endif

KSND_NS_BEGIN

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

KSND_NS_END
