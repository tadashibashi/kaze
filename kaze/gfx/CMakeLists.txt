project(kaze_gfx)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(kaze_gfx STATIC)
add_library(kaze::gfx ALIAS kaze_gfx)
target_sources(kaze_gfx PRIVATE
        Camera2D.cpp
        Camera2D.h
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
        SpriteBatch.cpp
        SpriteBatch.h
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

target_link_libraries(kaze_gfx PRIVATE kaze_core bgfx bimg bimg_decode)

if (KAZE_GFX_IMGUI)
        kaze_target_module(kaze_gfx extras/imgui)
endif()
