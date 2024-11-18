add_subdirectory(core)
add_subdirectory(graphics)

if (KAZE_AUDIO)
    add_subdirectory(audio)
endif()

if (KAZE_TK)
    add_subdirectory(tk)
endif()
