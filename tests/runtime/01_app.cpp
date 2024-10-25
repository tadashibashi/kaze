#include <kaze/tk/App.h>
#include <kaze/tk/Camera2D.h>

#include <kaze/core/AssetLoader.h>
#include <kaze/core/debug.h>

#include <kaze/core/input/CursorConstants.h>

#include <kaze/core/math/Vec/Vec3.h>
#include <kaze/core/video/GraphicsMgr.h>
#include <kaze/core/video/Renderable.h>
#include <kaze/core/video/Texture2D.h>
#include <kaze/core/video/UniformMgr.h>

#include <bgfx/bgfx.h>
#include <kaze/tk/SpriteBatch.h>
#include <kaze/tk/plugins/imgui/imgui_plugin.h>

#include <imgui/imgui.h>

USING_KAZE_NAMESPACE;
USING_KAZE_TK_NAMESPACE;

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
        .size = {640, 480},
        .flags = WindowInit::Resizable,
    }) { }

    ~Demo() override { }
private:
    Camera2D camera;
    //AssetLoader<String, Texture2D> textures;
    Texture2D testTexture{};
    SpriteBatch batch{};

    struct TestImage
    {
        const Texture2D *texture{};
        Vec2f position{};
        Float rotation{};
        Vec2f scale{1.f, 1.f};

        auto draw(SpriteBatch &batch) -> void
        {
            if (texture)
            {
                batch.drawTexture(texture,
                    {0, 0, static_cast<Int>(texture->size().x), static_cast<Int>(texture->size().y)},
                    position,
                    {
                        .scale = scale,
                        .anchor = {0, 0},
                        .angle = mathf::toRadians(rotation),
                    });
            }
        }
    };

    List<TestImage> images{};

    auto init() -> Bool override {

        if ( !batch.init(graphics()) )
            return False;

        Image image;
        if ( !image.load("dungeon_tiles.png") )
            return False;

        if ( !testTexture.loadImage(image) )
            return False;

        cursors().set(CursorType::Pointer);

        images.reserve(1000);
        for (Int i = 0; i < 1000; ++i)
        {
            images.emplace_back(TestImage{
                .texture = &testTexture,
                .position = {(float)(rand() % 10000), (float)(rand() % 10000) },
                .rotation = (float)(rand() % 360),
                .scale = {1.f, 1.f}
            });
        }

        const auto windowSize = window().getSize();
        camera.setViewport({0, 0, windowSize.x, windowSize.y});
        camera.setOrigin({0, 0});

        addPlugin(imgui::createPlugin({
            .window = window().getHandle()
        }));

        return True;
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
            camera.setScale(camera.getScale() * Vec2f{1.05f, 1.05f});
        }

        if (input().isDown(Key::X))
        {
            camera.setScale(camera.getScale() * Vec2f{.95f, .95f});
        }

        if (const auto scroll = input().getScroll(); scroll.y != 0)
        {
            camera.setScale(camera.getScale() * (Vec2f::One + Vec2f(scroll.y * .05f, scroll.y * .05f)));
        }

        if (input().isDown(Key::R))
        {
            camera.setRotationDegrees(camera.getRotationDegrees() + 4);
        }
        if (input().isDown(Key::T))
        {
            camera.setRotationDegrees(camera.getRotationDegrees() - 4);
        }

        for (auto &image : images)
        {
            image.rotation = mathf::fmod(image.rotation + 4.f, 360.f);
        }
        Size i = 0;
        for (; i <= images.size() - 4; i += 4)
        {
            images[i].rotation = mathf::fmod(images[i].rotation + 4.f, 360.f);
            images[i+1].rotation = mathf::fmod(images[i+1].rotation + 4.f, 360.f);
            images[i+2].rotation = mathf::fmod(images[i+2].rotation + 4.f, 360.f);
            images[i+3].rotation = mathf::fmod(images[i+3].rotation + 4.f, 360.f);
        }

        for (; i < images.size(); ++i)
        {
            images[i].rotation = mathf::fmod(images[i].rotation + 4.f, 360.f);
        }

        ImGui::ShowDemoWindow();
    }

    auto render() -> void override {

        const auto displaySize = window().getDisplaySize();
        const auto size = window().getSize();
        camera.setViewport({0, 0, size.x, size.y});

        batch.begin({
            .viewMtx = camera.getView(),
            .projMtx = camera.getProj(),
        });

        Size i = 0;
        const Size half = images.size() / 2UL;
        for (; i <= half - 4; i += 4)
        {
            images[i].draw(batch);
            images[i+1].draw(batch);
            images[i+2].draw(batch);
            images[i+3].draw(batch);
        }

        for (; i < half; ++i)
        {
            images[i].draw(batch);
        }


        batch.end();

        batch.begin({
            .viewMtx = camera.getView(),
            .projMtx = camera.getProj(),
        });

        for (; i <= images.size() - 4; i += 4)
        {
            images[i].draw(batch);
            images[i+1].draw(batch);
            images[i+2].draw(batch);
            images[i+3].draw(batch);
        }

        for (; i < images.size(); ++i)
        {
            images[i].draw(batch);
        }

        batch.end();
    }

    auto close() -> void override {
        testTexture.release();
        batch.release();
    }
};

int main()
{
    Demo().run();
    return 0;
}
