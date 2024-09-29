project(kaze_tests)
include(kaze/test_dependencies)

add_executable(${PROJECT_NAME}
    kaze/debug.tests.cpp
    kaze/graphics/Color.test.cpp
    kaze/macros/enum.test.cpp
    kaze/math/Circle.test.cpp
    kaze/math/geometry.test.cpp
    kaze/math/mathf.test.cpp
    kaze/math/Matrix.test.cpp
    kaze/math/Rect.test.cpp
    kaze/math/Rect3D.test.cpp
    kaze/math/Vec2.test.cpp
    kaze/math/Vec3.test.cpp
    kaze/math/Vec4.test.cpp
    kaze/ServiceProvider.test.cpp
    tests.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE kaze doctest::doctest)
