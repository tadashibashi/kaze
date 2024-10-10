#include <kaze/kaze.h>
#include <kaze/debug.h>
#include <kaze/app/App.h>
#include <kaze/app/Camera2D.h>
#include <kaze/math/Vec/Vec3.h>
#include <kaze/video/GraphicsMgr.h>
#include <kaze/video/Renderable.h>

#include "f_simple.h"
#include "v_simple.h"

#include <bgfx/bgfx.h>

USING_KAZE_NAMESPACE;

struct Vertex {
    Vec3f position;
    Color color;
};

Vertex rectVertices[] = {
    { {50, 50, 0.0f},   Color::fromRGBA8(0xff0000ff) },
    { {100, 50, 0.0f},  Color::fromRGBA8(0xff0000ff) },
    { {100, 100, 0.0f}, Color::fromRGBA8(0xff00ff00) },
    { {50, 100, 0.0f},  Color::fromRGBA8(0xff00ff00) }
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

    auto init() -> Bool override {
        if ( !renderable.init({
            .vertShader = Shader(v_simple, std::size(v_simple)),
            .fragShader = Shader(f_simple, std::size(f_simple)),
            .layout = VertexLayout()
                .begin()
                    .add(Attrib::Position, 3, AttribType::Float)
                    .add(Attrib::Color0, 4, AttribType::Uint8, KAZE_TRUE)
                .end(),
            .viewId = 0,
            .initialVertexCount = 10000,
            .initialIndexCount = 10000
        }) )
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
        const auto size = window().getDisplaySize();

        renderable.setViewTransform(camera.getView(), camera.getProj());
        renderable.setViewRect({0, 0, size.x, size.y});
        renderable.submit();
    }

    auto close() -> void override {
        renderable.release();
    }
};

int main()
{
    Demo().run();
    return 0;
}
