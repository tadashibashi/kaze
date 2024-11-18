cmake_minimum_required(VERSION 3.20)
project(kaze_engine LANGUAGES CXX)

# ===== C++ Standard =======================================================
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF) # No C++20 Modules in this project

# ===== Directory Variables ===================================================
set(KAZE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}" CACHE STRING "Path to the entire project's root" FORCE)
set(KAZE_SOURCE_ROOT "${KAZE_ROOT}/kaze"    CACHE STRING "Path to the Kaze engine source code directory" FORCE)
set(KAZE_BACKEND_ROOT "${KAZE_SOURCE_ROOT}/core/platform/backend" CACHE STRING "Path to the Kaze engine backends" FORCE)
set(KAZE_TESTS_ROOT "${KAZE_ROOT}/tests")

list(APPEND CMAKE_MODULE_PATH "${KAZE_ROOT}/cmake/modules")

if (KAZE_ROOT STREQUAL CMAKE_SOURCE_DIR)
    set(KAZE_IS_ROOT ON CACHE BOOL "" FORCE)
else()
    set(KAZE_IS_ROOT OFF CACHE BOOL "" FORCE)
endif()

include(kaze/detection/compiler)
include(kaze/detection/platform)
include(kaze/utils)

# ===== Options =================================================================
include(kaze/option_defaults)

# Bring in user cmake file if it exists
if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.user.txt)
    include(${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.user.txt)
endif()

# ----- Kaze Core Options -----
set(KAZE_BACKEND        ${KAZE_BACKEND_DEFAULT}         CACHE STRING "Backend to compile Kaze with: glfw3, sdl3")
set(KAZE_BACKEND_SHARED ${KAZE_BACKEND_SHARED_DEFAULT}  CACHE BOOL   "Whether to link dynamically to backend library; static linkage if false")
set(KAZE_NO_NAMESPACE   OFF                             CACHE BOOL   "Unwrap all kaze engine types and functions into the global namespace")
set(KAZE_NAMESPACE      "kaze"                          CACHE STRING "Set the namespace name, KAZE_NO_NAMESPACE overrides this option")

set(KAZE_DEBUG           ${KAZE_DEBUG_DEFAULT} CACHE BOOL   "Build with debug mode: logging and asserts")
set(KAZE_BUILD_TESTS     ${KAZE_IS_ROOT}       CACHE BOOL   "Make kaze tests available for compilation")
set(KAZE_BUILD_UNITTESTS ${KAZE_BUILD_TESTS}   CACHE BOOL   "Build the unit tests for the kaze core library")

set(KAZE_CPU_INTRINSICS  ON                    CACHE BOOL   "Build with CPU intrinsic optimizations")

# This is buggy with BGFX so turned off for now
set(KAZE_USE_WAYLAND    OFF                    CACHE BOOL   "Build with Wayland support on Linux" FORCE)

set(KAZE_USE_FMT_LIB    ${KAZE_USE_FMT_LIB_DEFAULT}      CACHE BOOL   "Use fmt internal library instead of C++20 std::format")

set(KAZE_NO_MAIN        OFF                    CACHE BOOL   "Build without main or necessity to implement the kaze::kmain function")

set(KAZE_EMSCRIPTEN_PTHREAD_POOL_SIZE  4      CACHE STRING "Set the pthread pool size on Emscripten builds. 0 turns off pthreads")
set(KAZE_EMSCRIPTEN_WASM_WORKERS       ON     CACHE BOOL   "Use WASM Workers on Emscripten builds, automatically turns on if Audio module is activated")

# ----- Kaze Audio Options -----
set(KAZE_AUDIO                ON              CACHE BOOL    "Build Kaze's audio library")
set(KAZE_AUDIO_DECODE_FLAC    ON              CACHE BOOL    "Support decoding Flac audio format")
set(KAZE_AUDIO_DECODE_MP3     ON              CACHE BOOL    "Support decoding Mp3 audio format")
set(KAZE_AUDIO_DECODE_VORBIS  ON              CACHE BOOL    "Support decoding Ogg vorbis audio format")
set(KAZE_AUDIO_DECODE_WAV     ON              CACHE BOOL    "Support decoding Wav audio format")

# ----- Kaze Graphics Options -----
set(KAZE_GRAPHICS_IMGUI       ON              CACHE BOOL  "Include Imgui backend and source files")

# ----- Kaze Toolkit Options -----
set(KAZE_TK                   ON                      CACHE BOOL "Build Kaze Toolkit")
set(KAZE_TK_IMGUI_PLUGIN      ${KAZE_GRAPHICS_IMGUI}  CACHE BOOL "Add ImGUI plugin to Kaze Toolkit. KAZE_GRAPHICS_IMGUI must be ON")

# ===== Projects ==============================================================
# Prevent projects from generating ctest targets
set_property(GLOBAL PROPERTY CTEST_TARGETS_ADDED TRUE)

if (NOT KAZE_BUILD_TOOLS)
    add_subdirectory(kaze)

    if (KAZE_BUILD_TESTS)
        add_subdirectory(tests)
    endif()
endif()

add_subdirectory(util)
