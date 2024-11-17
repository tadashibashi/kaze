set (KAZE_MODULE KAZE_BACKEND)

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

    if (KAZE_PLATFORM_EMSCRIPTEN)
        set(SDL_PTHREADS ON CACHE BOOL "" FORCE)
    endif()

    set(SDL3_DISABLE_UNINSTALL ON  CACHE BOOL "")
    set(SDL3_DISABLE_ININSTALL ON  CACHE BOOL "")
    set(SDL_TEST               OFF CACHE BOOL "")

    set(SDL_AUDIO   OFF CACHE BOOL "")
    set(SDL_CAMERA  OFF CACHE BOOL "")
    set(SDL_CPUINFO OFF CACHE BOOL "")
    set(SDL_FILE    OFF CACHE BOOL "")
    set(SDL_FILESYSTEM OFF CACHE BOOL "")
    set(SDL_LOCALE  OFF CACHE BOOL "")
    set(SDL_MISC    OFF CACHE BOOL "")
    set(SDL_RENDER  OFF CACHE BOOL "")

    FetchContent_MakeAvailable(SDL3)
endif()

set(KAZE_BACKEND_LINK_LIBS_PUBLIC SDL3::SDL3)
set(KAZE_BACKEND_SOURCES_PRIVATE
    common_sdl3.cpp
    common_sdl3.h
    window_sdl3.cpp
    window_sdl3.h
    private/GamepadMgr.cpp
    private/GamepadMgr.h
)

if (NOT KAZE_NO_MAIN)
    list(APPEND KAZE_BACKEND_SOURCES_PRIVATE
        main_sdl3.cpp
    )
endif()

if (KAZE_PLATFORM_APPLE_DEVICE)
    list(APPEND KAZE_BACKEND_SOURCES_PRIVATE
        private/uikit_window_sdl3.h
        private/uikit_window_sdl3.mm
        private/uikit_InputHandler.h
        private/uikit_InputHandler.mm
    )
endif()
