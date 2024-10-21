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
# Validate backend. The name must correspond to the name of the folder at: kaze/platform/backend/
# It must contain a file plugin.cmake, and contain the following defines that are described in the cmake function
# kaze_target_plugin, located in cmake/modules/kaze/utils.cmake
if (NOT EXISTS "${KAZE_BACKEND_ROOT}/${KAZE_BACKEND}")
    message(FATAL_ERROR "Backend ${KAZE_BACKEND} is not supported by Kaze")
endif()
message("Kaze Backend: ${KAZE_BACKEND}")
