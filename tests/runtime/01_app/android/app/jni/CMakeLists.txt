cmake_minimum_required(VERSION 3.19)

project(main)

# armeabi-v7a requires cpufeatures library
# include(AndroidNdkModules)
# android_ndk_import_module_cpufeatures()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# This project is a "test", so we need to turn this opt on
set(KAZE_BUILD_TESTS     ON  CACHE BOOL "" FORCE)
set(KAZE_BUILD_UNITTESTS OFF CACHE BOOL "" FORCE)
# Point to the root of this repo to add it as a "subdirectory"
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../../../../../.."
    "${CMAKE_CURRENT_BINARY_DIR}/kaze-cmake")

add_library(main SHARED main.cpp)
target_link_libraries(main PRIVATE ktest_01_app log android EGL)
