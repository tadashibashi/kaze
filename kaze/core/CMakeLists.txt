if (KAZE_PLATFORM_APPLE)
    project(kaze LANGUAGES C CXX OBJC OBJCXX)
else()
    project(kaze LANGUAGES C CXX)
endif()

include(kaze/deps/core)

add_library(kaze)
kaze_target_modules(kaze platform io)
target_include_directories(kaze SYSTEM PUBLIC ${KAZE_ROOT})

target_link_libraries(kaze PUBLIC
    gcem
    glm
    nlohmann_json
    spdlog::spdlog
    bgfx bx bimg bimg_decode
)

if (NOT KAZE_PLATFORM_ANDROID)
    target_precompile_headers(kaze PUBLIC lib.pch)
endif()

target_sources(kaze PRIVATE
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
        AlignedList.h
        Optional.h
        MemView.h

        ServiceProvider.h
        ServiceProvider.cpp

        input/CursorMgr.h
        input/CursorMgr.cpp
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
        math/private/mat4f.h
        math/private/mat4f.cpp

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

# ===== Compile defines ================
kaze_normalize_bool(KAZE_NO_NAMESPACE KAZE_NO_NAMESPACE)
kaze_normalize_bool(KAZE_DEBUG KAZE_DEBUG)
kaze_normalize_bool(KAZE_USE_WAYLAND KAZE_USE_WAYLAND)
kaze_normalize_bool(KAZE_USE_FMT_LIB KAZE_USE_FMT_LIB)
target_compile_definitions(kaze PUBLIC
    KAZE_NAMESPACE=${KAZE_NAMESPACE}
    KAZE_DEBUG=${KAZE_DEBUG}
    KAZE_USE_WAYLAND=${KAZE_USE_WAYLAND}
    KAZE_NO_NAMESPACE=${KAZE_NO_NAMESPACE}
    KAZE_USE_FMT_LIB=${KAZE_USE_FMT_LIB}
)

# ===== Compiler-specific settings =====

# Emscripten flags
if (KAZE_PLATFORM_EMSCRIPTEN)
    target_link_options(kaze PUBLIC
        -s ALLOW_MEMORY_GROWTH=1
        -msimd128)
    target_compile_options(kaze PUBLIC
        -msimd128)

    if (KAZE_DEBUG)
        target_link_options(kaze PUBLIC
            -gsource-map)
    endif()
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # May potentially hide an important warning, but otherwise the console gets cluttered
    target_compile_options(kaze PUBLIC "-Wno-psabi")
endif()
