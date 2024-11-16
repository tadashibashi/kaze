project(kaze_unit_tests)

add_executable(${PROJECT_NAME}
    kaze/audio/SampleFormat.test.cpp
    kaze/core/AssetLoader.test.cpp
    kaze/core/Action.test.cpp
    kaze/core/ConditionalAction.test.cpp
    kaze/core/debug.test.cpp
    kaze/core/endian.test.cpp
    kaze/core/graphics/Color.test.cpp
    kaze/core/Memory.test.cpp
    kaze/core/ServiceProvider.test.cpp

    kaze/core/io/BufferIO.test.cpp
    kaze/core/io/BufferWriter.test.cpp
    kaze/core/io/BufferView.test.cpp
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

    tests.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    kaze_core
    kaze_audio
    doctest::doctest
)
