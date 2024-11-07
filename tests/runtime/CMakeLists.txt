project(runtime_tests)

if (KAZE_PLATFORM_EMSCRIPTEN)
    set(CMAKE_EXECUTABLE_SUFFIX .html)
endif()

add_subdirectory(00_backend)
add_subdirectory(01_app)
add_subdirectory(02_ide)
