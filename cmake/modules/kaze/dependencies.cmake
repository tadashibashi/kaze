include(FetchContent)

FetchContent_Declare(json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG        v3.11.3
)

FetchContent_Declare(gcem
    GIT_REPOSITORY https://github.com/kthohr/gcem.git
    GIT_TAG        v1.18.0
)

FetchContent_Declare(glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        1.0.1
)

FetchContent_Declare(spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG        v1.14.1
)

FetchContent_Declare(bgfx
    GIT_REPOSITORY https://github.com/bkaradzic/bgfx.cmake
    GIT_TAG        v1.128.8808-482
)

FetchContent_MakeAvailable(json gcem glm spdlog bgfx)

list(APPEND CMAKE_MODULE_PATH ${bgfx_SOURCE_DIR}/cmake)
include(bgfxToolUtils)

# ===== Backend ===============================================================
# Backends are fetched from a cmake module and the following variables are available to set:
# KAZE_BACKEND_LINK_FLAGS:      C++ linker flags (passed to Kaze's target_link_options)
# KAZE_BACKEND_COMPILE_FLAGS:   C++ source compilation flags (passed to Kaze's target_compile_options)
# KAZE_BACKEND_LIBRARIES:       C++ link library targets (passed to Kaze's target_link_libraries)
# KAZE_BACKEND_SOURCES:         C++ source files (added to Kaze's target_sources)
# Please check one of the included files below for an example of how to set one up
if (KAZE_BACKEND STREQUAL "sdl2")      # ----- SDL2 -----
    message(FATAL_ERROR "Backend SDL2 is not supported by Kaze yet!")
    include(kaze/backends/sdl2_package)
elseif (KAZE_BACKEND STREQUAL "sdl3")  # ----- SDL3 -----
    include(kaze/backends/sdl3_package)
elseif (KAZE_BACKEND STREQUAL "glfw3") # ----- GLFW -----
    include(kaze/backends/glfw3_package)
else()
    message(FATAL_ERROR "Backend ${KAZE_BACKEND} is not supported by Kaze")
endif()

message("Kaze Backend: ${KAZE_BACKEND}")



