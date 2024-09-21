project(nova_tests)
include(nova_tests_dependencies)

add_executable(${PROJECT_NAME}
    nova/graphics/Color.test.cpp
    nova/macros/enum.test.cpp
    nova/math/Circle.test.cpp
    nova/math/geometry.test.cpp
    nova/math/mathf.test.cpp
    nova/math/Matrix.test.cpp
    nova/math/Rect.test.cpp
    nova/math/Rect3D.test.cpp
    nova/math/Vec2.test.cpp
    nova/math/Vec3.test.cpp
    nova/math/Vec4.test.cpp
    nova/ServiceProvider.test.cpp
    tests.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE nova doctest::doctest)
