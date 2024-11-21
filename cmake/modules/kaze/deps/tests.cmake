include(FetchContent)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (NOT EXISTS doctest::doctest)
    FetchContent_Declare(doctest
            GIT_REPOSITORY https://github.com/doctest/doctest.git
            GIT_TAG        v2.4.11
    )

    set(DOCTEST_WITH_TESTS OFF)
    set(DOCTEST_NO_INSTALL ON)
    set(DOCTEST_WITH_MAIN_IN_STATIC_LIB OFF)
    FetchContent_MakeAvailable(doctest)
endif()
