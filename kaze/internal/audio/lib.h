#pragma once
#include <kaze/internal/core/lib.h> // IWYU pragma: export

#define KAUDIO_NS_INTERNAL KAZE_NS_INTERNAL::audio
#define KAUDIO_NS KAZE_NS::audio

#define KAUDIO_NS_BEGIN namespace KAUDIO_NS_INTERNAL {
#define KAUDIO_NS_END }
