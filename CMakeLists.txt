cmake_minimum_required(VERSION 3.29)
project(nova_engine LANGUAGES CXX)

# Set CMake module path
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

include(nova_utils)

# ===== C++ 20 Standard =======================================================
set(CMAKE_CXX_STANDARD 20)


# ===== Directory Variables ====================================================
if (${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR})
    set(NOVA_IS_ROOT 1)
else()
    set(NOVA_IS_ROOT 0)
endif()

set(NOVA_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

if (NOT NOVA_NAMESPACE) # Set the namespace name, can be overridden by the user
    set (NOVA_NAMESPACE nova)
endif()

option(NOVA_BUILD_TESTS "Include Nova Unit Test Targets" ${NOVA_IS_ROOT})


include(nova_dependencies)

add_subdirectory(nova)

if (NOVA_BUILD_TESTS)
    add_subdirectory(tests)
endif()

add_executable(${PROJECT_NAME}_sandbox main.cpp)
target_link_libraries(${PROJECT_NAME}_sandbox PRIVATE nova)
