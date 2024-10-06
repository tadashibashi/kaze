if (APPLE)
    project(kaze LANGUAGES C CXX OBJC OBJCXX)
else()
    project(kaze LANGUAGES C CXX)
endif()

include(kaze/dependencies)

add_library(${PROJECT_NAME}
        input/Mouse.cpp)
target_sources(${PROJECT_NAME} PRIVATE
        app/Input.h

        errors.h
        errors.cpp

        Optional.h

        app/App.h
        app/App.cpp
        app/BackendInitGuard.h
        app/BackendInitGuard.cpp

        concepts.h
        endian.h
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

        input/Gamepad.cpp
        input/KeyboardConstants.h
        input/KeyboardConstants.cpp
        input/Keyboard.h
        input/Keyboard.cpp
        input/MouseConstants.h


        platform/PlatformBackend.h
        platform/PlatformBackend.cpp
        platform/PlatformEvent.h
        platform/PlatformEvent.cpp
        platform/PlatformDefines.h
        platform/backends/WindowHandleContainer.h

        ServiceProvider.h
        ServiceProvider.cpp

        video/Color.h
        video/Color.cpp
        video/Window.h
        video/Window.cpp
        video/WindowConstants.h

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

