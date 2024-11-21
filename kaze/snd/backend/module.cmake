set(KAZE_MODULE KSND_BACKEND)

if (KAZE_PLATFORM_IOS) # ===== iOS ============================================

    list(APPEND KSND_BACKEND_SOURCES_PRIVATE
        ios/iOSAudioDevice.h
        ios/iOSAudioDevice.mm
    )

    list(APPEND KSND_BACKEND_LINK_LIBS_PRIVATE
        "-framework AudioToolbox" "-framework AVFoundation"
    )

elseif(KAZE_PLATFORM_ANDROID) # ===== Android =================================

    list(APPEND KSND_BACKEND_SOURCES_PRIVATE
        android/AAudioDevice.cpp
        android/AAudioDevice.h
    )

    list(APPEND KSND_BACKEND_LINK_LIBS_PRIVATE aaudio)

elseif(KAZE_PLATFORM_EMSCRIPTEN) # ===== Emscripten ===========================

    list(APPEND KSND_BACKEND_SOURCES_PRIVATE
        emscripten/EmAudioDevice.cpp
        emscripten/EmAudioDevice.h
    )

    # Link opts are set under kaze/core/platform/native/emscripten/module.cmake

elseif(KAZE_PLATFORM_DESKTOP) # ===== Desktop =================================

    FetchContent_Declare(PortAudio
        GIT_REPOSITORY https://github.com/PortAudio/portaudio
        GIT_TAG        57aa393109ec996799d3a5846c9ecb0a65b64644
    )
    FetchContent_MakeAvailable(PortAudio)

    list(APPEND KSND_BACKEND_LINK_LIBS_PRIVATE PortAudio)

    list(APPEND KSND_BACKEND_COMPILE_DEFS_PRIVATE
        KSND_BACKEND_BACKEND_PORTAUDIO=1
    )

    list(APPEND KSND_BACKEND_SOURCES_PRIVATE
        portaudio/PortAudioDevice.cpp
        portaudio/PortAudioDevice.h
    )

endif()
