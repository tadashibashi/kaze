if (APPLE)
    project(kaze LANGUAGES C CXX OBJC OBJCXX)
else()
    project(kaze LANGUAGES C CXX)
endif()

# Library

include(kaze/dependencies)

add_library(${PROJECT_NAME})

# Built-in Shaders
include(bgfxToolUtils)

set(KAZE_SHADER_ROOT ${CMAKE_BINARY_DIR}/include/generated/shaders)
if (EMSCRIPTEN)
    set (KAZE_SHADER_INCLUDES ${KAZE_SHADER_ROOT}/essl)
elseif(APPLE)
    set (KAZE_SHADER_INCLUDES ${KAZE_SHADER_ROOT}/mtl)
elseif(WINDOWS)
    set (KAZE_SHADER_INCLUDES ${KAZE_SHADER_ROOT}/dx11)
elseif(ANDROID) # TODO: compatibility via OpenGLES on older devices
    set (KAZE_SHADER_INCLUDES ${KAZE_SHADER_ROOT}/spv)
elseif(LINUX)   # TODO: compatibility via OpenGL on older devices
    set (KAZE_SHADER_INCLUDES ${KAZE_SHADER_ROOT}/spv)
else() # Default to OpenGL
    set (KAZE_SHADER_INCLUDES ${KAZE_SHADER_ROOT}/glsl)
endif()

bgfx_compile_shaders(
    TYPE VERTEX
    SHADERS shaders/spritebatch_v.sc
    VARYING_DEF ${CMAKE_CURRENT_SOURCE_DIR}/shaders/varying.def.sc
    OUTPUT_DIR ${CMAKE_BINARY_DIR}/include/generated/shaders
    INCLUDE_DIRS ${BGFX_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/shaders
    AS_HEADERS
)
bgfx_compile_shaders(
    TYPE FRAGMENT
    SHADERS shaders/spritebatch_f.sc
    VARYING_DEF ${CMAKE_CURRENT_SOURCE_DIR}/shaders/varying.def.sc
    OUTPUT_DIR ${CMAKE_BINARY_DIR}/include/generated/shaders
    INCLUDE_DIRS ${BGFX_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/shaders
    AS_HEADERS
)
set(KAZE_SHADER_SOURCES
    shaders/spritebatch_v.sc
    shaders/spritebatch_f.sc
)

target_sources(${PROJECT_NAME} PRIVATE
    kaze.h
    concepts.h
    debug.h
    debug.cpp
    errors.h
    errors.cpp
    endian.h
    json.h
    memory.h
    memory.cpp

    app/App.h
    app/App.cpp
    app/BackendInitGuard.h
    app/BackendInitGuard.cpp
    app/Camera2D.h
    app/Camera2D.cpp
    app/InputRouter.h
    app/InputRouter.cpp
    app/SpriteBatch.h
    app/SpriteBatch.cpp

    core/AssetLoader.h
    core/Optional.h
    core/ServiceProvider.h
    core/ServiceProvider.cpp

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

    io/BufferView.h
    io/BufferView.cpp
    io/FileBuffer.h
    io/FileBuffer.cpp
    io/io.h
    io/io.cpp

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

    platform/backend.h
    platform/backend.cpp
    platform/PlatformCallbacks.h
    platform/PlatformCallbacks.cpp
    platform/defines.h
    platform/backends/WindowHandleContainer.h

    video/Color.h
    video/Color.cpp
    video/GraphicsMgr.h
    video/GraphicsMgr.cpp
    video/Renderable.h
    video/Renderable.cpp
    video/Shader.h
    video/Shader.cpp
    video/ShaderProgram.h
    video/ShaderProgram.cpp
    video/Texture2D.h
    video/Texture2D.cpp
    video/UniformMgr.h
    video/UniformMgr.cpp
    video/VertexLayout.h
    video/VertexLayout.cpp
    video/Window.h
    video/Window.cpp
    video/WindowConstants.h

    ${KAZE_BACKEND_SOURCES}
    ${KAZE_SHADER_SOURCES}
)

target_include_directories(${PROJECT_NAME} PUBLIC ${KAZE_ROOT} ${KAZE_SHADER_INCLUDES})
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
    ${KAZE_BACKEND_LIBRARIES}
)
target_link_options(${PROJECT_NAME} PUBLIC ${KAZE_BACKEND_LINK_FLAGS})
target_compile_options(${PROJECT_NAME} PUBLIC ${KAZE_BACKEND_COMPILE_FLAGS})

