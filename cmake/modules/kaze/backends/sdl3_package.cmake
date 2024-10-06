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

set(KAZE_BACKEND_LIBRARIES SDL3::SDL3)

set(SDL3_BACKEND_ROOT ${KAZE_BACKEND_ROOT}/sdl3)

set(KAZE_BACKEND_SOURCES
    ${SDL3_BACKEND_ROOT}/common_sdl3.cpp
    ${SDL3_BACKEND_ROOT}/GamepadMgr.cpp
    ${SDL3_BACKEND_ROOT}/window_sdl3.cpp
)
set(KAZE_BACKEND_LINK_FLAGS "")
set(KAZE_BACKEND_COMPILE_FLAGS "")
