project(nova)

add_library(${PROJECT_NAME}

        nova.h
        macros/map.h
        macros/enum.h
        macros/json.h

        concepts.h
        ServiceProvider.h
        ServiceProvider.cpp
        math/Circle.h
        graphics/Color.cpp
        graphics/Color.h
        endian.h

        math/geometry.h
        math/Matrix.h
        math/mathf.h
        math/easings.h
        math/Vec/VecBase.h
        math/Vec/Vec1.h
        math/Vec/Vec2.h
        math/Vec/Vec3.h
        math/Vec/Vec4.h
        math/Vec.hpp
        math/Rect.h
        math/Rect3D.h
)

target_include_directories(${PROJECT_NAME} PUBLIC ${NOVA_ROOT})
target_compile_definitions(${PROJECT_NAME} PUBLIC NOVA_NAMESPACE=${NOVA_NAMESPACE})
target_link_libraries(${PROJECT_NAME} PUBLIC gcem glm)

