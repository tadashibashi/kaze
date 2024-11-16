set(KAZE_MODULE KAUDIO_BACKEND)

if (KAZE_PLATFORM_IOS) # ===== iOS ============================================

    list(APPEND KAUDIO_BACKEND_SOURCES_PRIVATE
        # TODO: create and add iOSAudioDevice sources here
    )

    list(APPEND KAUDIO_BACKEND_LINK_LIBS_PRIVATE
        "-framework AudioToolbox" "-framework AVFoundation"
    )

elseif(KAZE_PLATFORM_ANDROID) # ===== Android =================================

    list(APPEND KAUDIO_BACKEND_SOURCES_PRIVATE
        # TODO: create and add EmAudioDevice sources here
    )
    list(APPEND KAUDIO_BACKEND_LINK_LIBS_PRIVATE aaudio)

elseif(KAZE_PLATFORM_EMSCRIPTEN) # ===== Emscripten ===========================

    list(APPEND KAUDIO_BACKEND_SOURCES_PRIVATE
        # TODO: create and add EmAudioDevice sources here
    )

    # Link opts are set under kaze/core/platform/native/emscripten/module.cmake


elseif(KAZE_PLATFORM_DESKTOP) # ===== Desktop =================================

    FetchContent_Declare(PortAudio
        GIT_REPOSITORY https://github.com/PortAudio/portaudio
        GIT_TAG        57aa393109ec996799d3a5846c9ecb0a65b64644
    )
    FetchContent_MakeAvailable(PortAudio)

    list(APPEND KAUDIO_BACKEND_LINK_LIBS_PRIVATE PortAudio)

    list(APPEND KAUDIO_BACKEND_COMPILE_DEFS_PRIVATE
        KAUDIO_BACKEND_BACKEND_PORTAUDIO=1
    )

    list(APPEND KAUDIO_BACKEND_SOURCES_PRIVATE
        portaudio/PortAudioDevice.cpp
        portaudio/PortAudioDevice.h
    )

endif()
