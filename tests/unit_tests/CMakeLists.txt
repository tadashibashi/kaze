project(kaze_unit_tests)

add_executable(${PROJECT_NAME}
    kaze/core/AssetLoader.test.cpp
    kaze/core/Action.test.cpp
    kaze/core/ConditionalAction.test.cpp
    kaze/core/debug.test.cpp
    kaze/core/endian.test.cpp
    kaze/core/Memory.test.cpp
    kaze/core/ServiceProvider.test.cpp
    kaze/core/io/BufferWriter.test.cpp
    kaze/core/io/BufferView.test.cpp
    kaze/core/io/StructIO.test.cpp
    kaze/core/io/StructLayout.test.cpp
    kaze/core/macros/enum.test.cpp
    kaze/core/math/Circle.test.cpp
    kaze/core/math/geometry.test.cpp
    kaze/core/math/mathf.test.cpp
    kaze/core/math/Matrix.test.cpp
    kaze/core/math/Rect.test.cpp
    kaze/core/math/Rect3D.test.cpp
    kaze/core/math/Vec2.test.cpp
    kaze/core/math/Vec3.test.cpp
    kaze/core/math/Vec4.test.cpp

    kaze/gfx/Color.test.cpp

    kaze/snd/SampleFormat.test.cpp

    tests.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    kaze_core
    kaze_snd
    doctest::doctest
)
