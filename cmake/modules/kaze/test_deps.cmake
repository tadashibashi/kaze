include(FetchContent)

if (NOT EXISTS doctest::doctest)
    FetchContent_Declare(doctest
            GIT_REPOSITORY https://github.com/doctest/doctest.git
            GIT_TAG        v2.4.11
    )
    FetchContent_MakeAvailable(doctest)
endif()