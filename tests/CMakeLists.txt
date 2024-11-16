project(kaze_tests)
include(kaze/deps/tests)

add_subdirectory(runtime)

if (KAZE_BUILD_UNITTESTS)
    add_subdirectory(unit_tests)
endif()
