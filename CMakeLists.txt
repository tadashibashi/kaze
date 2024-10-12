cmake_minimum_required(VERSION 3.28)
project(kaze_engine LANGUAGES CXX)

# ===== C++ 20 Standard =======================================================
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF) # No C++20 Modules in this project

# ===== Directory Variables ===================================================
set(KAZE_ROOT ${CMAKE_CURRENT_SOURCE_DIR} CACHE STRING "Path to the entire project's root" FORCE)
set(KAZE_SOURCE_ROOT ${KAZE_ROOT}/kaze    CACHE STRING "Path to the Kaze engine source code directory" FORCE)
set(KAZE_BACKEND_ROOT ${KAZE_SOURCE_ROOT}/platform/backends CACHE STRING "Path to the Kaze engine backends" FORCE)
set(KAZE_TESTS_ROOT ${KAZE_ROOT}/tests)

list(APPEND CMAKE_MODULE_PATH ${KAZE_ROOT}/cmake/modules)

if (KAZE_ROOT STREQUAL CMAKE_SOURCE_DIR)
    set(KAZE_IS_ROOT ON CACHE BOOL "" FORCE)
else()
    set(KAZE_IS_ROOT OFF CACHE BOOL "" FORCE)
endif()

# ===== Options =================================================================
include(kaze/option_defaults)

# Bring in user cmake file if it exists
if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.user.txt)
    include(${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.user.txt)
endif()

set(KAZE_BACKEND        ${KAZE_BACKEND_DEFAULT}         CACHE STRING "Backend to compile Kaze with: glfw3, sdl3")
set(KAZE_BACKEND_SHARED ${KAZE_BACKEND_SHARED_DEFAULT}  CACHE BOOL   "Whether to link dynamically to backend library; static linkage if false")
set(KAZE_NO_NAMESPACE    OFF                            CACHE BOOL   "Unwrap all kaze engine types and functions into the global namespace")
set(KAZE_NAMESPACE      "kaze"                          CACHE STRING "Set the namespace name, KAZE_NO_NAMESPACE overrides this option")

set(KAZE_DEBUG         ${KAZE_DEBUG_DEFAULT} CACHE BOOL   "Build with debug mode: logging and asserts")
set(KAZE_BUILD_TESTS   ${KAZE_IS_ROOT}       CACHE BOOL   "Make kaze unit tests available for compilation")

set(KAZE_USE_WAYLAND   OFF                   CACHE BOOL   "Build with Wayland support on Linux")

# ===== Projects ==============================================================
add_subdirectory(kaze)

if (KAZE_BUILD_TESTS)
    add_subdirectory(tests)
endif()
