if (KAZE_CORE)
    add_subdirectory(core)
endif()

if (KAZE_GFX)
    add_subdirectory(gfx)
endif()

if (KAZE_SND)
    add_subdirectory(snd)
endif()

if (KAZE_TK)
    add_subdirectory(tk)
endif()
