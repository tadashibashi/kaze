#include <kaze/kaze.h>
#include <kaze/debug.h>
#include <kaze/app/App.h>
#include <kaze/app/Camera2D.h>
#include <kaze/math/Vec/Vec3.h>
#include <kaze/video/GraphicsMgr.h>
#include <kaze/video/Renderable.h>
#include <kaze/video/Texture2D.h>

#include "spritebatch_f.sc.bin.h"
#include "spritebatch_v.sc.bin.h"

#include <bgfx/bgfx.h>
#include <kaze/core/AssetLoader.h>
#include <kaze/video/UniformMgr.h>

USING_KAZE_NAMESPACE;

struct Vertex {
    Vec3f position;
    Vec2f uv;
    Color color;
};

Vertex rectVertices[] = {
    { {50, 50, 0.0f},   {0, 0},  Color::White },
    { {100, 50, 0.0f},  {1, 0},  Color::White },
    { {100, 100, 0.0f}, {1, 1},  Color::White },
    { {50, 100, 0.0f},  {0, 1},  Color::White }
};

Uint16 rectTriangles[] = {
    0, 1, 2,
    0, 2, 3,
};

class Demo : public App {
public:
    Demo() : App({
        .title = "App Demo",
        .size = {200, 200},
        .flags = WindowInit::Resizable
    }) { }
private:
    Renderable renderable;
    Camera2D camera;
    Texture2D pixel;
    UniformMgr uniforms;
    AssetLoader<String, Texture2D> textures;
    const Texture2D *warioTexture{};

    auto init() -> Bool override {
        if ( !renderable.init({
            .vertShader = Shader(spritebatch_v_mtl, std::size(spritebatch_v_mtl)),
            .fragShader = Shader(spritebatch_f_mtl, std::size(spritebatch_f_mtl)),
            .layout = VertexLayout()
                .begin()
                    .add(Attrib::Position, 3, AttribType::Float)
                    .add(Attrib::TexCoord0, 2, AttribType::Float)
                    .add(Attrib::Color0, 4, AttribType::Uint8, KAZE_TRUE)
                .end(),
            .viewId = 0,
            .initialVertexCount = 10000,
            .initialIndexCount = 10000
        }) )
        {
            return KAZE_FALSE;
        }

        if ( !pixel.loadPixels(&Color::White, 1, 1, 1) )
        {
            return KAZE_FALSE;
        }

        if (warioTexture = textures.load("dungeon_tiles.png"); warioTexture == nullptr)
        {
            return KAZE_FALSE;
        }

        renderable.setVertices(rectVertices, sizeof(rectVertices));
        renderable.setIndices(rectTriangles, std::size(rectTriangles));

        const auto windowSize = window().getSize();
        camera.setViewport({0, 0, windowSize.x, windowSize.y});
        camera.setOrigin({0, 0});
        return KAZE_TRUE;
    }

    auto update() -> void override {
        if (input().isDown(Key::Escape))
            quit();

        if (input().getAxesMoved(0, GamepadAxis::LeftX, GamepadAxis::LeftY, .2f))
        {
            auto axes = input().getAxes(0, GamepadAxis::LeftX, GamepadAxis::LeftY, .2f);
            KAZE_LOG("Axis moved to: {}, {}", axes.x, axes.y);
        }

        if (input().isDown(Key::Left))
        {
            camera.setPosition(camera.getPosition() + Vec2f{-4.f, 0});
        }

        if (input().isDown(Key::Right))
        {
            camera.setPosition(camera.getPosition() + Vec2f{4.f, 0});
        }

        if (input().isDown(Key::Up))
        {
            camera.setPosition(camera.getPosition() + Vec2f{0, -4.f});
        }

        if (input().isDown(Key::Down))
        {
            camera.setPosition(camera.getPosition() + Vec2f{0, 4.f});
        }

        if (input().isDown(Key::Z))
        {
            camera.setScale(camera.getScale() + Vec2f{.05f, .05f});
        }

        if (input().isDown(Key::X))
        {
            camera.setScale(camera.getScale() + Vec2f{-.05f, -.05f});
        }
    }

    auto draw() -> void override {
        const auto displaySize = window().getDisplaySize();
        const auto size = window().getSize();
        camera.setViewport({0, 0, size.x, size.y});

        uniforms.setTexture(0, *warioTexture);
        renderable.setViewTransform(camera.getView(), camera.getProj());
        renderable.setViewRect({0, 0, displaySize.x, displaySize.y});
        renderable.submit();
    }

    auto close() -> void override {
        uniforms.clear();
        pixel.release();
        textures.clear();
        renderable.release();
    }
};

int main()
{
    Demo().run();
    return 0;
}
