if (KAZE_CORE)
    add_subdirectory(core)
endif()

if (KAZE_GRAPHICS)
    if (NOT KAZE_CORE)
        message(FATAL_ERROR "KAZE_CORE must be ON because KAZE_GRAPHICS depends on it")
    endif()

    add_subdirectory(graphics)
endif()

if (KAZE_AUDIO)
    if (NOT KAZE_CORE)
        message(FATAL_ERROR "KAZE_CORE must be ON because KAZE_AUDIO depends on it")
    endif()

    add_subdirectory(audio)
endif()

if (KAZE_CORE AND KAZE_GRAPHICS AND KAZE_AUDIO)
    add_library(kaze INTERFACE)
    target_link_libraries(kaze INTERFACE kaze_core kaze_graphics kaze_audio)
endif()

if (KAZE_TK)
    if (NOT KAZE_CORE OR NOT KAZE_AUDIO OR NOT KAZE_GRAPHICS)
        message(FATAL_ERROR "KAZE_CORE, KAZE_AUDIO, and KAZE_GRAPHICS must be ON
            because KAZE_TK depends on it")
    endif()

    add_subdirectory(tk)
endif()
