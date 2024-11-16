# GLFW3 Backend Plugin
set (KAZE_MODULE KAZE_BACKEND)

if (EMSCRIPTEN)
    set(KAZE_BACKEND_LINK_OPTS_PUBLIC --use-port=contrib.glfw3)
    set(KAZE_BACKEND_COMPILE_OPTS_PUBLIC --use-port=contrib.glfw3)
else()
    if (NOT TARGET glfw) # pull in dependency if target is not available
        FetchContent_Declare(glfw3
                GIT_REPOSITORY https://github.com/glfw/glfw.git
                GIT_TAG        3.4
        )

        if (KAZE_BACKEND_SHARED)
            set(BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)
        else()
            set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
        endif()

        set(GLFW_BUILD_WAYLAND ${KAZE_USE_WAYLAND} CACHE BOOL "" FORCE)
        FetchContent_MakeAvailable(glfw3)

    endif()

    set(KAZE_BACKEND_LINK_LIBS_PUBLIC glfw)
endif()

set(KAZE_BACKEND_SOURCES_PRIVATE
    common_glfw3.cpp
    window_glfw3.cpp
)

if (NOT KAZE_NO_MAIN)
    list(APPEND KAZE_BACKEND_SOURCES_PRIVATE
        main_glfw.cpp
    )
endif()

if (APPLE)
    list(APPEND KAZE_BACKEND_SOURCES_PRIVATE
        window_glfw3.mm)
endif()
