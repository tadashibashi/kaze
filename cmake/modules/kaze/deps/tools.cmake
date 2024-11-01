# ===== KAZE CORE Dependencies ================================================
include(FetchContent)

# ----- Nlohmann Json ---------------------------------------------------------
# License: MIT
# Copyright (c) 2013-2022 - Niels Lohmann
if (NOT TARGET nlohmann_json)
    FetchContent_Declare(json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG        v3.11.3
    )

    set(JSON_BuildTests          OFF CACHE BOOL "Build the unit tests when BUILD_TESTING is enabled.")
    set(JSON_ImplicitConversions ON  CACHE BOOL "Enable implicit conversions.")
    set(JSON_MultipleHeaders     OFF CACHE BOOL "Use non-amalgamated version of the library.")
    set(JSON_SystemInclude       ON  CACHE BOOL "Include as system headers (skip for clang-tidy).")

    FetchContent_MakeAvailable(json)
endif()

# ----- Bgfx ------------------------------------------------------------------
# License: BSD 2-Clause "Simplified"
# Copyright (c) 2010-2024 - Branimir Karadzic
if (NOT TARGET bgfx)
    FetchContent_Declare(bgfx-cmake
        GIT_REPOSITORY https://github.com/bkaradzic/bgfx.cmake
        GIT_TAG        v1.128.8808-482
    )
    set(BX_AMALGAMATED             OFF CACHE BOOL "")
    set(BGFX_AMALGAMATED           OFF CACHE BOOL "")
    set(BGFX_BUILD_EXAMPLES        OFF CACHE BOOL "")
    set(BGFX_BUILD_TESTS           OFF CACHE BOOL "")
    set(BGFX_INSTALL               OFF CACHE BOOL "")

    if (EMSCRIPTEN)
        set(BGFX_CONFIG_MULTITHREADED  OFF CACHE BOOL "" FORCE)
    endif()

    FetchContent_MakeAvailable(bgfx-cmake)
endif()
