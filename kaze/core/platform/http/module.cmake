set (KAZE_MODULE KAZE_HTTP)

if (KAZE_PLATFORM_MACOS OR KAZE_PLATFORM_IOS)
    set(KAZE_HTTP_SOURCES_PRIVATE http_apple.mm)
elseif(KAZE_PLATFORM_WINDOWS)
    set(KAZE_HTTP_LINK_LIBS_PRIVATE winhttp.lib)
    set(KAZE_HTTP_SOURCES_PRIVATE http_windows.cpp)
elseif(KAZE_PLATFORM_LINUX)
    find_package(CURL REQUIRED)
    set(KAZE_HTTP_LINK_LIBS_PRIVATE CURL::libcurl ssl)
    set(KAZE_HTTP_SOURCES_PRIVATE http_curl.cpp)
elseif(KAZE_PLATFORM_ANDROID)
    set(KAZE_HTTP_SOURCES_PRIVATE http_android.cpp)
elseif(KAZE_PLATFORM_EMSCRIPTEN)
    set(KAZE_HTTP_SOURCES_PRIVATE http_emscripten.cpp)
endif()
