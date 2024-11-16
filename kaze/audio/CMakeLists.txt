project(kaze_audio)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(kaze_audio STATIC)
target_sources(kaze_audio PRIVATE
        AudioDevice.cpp
        AudioDevice.h
        AudioSpec.h
        AudioTime.cpp
        Audiotime.h
        FadePoint.h
        lib.h
        SampleFormat.cpp
        SampleFormat.h
        SoundBuffer.cpp
        SoundBuffer.h

        engine/AudioCommands.cpp
        engine/AudioCommands.h
        engine/AudioContext.cpp
        engine/AudioContext.h
        engine/AudioEffect.cpp
        engine/AudioEffect.h
        engine/AudioEngine.cpp
        engine/AudioEngine.h
        engine/AudioSource.cpp
        engine/AudioSource.h
        engine/Sound.cpp
        engine/Sound.h
        engine/effects/DelayEffect.cpp
        engine/effects/DelayEffect.h
        engine/effects/PanEffect.cpp
        engine/effects/PanEffect.h
        engine/effects/VolumeEffect.cpp
        engine/effects/VolumeEffect.h

        engine/sources/AudioBus.cpp
        engine/sources/AudioBus.h
        engine/sources/StreamSource.cpp
        engine/sources/StreamSource.h
)

target_link_libraries(kaze_audio PRIVATE kaze_core)

kaze_target_modules(kaze_audio
    backend
    conv
)
