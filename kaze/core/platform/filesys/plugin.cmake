set (KAZE_PLUGIN_NAME KAZE_FILESYS)

if (KAZE_PLATFORM_MACOS)
    set(KAZE_FILESYS_SOURCES_PRIVATE filesys_macos.mm)
elseif(KAZE_PLATFORM_WINDOWS)
    set(KAZE_FILESYS_SOURCES_PRIVATE filesys_windows.cpp)
elseif(KAZE_PLATFORM_EMSCRIPTEN)
    set(KAZE_FILESYS_SOURCES_PRIVATE filesys_emscripten.cpp)
endif()
