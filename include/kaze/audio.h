#pragma once
#include <kaze/core.h>
#include <kaze/internal/audio/AudioDevice.h>
#include <kaze/internal/audio/AudioMarker.h>
#include <kaze/internal/audio/AudioSpec.h>
#include <kaze/internal/audio/AudioTime.h>
#include <kaze/internal/audio/FadePoint.h>
#include <kaze/internal/audio/SampleFormat.h>
#include <kaze/internal/audio/engine/AudioEffect.h>
#include <kaze/internal/audio/engine/AudioEngine.h>
#include <kaze/internal/audio/engine/Sound.h>

KAZE_PUBLIC_NS_BEGIN

using KAUDIO_NS_INTERNAL::AudioDevice;
using KAUDIO_NS_INTERNAL::AudioDeviceOpen;

using KAUDIO_NS_INTERNAL::AudioMarker;
using KAUDIO_NS_INTERNAL::AudioSpec;
using KAUDIO_NS_INTERNAL::AudioTime;
using KAUDIO_NS_INTERNAL::FadePoint;
using KAUDIO_NS_INTERNAL::SampleFormat;

using KAUDIO_NS_INTERNAL::AudioEffect;
using KAUDIO_NS_INTERNAL::AudioEngine;

using KAUDIO_NS_INTERNAL::Sound;

KAZE_PUBLIC_NS_END
