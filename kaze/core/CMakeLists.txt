if (APPLE)
    project(kaze LANGUAGES C CXX OBJC OBJCXX)
else()
    project(kaze LANGUAGES C CXX)
endif()

include(kaze/core_deps)

add_library(${PROJECT_NAME}
        concepts.h
        debug.h
        debug.cpp
        errors.h
        errors.cpp
        endian.h
        json.h
        lib.h
        memory.h
        memory.cpp

        AssetLoader.h
        Optional.h
        Memory.h
        ServiceProvider.h
        ServiceProvider.cpp

        input/Gamepad.h
        input/Gamepad.cpp
        input/InputMgr.h
        input/InputMgr.cpp
        input/KeyboardConstants.h
        input/KeyboardConstants.cpp
        input/Keyboard.h
        input/Keyboard.cpp
        input/Mouse.h
        input/Mouse.cpp
        input/MouseConstants.h

        io/BufferIO.h
        io/BufferIO.cpp
        io/BufferView.h
        io/BufferView.cpp
        io/BufferWriter.h
        io/BufferWriter.cpp
        io/FileBuffer.h
        io/FileBuffer.cpp
        io/io.h
        io/io.cpp
        io/StructLayout.h

        macros/enum.h
        macros/map.h

        math/Circle.h
        math/easings.h
        math/geometry.h
        math/Matrix.h
        math/mathf.h
        math/Rect.h
        math/Rect3D.h
        math/Vec/VecBase.h
        math/Vec/Vec1.h
        math/Vec/Vec2.h
        math/Vec/Vec3.h
        math/Vec/Vec4.h
        math/Vec.hpp

        platform/backend/backend.h
        platform/backend/backend.cpp
        platform/BackendInitGuard.h
        platform/BackendInitGuard.cpp
        platform/defines.h

        video/Color.h
        video/Color.cpp
        video/GraphicsMgr.h
        video/GraphicsMgr.cpp
        video/Image.h
        video/Image.cpp
        video/ImageHandle.h
        video/ImageHandle.cpp
        video/PixelFormat.h
        video/PixelFormat.cpp
        video/Renderable.h
        video/Renderable.cpp
        video/Shader.h
        video/Shader.cpp
        video/ShaderProgram.h
        video/ShaderProgram.cpp
        video/Texture2D.h
        video/Texture2D.cpp
        video/TextureHandle.h
        video/TextureHandle.cpp
        video/Uniform.h
        video/Uniform.cpp
        video/UniformMgr.h
        video/UniformMgr.cpp
        video/VertexLayout.h
        video/VertexLayout.cpp
        video/Window.h
        video/Window.cpp
        video/WindowConstants.h

        video/private/image.h
        video/private/image.cpp
        video/private/texture.h
        video/private/texture.cpp
)

target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC ${KAZE_ROOT})

if (KAZE_NO_NAMESPACE)
    set(KAZE_NO_NAMESPACE 1)
else()
    set(KAZE_NO_NAMESPACE 0)
endif()

target_compile_definitions(${PROJECT_NAME} PUBLIC
    KAZE_NAMESPACE=${KAZE_NAMESPACE}
    KAZE_DEBUG=${KAZE_DEBUG}
    KAZE_USE_WAYLAND=${KAZE_USE_WAYLAND}
    KAZE_NO_NAMESPACE=${KAZE_NO_NAMESPACE}
)

target_link_libraries(${PROJECT_NAME} PUBLIC
    gcem
    glm
    nlohmann_json
    spdlog::spdlog
    bgfx bx bimg bimg_decode
)

target_precompile_headers(${PROJECT_NAME} PUBLIC lib.pch)
kaze_target_plugin(${PROJECT_NAME} "${KAZE_BACKEND_ROOT}/${KAZE_BACKEND}")

