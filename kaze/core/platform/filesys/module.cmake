set (KAZE_MODULE KAZE_FILESYS)

if (KAZE_PLATFORM_MACOS OR KAZE_PLATFORM_IOS)
    set(KAZE_FILESYS_SOURCES_PRIVATE filesys_macos.mm)
elseif(KAZE_PLATFORM_WINDOWS)
    set(KAZE_FILESYS_SOURCES_PRIVATE filesys_windows.cpp)
elseif(KAZE_PLATFORM_LINUX)
    set(KAZE_FILESYS_SOURCES_PRIVATE filesys_linux.cpp)
elseif(KAZE_PLATFORM_ANDROID)
    set(KAZE_FILESYS_SOURCES_PRIVATE filesys_android.cpp)
elseif(KAZE_PLATFORM_EMSCRIPTEN)
    set(KAZE_FILESYS_SOURCES_PRIVATE filesys_emscripten.cpp)
endif()