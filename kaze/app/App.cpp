#include "App.h"

#include "BackendInitGuard.h"

#include <kaze/platform/backend.h>


KAZE_NAMESPACE_BEGIN

struct App::Impl
{
    explicit Impl(const AppInit &config, App *app) : config(config), app(app) { }

    Bool isRunning{};
    Double lastTime{}, deltaTime{};
    InputMgr input{};
    GraphicsMgr graphics{};

    AppInit config;
    Window window;
    App *app;
};

App::App(const AppInit &config) : m(new Impl(config, this))
{
}

void App::run()
{
    BackendInitGuard initGuard{};

    if ( !preInit() )
        return;

    if ( !init() )
        return;

    m->isRunning = true;
    do
    {
        runOneFrame();
    } while (m->isRunning);

    close();
    m->graphics.close();
    m->window.close();
}

Double App::deltaTime() const noexcept
{
    return m->deltaTime;
}

Double App::time() const noexcept
{
    double time = -1.0;
    backend::getTime(&time);

    return static_cast<Double>(time);
}

auto App::input() const noexcept -> const InputMgr &
{
    return m->input;
}

auto App::window() const noexcept -> const Window &
{
    return m->window;
}

auto App::window() noexcept -> Window &
{
    return m->window;
}

auto App::graphics() const noexcept -> const GraphicsMgr &
{
    return m->graphics;
}

auto App::graphics() noexcept -> GraphicsMgr &
{
    return m->graphics;
}

auto App::quit() -> void
{
    m->isRunning = false;
}

auto App::preInit() -> Bool
{
    if ( !m->window.open(m->config.title.c_str(), m->config.size.x, m->config.size.y, m->config.flags) )
        return KAZE_FALSE;

    if ( !m->graphics.init(m->window.getPtr()) )
        return KAZE_FALSE;

    backend::setCallbacks(PlatformCallbacks {
        .userptr = m,
        .gamepadAxisCallback = [] (const GamepadAxisEvent &e, Double timestamp, void *userdata) {
            auto impl = static_cast<Impl *>(userdata);
            impl->input.processEvent(e, timestamp);
        },
        .gamepadButtonCallback = [] (const GamepadButtonEvent &e, Double timestamp, void *userdata) {
            auto impl = static_cast<Impl *>(userdata);
            impl->input.processEvent(e, timestamp);
        },
        .gamepadConnectCallback = [] (const GamepadConnectEvent &e, Double timestamp, void *userdata) {
            auto impl = static_cast<Impl *>(userdata);
            impl->input.processEvent(e, timestamp);
        },
        .mouseButtonCallback = [] (const MouseButtonEvent &e, Double timestamp, void *userdata) {
            auto impl = static_cast<Impl *>(userdata);
            impl->input.processEvent(e, timestamp);
        },
        .mouseMotionCallback = [] (const MouseMotionEvent &e, Double timestamp, void *userdata) {
            auto impl = static_cast<Impl *>(userdata);
            impl->input.processEvent(e, timestamp);
        },
        .mouseScrollCallback = [] (const MouseScrollEvent &e, Double timestamp, void *userdata) {
            auto impl = static_cast<Impl *>(userdata);
            impl->input.processEvent(e, timestamp);
        },
        .keyCallback = [] (const KeyboardEvent &e, Double timestamp, void *userdata) {
            auto impl = static_cast<Impl *>(userdata);
            impl->input.processEvent(e, timestamp);
        },
        .windowCallback = [] (const WindowEvent &e, Double timestamp, void *userdata) {
            auto impl = static_cast<Impl *>(userdata);
            impl->app->processWindowEvent(e, timestamp);
        }
    });
    return KAZE_TRUE;
}

 /// Standard window behavior
auto App::processWindowEvent(const WindowEvent &e, Double timestamp) -> void
{
    switch (e.type)
    {
    case WindowEvent::Closed:
        if (m->window.getPtr() == e.window)
            m->isRunning = false;
        break;
    case WindowEvent::ResizedFramebuffer:
    {
        if (m->window.getPtr() == e.window)
        {
            m->graphics.reset(e.data0, e.data1);
            draw();
            m->graphics.frame();
            m->graphics.renderFrame();
        }
    } break;
    default:
        break;
    }
}

void App::pollEvents()
{
    m->input.preProcessEvents();
    backend::pollEvents();
    m->input.postProcessEvents();
}

void App::runOneFrame()
{
    double currentTime = 0;
    backend::getTime(&currentTime);
    m->deltaTime = currentTime - m->lastTime;

    pollEvents();
    update();
    draw();
    m->graphics.frame();
    m->lastTime = currentTime;
}

KAZE_NAMESPACE_END
