add_subdirectory(core)

if (KAZE_AUDIO)
    add_subdirectory(audio)
endif()

if (KAZE_TK)
    add_subdirectory(tk)
endif()
