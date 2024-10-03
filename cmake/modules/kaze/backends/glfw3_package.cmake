if (EMSCRIPTEN)
    set(KAZE_BACKEND_LINK_FLAGS --use-port=contrib.glfw3)
    set(KAZE_BACKEND_COMPILE_FLAGS --use-port=contrib.glfw3)
    set(KAZE_BACKEND_LIBRARIES "")
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
        FetchContent_MakeAvailable(glfw3)

    endif()

    set(KAZE_BACKEND_LIBRARIES glfw)
endif()

set(GLFW3_BACKEND_ROOT ${KAZE_ROOT}/kaze/platform/backends/glfw3)
set(KAZE_BACKEND_SOURCES
    ${GLFW3_BACKEND_ROOT}/PlatformBackend_glfw3.cpp
    ${GLFW3_BACKEND_ROOT}/PlatformBackend_globals.cpp
    ${GLFW3_BACKEND_ROOT}/Window_glfw3.cpp
)

if (APPLE)
    list(APPEND KAZE_BACKEND_SOURCES
        ${GLFW3_BACKEND_ROOT}/Window_glfw3.mm)
endif()
