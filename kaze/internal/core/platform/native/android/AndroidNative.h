#pragma once
#include <kaze/internal/core/lib.h>
#include <kaze/internal/core/platform/defines.h>

#if KAZE_PLATFORM_ANDROID
class AAsset;

KAZE_NS_BEGIN
namespace android {
    auto openAsset(const char *filename) -> AAsset *; ///< Intended for single read into a buffer
    auto openAssetStream(const char *filename) -> AAsset *; ///< Intended for streaming multiple reads
    auto closeAsset(AAsset *asset) -> void;
    auto getDefaultSampleRate() -> int;
    auto getDefaultFramesPerBuffer() -> int;
    auto getDataDirectory() -> const String &;
}
KAZE_NS_END

#endif
