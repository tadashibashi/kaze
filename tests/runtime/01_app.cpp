#include <kaze/kaze.h>

#include <kaze/app/App.h>
#include <kaze/app/Camera2D.h>

#include <kaze/core/AssetLoader.h>
#include <kaze/debug.h>

#include <kaze/math/Vec/Vec3.h>
#include <kaze/video/GraphicsMgr.h>
#include <kaze/video/Renderable.h>
#include <kaze/video/Texture2D.h>
#include <kaze/video/UniformMgr.h>

#include <bgfx/bgfx.h>
#include <kaze/app/SpriteBatch.h>

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
    { {50, 100, 0.0f},  {0, 1},  Color::White },
    { {150, 150, 0.0f}, {0, 0},  Color::White },
    { {300, 150, 0.0f}, {1, 0},  Color::White },
    { {300, 300, 0.0f}, {1, 1},  Color::White },
    { {150, 300, 0.0f}, {0, 1},  Color::White },
};

Uint16 rectTriangles[] = {
    0, 1, 2,
    0, 2, 3,
    4, 5, 6,
    4, 6, 7,
};

class Demo final : public App {
public:
    Demo() : App({
        .title = "App Demo",
        .size = {200, 200},
        .flags = WindowInit::Resizable
    }) { }

    ~Demo() override { }
private:
    Renderable renderable;
    Camera2D camera;
    Texture2D pixel;
    UniformMgr uniforms;
    AssetLoader<String, Texture2D> textures;
    const Texture2D *warioTexture{};
    SpriteBatch batch{};

    struct TestImage
    {
        const Texture2D *texture{};
        Vec2f position{};
        Float rotation{};
        Vec2f scale{1.f, 1.f};

        auto draw(SpriteBatch &batch) -> void
        {
            batch.drawTexture(*texture, {0, 0, texture->size().x, texture->size().y}, position, {
                .scale = scale,
                .anchor = {0, 0},
                .angle = mathf::toRadians(rotation),
            });
        }
    };

    List<TestImage> images{};

    auto init() -> Bool override {
        if ( !renderable.init({
            .viewId = 0,
            .vertShader = std::move(Shader(Shader::makePath("kaze/shaders", "spritebatch_v.sc.bin"))),
            .fragShader = std::move(Shader(Shader::makePath("kaze/shaders", "spritebatch_f.sc.bin"))),
            .layout = VertexLayout()
                .begin()
                    .add(Attrib::Position, 3, AttribType::Float)
                    .add(Attrib::TexCoord0, 2, AttribType::Float)
                    .add(Attrib::Color0, 4, AttribType::Uint8, KAZE_TRUE)
                .end(),
            .initialVertexCount = 10000,
            .initialIndexCount = 10000
        }) )
        {
            return KAZE_FALSE;
        }

        if ( !pixel.loadPixels(makeRef(&Color::White, 1), 1, 1) )
        {
            return KAZE_FALSE;
        }

        if ( !batch.init(window(), uniforms) )
            return KAZE_FALSE;

        if (warioTexture = textures.load("dungeon_tiles.png"); warioTexture == nullptr)
        {
            return KAZE_FALSE;
        }

        images.reserve(15000);
        for (Int i = 0; i < 15000; ++i)
        {
            images.emplace_back(TestImage{
                .texture = warioTexture,
                .position = {(float)(rand() % 1000), (float)(rand() % 1000) },
                .rotation = (float)(rand() % 360),
                .scale = {1.f, 1.f}
            });
        }

        renderable.setVertices(makeRef(rectVertices));
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

        if (input().isDown(Key::R))
        {
            camera.setRotationDegrees(camera.getRotationDegrees() + 4);
        }
        if (input().isDown(Key::T))
        {
            camera.setRotationDegrees(camera.getRotationDegrees() - 4);
        }
    }

    auto draw() -> void override {
        const auto displaySize = window().getDisplaySize();
        const auto size = window().getSize();
        camera.setViewport({0, 0, size.x, size.y});

        batch.begin({
            .projMtx = camera.getProj(),
            .viewMtx = camera.getView(),
        });

        for (auto &img : images)
            img.draw(batch);

        batch.end();

        for (auto &image : images)
        {
            image.rotation = mathf::fmod(image.rotation + 4.f, 360.f);
        }
    }

    auto close() -> void override {
        uniforms.clear();
        pixel.release();
        textures.clear();
        batch.release();
        renderable.release();
    }
};

int main()
{
    Demo().run();
    return 0;
}
