#include <kaze/kaze.h>
#include <kaze/debug.h>
#include <kaze/app/App.h>

USING_KAZE_NAMESPACE;

class Demo : public App {
public:
    Demo() : App({
        .title = "App Demo",
        .size = {200, 200},
        .flags = WindowInit::Resizable
    }) { }
private:
    auto init() -> Bool override {
        KAZE_LOG("Application initialized");
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
    }

    auto draw() -> void override {

    }

    auto close() -> void override {
        KAZE_LOG("Application closed");
    }
};

int main()
{
    Demo().run();
    return 0;
}
