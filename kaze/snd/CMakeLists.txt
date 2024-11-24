project(kaze_snd)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(kaze_snd STATIC)
add_library(kaze::snd ALIAS kaze_snd)

target_sources(kaze_snd PRIVATE
        AudioCommands.cpp
        AudioCommands.h
        AudioContext.cpp
        AudioContext.h
        AudioDevice.cpp
        AudioDevice.h
        AudioEffect.cpp
        AudioEffect.h
        AudioEngine.cpp
        AudioEngine.h
        AudioSource.cpp
        AudioSource.h
        AudioSpec.h
        AudioTime.cpp
        AudioTime.h
        FadePoint.h
        lib.h
        SampleFormat.cpp
        SampleFormat.h
        Sound.cpp
        Sound.h
        SoundBuffer.cpp
        SoundBuffer.h

        effects/DelayEffect.cpp
        effects/DelayEffect.h
        effects/PanEffect.cpp
        effects/PanEffect.h
        effects/VolumeEffect.cpp
        effects/VolumeEffect.h

        sources/AudioBus.cpp
        sources/AudioBus.h
        sources/StreamSource.cpp
        sources/StreamSource.h
)

target_link_libraries(kaze_snd PRIVATE kaze_core)

kaze_target_modules(kaze_snd
    backend
    conv
)
