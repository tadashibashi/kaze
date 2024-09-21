project(nova)
include(nova_dependencies)

add_library(${PROJECT_NAME}
    concepts.h
    endian.h
    graphics/Color.cpp
    graphics/Color.h
    json.h
    logging.h
    logging.cpp
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
    nova.h
    platform/Key.h
    platform/Keyboard.h
    platform/PlatformBackend.h
    platform/Window.h
    ServiceProvider.h
    ServiceProvider.cpp
        platform/sdl3/Window.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC ${NOVA_ROOT})
target_compile_definitions(${PROJECT_NAME} PUBLIC NOVA_NAMESPACE=${NOVA_NAMESPACE} NOVA_DEBUG=${NOVA_DEBUG})
target_link_libraries(${PROJECT_NAME} PUBLIC gcem glm nlohmann_json spdlog::spdlog)

