set (KAZE_PLUGIN_NAME KAZE_BACKEND)

if (NOT TARGET SDL3::SDL3)
    FetchContent_Declare(SDL3
            GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
            GIT_TAG        6f80d47d64d8b8113df5a64870ff782e8fde45cc
    )
    if (KAZE_BACKEND_SHARED AND NOT EMSCRIPTEN)
        set(SDL_SHARED ON  CACHE BOOL "" FORCE)
        set(SDL_STATIC OFF CACHE BOOL "" FORCE)
    else()
        set(SDL_SHARED OFF  CACHE BOOL "" FORCE)
        set(SDL_STATIC ON   CACHE BOOL "" FORCE)
    endif()
    FetchContent_MakeAvailable(SDL3)
endif()

set(KAZE_BACKEND_LINK_LIBS_PRIVATE SDL3::SDL3)
set(KAZE_BACKEND_SOURCES_PRIVATE
    common_sdl3.cpp
    main_sdl3.cpp
    window_sdl3.cpp
    private/GamepadMgr.cpp
)

if (KAZE_PLATFORM_APPLE_DEVICE)
    list(APPEND KAZE_BACKEND_SOURCES_PRIVATE
        private/uikit_window_sdl3.mm)
endif()
