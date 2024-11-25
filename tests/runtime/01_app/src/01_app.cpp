#include <kaze/tk.hpp>
#include <kaze/core/platform/http/http.h>
#include <imgui/imgui.h>

#include <filesystem>

USING_KAZE_NS;

class Demo final : public App {
public:
    Demo() : App({
        .title = "App Demo",
        .size = {640, 480},
        .flags = WindowInit::Resizable,
    }) { }

    ~Demo() override { }
private:
    gfx::Camera2D camera;
    //AssetLoader<String, Texture2D> textures;
    gfx::Texture2D testTexture{};
    gfx::SpriteBatch batch{};
    snd::AudioEngine audio{};
    Handle<snd::Sound> computerScoreSnd{}, playerScoreSnd{};

    struct TestImage
    {
        const gfx::Texture2D *texture{};
        Vec2f position{};
        Float rotation{};
        Vec2f scale{1.f, 1.f};

        auto draw(gfx::SpriteBatch &batch) -> void
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

        const std::filesystem::path baseDir = filesys::getBaseDir();

        gfx::Image image;
        if ( !image.load((baseDir / "assets/dungeon_tiles.png").string()) )
            return False;

        if ( !testTexture.loadImage(image) )
            return False;

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

        addPlugin(plugins::imgui::create({
            .window = window().getHandle(),
            .viewId=1,
        }));

        if ( !audio.open({
            .samplerate = 0,
            .bufferFrameSize = 1024
        }) )
            return False;

        computerScoreSnd = audio.createSound( (baseDir / "assets/computer_score.wav").string(),
            snd::Sound::InMemory | snd::Sound::OneShot);
        playerScoreSnd = audio.createSound( (baseDir / "assets/player_score.wav").string(),
            snd::Sound::InMemory | snd::Sound::OneShot);
        if ( !computerScoreSnd || !playerScoreSnd )
            return False;

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

        if (input().isDown(MouseBtn::Left))
        {
            audio.playSound(playerScoreSnd, False);
        }

        if (input().isJustDown(Key::C))
        {
            audio.playSound(computerScoreSnd, False);
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


        static char text[1024];
        if (ImGui::Begin("Multi-line Test"))
        {
            if (ImGui::InputTextMultiline("Multi-line Text", text, 1024, {480, 1024}))
            {

            }
        }
        ImGui::End();


        ImGui::ShowDemoWindow();
        if (ImGui::Begin("Frame rate"))
        {
            ImGui::Text("Current fps: %f", fps());

            ImGui::Spacing();

            if (ImGui::Button("Play Sound!"))
            {
                audio.playSound(playerScoreSnd, False);
                KAZE_LOG("Pressed the button at: {}", time());
            }

            static String resString;
            if (ImGui::Button("Request"))
            {
                HttpRequest::create({
                        .url = fmt_lib::format("http://{}:3000", http::getLocalHost()),
                        .method = HttpRequest::Get,
                        .body = {},
                        .headers = {}
                }).send(
                    [] (const HttpResponse &res, void *userdata) {
                        if (res.ok())
                        {
                            auto str = static_cast<String *>(userdata);
                            *str = res.body;

                            for (const auto &[header, value] : res.headers)
                            {
                                KAZE_LOG("Header => {}: {}", header, value);
                            }
                        }
                        else
                        {
                            KAZE_ERR("Http error: {}: {}", res.status, res.error);
                            for (const auto &[header, value] : res.headers)
                            {
                                KAZE_LOG("Header => {}: {}", header, value);
                            }
                        }
                    },
                    &resString);
            }

            static String uploadRes;
            static Int uploadTimes;
            if (ImGui::Button("Upload"))
            {
                HttpRequest::create({
                    .url = fmt_lib::format("http://{}:3000/upload", http::getLocalHost()),
                    .method = HttpRequest::Post,
                    .mimeType = "text/plain",
                    .body = fmt_lib::format("Testing 1 2 3 at app time: {}", time()),
                }).send(
                    [] (const HttpResponse &res, void *userdata) {
                        if (res.ok())
                        {
                            auto str = (String *)userdata;
                            *str = fmt_lib::format("Uploaded x{} times", ++uploadTimes);
                        }
                    }, &uploadRes);
            }

            ImGui::Text("%s", resString.c_str());
            ImGui::Text("%s", uploadRes.c_str());
        }
        ImGui::End();

        audio.update();
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
        audio.close();
    }
};

auto main(Int argc, Char *argv[]) -> Int
{
    Demo().run();
    return 0;
}
