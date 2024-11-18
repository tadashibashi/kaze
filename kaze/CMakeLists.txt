if (KAZE_CORE)
    add_subdirectory(core)
endif()

if (KAZE_GRAPHICS)
    add_subdirectory(graphics)
endif()

if (KAZE_AUDIO)
    add_subdirectory(audio)
endif()

if (KAZE_TK)
    add_subdirectory(tk)
endif()
