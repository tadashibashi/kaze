project(kaze_unit_tests)

add_executable(${PROJECT_NAME}
    kaze/core/ServiceProvider.test.cpp
    kaze/core/Action.test.cpp
    kaze/debug.tests.cpp
    kaze/graphics/Color.test.cpp
    kaze/io/BufferView.test.cpp
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
    tests.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE kaze doctest::doctest)
