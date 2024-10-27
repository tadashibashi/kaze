# ===== KAZE CORE Dependencies ================================================
include(FetchContent)

# ----- Nlohmann Json ---------------------------------------------------------
# License: MIT
# Copyright (c) 2013-2022 - Niels Lohmann
FetchContent_Declare(json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG        v3.11.3
)

set(JSON_BuildTests          OFF CACHE BOOL "Build the unit tests when BUILD_TESTING is enabled.")
set(JSON_ImplicitConversions ON  CACHE BOOL "Enable implicit conversions.")
set(JSON_MultipleHeaders     OFF CACHE BOOL "Use non-amalgamated version of the library.")
set(JSON_SystemInclude       ON  CACHE BOOL "Include as system headers (skip for clang-tidy).")

FetchContent_MakeAvailable(json)

# ----- Gcem ------------------------------------------------------------------
# License: Apache 2.0
# Copyright (c) 2004 - Keith O’Hara
FetchContent_Declare(gcem
    GIT_REPOSITORY https://github.com/kthohr/gcem.git
    GIT_TAG        v1.18.0
)

set(GCEM_BUILD_TESTS OFF CACHE BOOL "Include Gcem test targets")

FetchContent_MakeAvailable(gcem)

# ----- Glm -------------------------------------------------------------------
# License: Happy Bunny License (Modified MIT)
# Copyright (c) 2005 - G-Truc Creation
FetchContent_Declare(glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        1.0.1
)

set(GLM_ENABLE_CXX_20         ON  CACHE BOOL "Enable C++ 20")
set(GLM_ENABLE_FAST_MATH      ON  CACHE BOOL "Enable fast math optimizations")
set(GLM_BUILD_LIBRARY         ON  CACHE BOOL "Build dynamic/static library")
set(GLM_BUILD_TESTS           OFF CACHE BOOL "Build the GLM test programs")
set(GLM_BUILD_INSTALL         OFF CACHE BOOL "Generate the GLM install target")

# Glm Cpu Intrinsic Support
if (KAZE_COMPILER_SUPPORTS_SSE2)
    set(GLM_ENABLE_SIMD_SSE2      ON "Enable SSE2 optimizations")
endif()

if (KAZE_COMPILER_SUPPORTS_SSE3)
    set(GLM_ENABLE_SIMD_SSE3      ON "Enable SSE3 optimizations")
endif()

if (KAZE_COMPILER_SUPPORTS_SSSE3)
    set(GLM_ENABLE_SIMD_SSSE3     ON "Enable SSSE3 optimizations")
endif()

if (KAZE_COMPILER_SUPPORTS_SSE4_1)
    set(GLM_ENABLE_SIMD_SSE4_1    ON "Enable SSE 4.1 optimizations")
endif()

if (KAZE_COMPILER_SUPPORTS_SSE4_2)
    set(GLM_ENABLE_SIMD_SSE4_2    ON "Enable SSE 4.2 optimizations")
endif()

if (KAZE_COMPILER_SUPPORTS_AVX)
    set(GLM_ENABLE_SIMD_AVX       ON "Enable AVX optimizations")
endif()

if (KAZE_COMPILER_SUPPORTS_AVX2)
    set(GLM_ENABLE_SIMD_AVX2      ON "Enable AVX2 optimizations")
endif()

if (KAZE_COMPILER_SUPPORTS_ARM_NEON)
    set(GLM_TEST_ENABLE_SIMD_NEON ON "Enable ARM NEON optimizations")
endif()

FetchContent_MakeAvailable(glm)

# ----- Spdlog ----------------------------------------------------------------
# License: MIT
# Copyright (c) 2016 - Gabi Melman
FetchContent_Declare(spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG        v1.14.1
)

set(SPDLOG_USE_STD_FORMAT         ON CACHE BOOL "Use C++20 std::format")
set(SPDLOG_ENABLE_PCH             ON CACHE BOOL "Build static or shared library using precompiled header to speed up compilation time")
set(SPDLOG_SYSTEM_INCLUDES        ON CACHE BOOL "Include as system headers (skip for clang-tidy).")
set(SPDLOG_NO_EXCEPTIONS          ON CACHE BOOL "Compile with -fno-exceptions. Call abort() on any spdlog exceptions")
set(SPDLOG_DISABLE_DEFAULT_LOGGER ON CACHE BOOL "Disable instantiation of default logger")

FetchContent_MakeAvailable(spdlog)

# ----- Bgfx ------------------------------------------------------------------
# License: BSD 2-Clause "Simplified"
# Copyright (c) 2010-2024 - Branimir Karadzic
FetchContent_Declare(bgfx
    GIT_REPOSITORY https://github.com/bkaradzic/bgfx.cmake
    GIT_TAG        v1.128.8808-482
)
set(BX_AMALGAMATED             ON  CACHE BOOL "")
set(BGFX_AMALGAMATED           ON  CACHE BOOL "")
set(BGFX_BUILD_EXAMPLE_COMMON  OFF CACHE BOOL "")
set(BGFX_BUILD_TESTS           OFF CACHE BOOL "")
set(BGFX_INSTALL               OFF CACHE BOOL "")

FetchContent_MakeAvailable(bgfx)

list(APPEND CMAKE_MODULE_PATH ${bgfx_SOURCE_DIR}/cmake)
include(bgfxToolUtils)
