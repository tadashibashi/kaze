if (APPLE)
    project(kaze LANGUAGES C CXX OBJC OBJCXX)
else()
    project(kaze LANGUAGES C CXX)
endif()

include(kaze/dependencies)

add_library(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE
        kaze.h
        concepts.h
        debug.h
        debug.cpp
        errors.h
        errors.cpp
        endian.h
        json.h

        app/App.h
        app/App.cpp
        app/BackendInitGuard.h
        app/BackendInitGuard.cpp

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

