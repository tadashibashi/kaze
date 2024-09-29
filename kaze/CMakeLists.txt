if (APPLE)
    project(kaze LANGUAGES C CXX OBJC OBJCXX)
else()
    project(kaze LANGUAGES C CXX)
endif()

include(kaze/dependencies)

add_library(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE
        concepts.h
        endian.h
        graphics/Color.cpp
        graphics/Color.h
        json.h
        debug.h
        debug.cpp
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
        kaze.h
        platform/Gamepad.cpp
        platform/Key.h
        platform/Keyboard.h
        platform/PlatformBackend.h
        platform/PlatformBackend.cpp
        platform/PlatformEvent.h
        platform/PlatformEvent.cpp
        platform/PlatformDefines.h
        platform/Mouse.h
        platform/Window.h
        platform/Window.cpp
        ServiceProvider.h
        ServiceProvider.cpp

        ${KAZE_BACKEND_SOURCES}
)

target_include_directories(${PROJECT_NAME} PUBLIC ${KAZE_ROOT})
target_compile_definitions(${PROJECT_NAME} PUBLIC
    KAZE_NAMESPACE=${KAZE_NAMESPACE}
    KAZE_DEBUG=${KAZE_DEBUG}
    KAZE_USE_WAYLAND=${KAZE_USE_WAYLAND}
    KAZE_NO_NAMESPACE=${KAZE_NO_NAMESPACE}
)

target_link_libraries(${PROJECT_NAME} PUBLIC gcem glm nlohmann_json spdlog::spdlog ${KAZE_BACKEND_LIBRARIES})
target_link_options(${PROJECT_NAME} PUBLIC ${KAZE_BACKEND_LINK_FLAGS})
target_compile_options(${PROJECT_NAME} PUBLIC ${KAZE_BACKEND_COMPILE_FLAGS})

