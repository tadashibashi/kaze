# KAZE_DEBUG mode
string(TOUPPER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE_LOWER)
if (NOT CMAKE_BUILD_TYPELOWER OR CMAKE_BUILD_TYPE_LOWER MATCHES "DEB")
    set(KAZE_DEBUG_DEFAULT 1)
else()
    set(KAZE_DEBUG_DEFAULT 0)
endif()

# Kaze backend
if (IOS OR ANDROID)  # Mobile platforms require SDL
    set(KAZE_BACKEND_DEFAULT "sdl3")
    set(KAZE_BACKEND_SHARED_DEFAULT OFF)
elseif (EXISTS glfw) # Check for prexisting libraries to share
    set(KAZE_BACKEND_DEFAULT "glfw3")
    set(KAZE_BACKEND_SHARED_DEFAULT ON)
elseif (EXISTS SDL3::SDL3)
    set(KAZE_BACKEND_DEFAULT "sdl3")
    set(KAZE_BACKEND_SHARED_DEFAULT ON)
elseif (EXISTS SDL2::SDL2)
    set(KAZE_BACKEND_DEFAULT "sdl2")
    set(KAZE_BACKEND_SHARED_DEFAULT ON)
else()               # Default to at least build for Desktop
    if (EMSCRIPTEN) # the glfw3 emscripten port causes a linkage issue atm
        set(KAZE_BACKEND_DEFAULT "sdl3")
    else()
        set(KAZE_BACKEND_DEFAULT "glfw3")
    endif()
    set(KAZE_BACKEND_SHARED_DEFAULT OFF)
endif()

if (ANDROID)
    set(KAZE_USE_FMT_LIB_DEFAULT ON)
else()
    set(KAZE_USE_FMT_LIB_DEFAULT OFF)
endif()
