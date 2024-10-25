
#include <kaze/core/debug.h>
#include <kaze/core/input/Gamepad.h>
#include <kaze/core/video/Window.h>

#include <kaze/core/platform/backend/backend.h>

USING_KAZE_NAMESPACE;

struct AppData
{
    bool isRunning{};
    bool logMousePos{};
    Window window{};
};

int main(int argc, const char *argv[])
{
    AppData app;
    if (!backend::init())
    {
        KAZE_ERR("Failed to init backend");
        return -1;
    }

    backend::setCallbacks({
        .userptr = &app,
        .fileDropCallback = [](const FileDropEvent &e, Double timestamp, void *userptr) {
            KAZE_LOG("File dropped: \"{}\", at: {}, {}", e.path, e.position.x, e.position.y);
        },
        .gamepadAxisCallback = [](const GamepadAxisEvent &e, Double timestamp, void *userptr) {
            KAZE_LOG("Controller {}, axis {} moved: {}", e.controllerIndex, (int)e.axis, e.value);
        },
        .gamepadButtonCallback = [](const GamepadButtonEvent &e, Double timestamp, void *userptr) {
            KAZE_LOG("Controller button {} was {}", (int)e.button, e.type == GamepadButtonEvent::Down ? "pressed" : "released");
        },
        .gamepadConnectCallback = [](const GamepadConnectEvent &e, auto timestamp, auto userptr) {
            if (e.type == GamepadConnectEvent::Connected)
            {
                KAZE_LOG("Gamepad connected: {}", e.id);
            }
            else
            {
                KAZE_LOG("Gamepad disconnected: {}", e.id);
            }
        },
        .keyCallback = [](const KeyboardEvent &e, Double timestamp, void *userptr) {
            const auto app = static_cast<AppData *>(userptr);
            if (e.type == KeyboardEvent::Down && !e.isRepeat)
            {
                KAZE_LOG("Key pressed: {}, at time: {}", static_cast<Uint>(e.key), timestamp);

                switch(e.key)
                {
                    case Key::B:
                        app->window.setBordered(!app->window.isBordered());
                        break;
                case Key::F:
                        app->window.setFullscreenMode(FullscreenMode::Native);
                        app->window.setFullscreen(!app->window.isFullscreen());
                        break;
                case Key::D:
                        app->window.setFullscreenMode(FullscreenMode::Desktop);
                        app->window.setFullscreen(!app->window.isFullscreen());
                        break;
                    case Key::M:
                        if (app->window.isMinimized())
                            app->window.restore();
                        else
                            app->window.minimize();
                        break;
                    case Key::N:
                        if (app->window.isMaximized())
                            app->window.restore();
                        else
                            app->window.maximize();
                        break;
                    case Key::P:
                        app->logMousePos = !app->logMousePos;
                        if (app->logMousePos)
                            KAZE_LOG("Activated mouse position logging.");
                        else
                            KAZE_LOG("Deactivated mouse position logging.");
                        break;
                    default: break;
                }
            }
        },
        .mouseButtonCallback = [](const MouseButtonEvent &e, Double timestamp, void *userptr) {
            const auto app = static_cast<AppData *>(userptr);
            if (e.type == MouseButtonEvent::Down)
                KAZE_LOG("Mouse button pressed: {}", static_cast<Uint>(e.button));
        },
        .mouseMotionCallback = [](const MouseMotionEvent &e, Double timestamp, void *userptr) {
            const auto app = static_cast<AppData *>(userptr);
            if (app->logMousePos)
                KAZE_LOG("Mouse moved: {{{}, {}}}", e.position.x, e.position.y);
        },
        .mouseScrollCallback = [](const MouseScrollEvent &e, Double timestamp, void *userptr) {
            KAZE_LOG("Scrolled: {}, {}", e.offset.x, e.offset.y);
        },
        .windowCallback = [](const WindowEvent &e, Double timestamp, void *userptr) {
            const auto app = static_cast<AppData *>(userptr);
            switch(e.type)
            {
            case WindowEvent::Closed:
                {
                    app->isRunning = false;
                    KAZE_LOG("Window closed");
                } break;

            case WindowEvent::Resized:
                {
                    KAZE_LOG("Window resized to: {}, {}", e.data0, e.data1);
                } break;

            case WindowEvent::ResizedFramebuffer:
                {
                    KAZE_LOG("Framebuffer resized to: {}, {}", e.data0, e.data1);
                } break;

            case WindowEvent::Moved:
                {
                    KAZE_LOG("Window moved to: {}, {}", e.data0, e.data1);
                } break;

            case WindowEvent::FocusGained:
                {
                    KAZE_LOG("Window focus gained");
                } break;

            case WindowEvent::FocusLost:
                {
                    KAZE_LOG("Window focus lost");
                } break;

            case WindowEvent::Maximized:
                {
                    KAZE_LOG("Window maximized");
                } break;

            case WindowEvent::Minimized:
                {
                    KAZE_LOG("Window minimized");
                } break;

            case WindowEvent::Restored:
                {
                    KAZE_LOG("Window restored");
                } break;

            case WindowEvent::MouseEntered:
                {
                    KAZE_LOG("Mouse entered");
                } break;

            case WindowEvent::MouseExited:
                {
                    KAZE_LOG("Mouse exited");
                } break;

            default:
                KAZE_WARN("Warning: unknown window event was passed.");
                break;
            }
        },
    });

    const Gamepad gamepad(0);

    if (!app.window.open("My Game", 480, 480, WindowInit::Resizable))
    {
        KAZE_ERR("Window failed to open!");
        return -1;
    }

    if (app.window.isOpen())
    {
        KAZE_LOG("Window is open!");
    }

    app.isRunning = true;
    while (app.isRunning)
    {
        if (gamepad.isConnected())
        {
            if (gamepad.isJustDown(GamepadBtn::A))
            {
                KAZE_LOG("Pressed A!");
            }

            if (gamepad.getAxesMoved(GamepadAxis::LeftX, GamepadAxis::LeftY, .2f))
            {
                const auto axes = gamepad.getAxes(GamepadAxis::LeftX, GamepadAxis::LeftY, .2f);
                KAZE_LOG("Left Stick moved to: {}, {}", axes.x, axes.y);
            }
        }

        backend::pollEvents();
    }

    app.window.close();
    backend::shutdown();
    return 0;
}
