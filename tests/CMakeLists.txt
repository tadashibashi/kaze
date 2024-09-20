project(nova_tests)

include(FetchContent)

if (NOT EXISTS doctest::doctest)
    FetchContent_Declare(doctest
        GIT_REPOSITORY https://github.com/doctest/doctest.git
        GIT_TAG        v2.4.11
    )
    FetchContent_MakeAvailable(doctest)
endif()

add_executable(${PROJECT_NAME}
    tests.cpp
    nova/macros/enum.test.cpp
    nova/math/mathf.test.cpp
    nova/ServiceProvider.test.cpp
    nova/math/Vec2.test.cpp
    nova/graphics/Color.test.cpp
    nova/math/Vec3.test.cpp
    nova/math/Vec4.test.cpp
    nova/math/Circle.test.cpp
    nova/math/Rect.test.cpp
    nova/math/Matrix.test.cpp
    nova/math/geometry.test.cpp
    nova/math/Rect3D.test.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE nova doctest::doctest)
