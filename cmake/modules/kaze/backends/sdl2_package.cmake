# TODO: SDL2 not supported yet!
#elseif (KAZE_BACKEND STREQUAL "sdl2") # ----- SDL2 -----
#
#    if (NOT EXISTS SDL2::SDL2)
#        FetchContent_Declare(SDL2
#            GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
#            GIT_TAG        6f80d47d64d8b8113df5a64870ff782e8fde45cc
#        )
#        if (KAZE_BACKEND_SHARED)
#            set(SDL_SHARED ON  CACHE BOOL "" FORCE)
#            set(SDL_STATIC OFF CACHE BOOL "" FORCE)
#        else()
#            set(SDL_SHARED OFF  CACHE BOOL "" FORCE)
#            set(SDL_STATIC ON   CACHE BOOL "" FORCE)
#        endif()
#        FetchContent_MakeAvailable(SDL2)
#    endif()
#
#    set(KAZE_BACKEND_SOURCES ${KAZE_ROOT}/kaze/platform/backends/PlatformBackend_sdl2.cpp)
#    set(KAZE_BACKEND_LIBRARIES SDL2::SDL2main)
