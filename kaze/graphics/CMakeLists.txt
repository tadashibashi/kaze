project(kaze_graphics)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(kaze_graphics STATIC)
target_sources(kaze_graphics PRIVATE
        Color.h
        Color.cpp
        GraphicsMgr.h
        GraphicsMgr.cpp
        Image.h
        Image.cpp
        ImageHandle.h
        ImageHandle.cpp
        PixelFormat.h
        PixelFormat.cpp
        Renderable.h
        Renderable.cpp
        Shader.h
        Shader.cpp
        ShaderProgram.h
        ShaderProgram.cpp
        Texture2D.h
        Texture2D.cpp
        TextureHandle.h
        TextureHandle.cpp
        Uniform.h
        Uniform.cpp
        UniformMgr.h
        UniformMgr.cpp
        VertexLayout.h
        VertexLayout.cpp

        private/image.h
        private/image.cpp
        private/texture.h
        private/texture.cpp
)

target_link_libraries(kaze_graphics PRIVATE kaze_core bgfx bimg bimg_decode)

if (KAZE_GRAPHICS_IMGUI)
        kaze_target_module(kaze_graphics extras/imgui)
endif()
