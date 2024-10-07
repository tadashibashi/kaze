#include "App.h"

#include "BackendInitGuard.h"

#include <kaze/platform/backend.h>
#include <kaze/video/Window.h>

KAZE_NAMESPACE_BEGIN

struct App::Impl
{
    explicit Impl(const AppInit &config) : config(config) { }

    Bool isRunning{};
    Double lastTime{}, deltaTime{};
    InputMgr input{};

    AppInit config;
    Window window;
};

App::App(const AppInit &config) : m(new Impl(config))
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
    while (m->isRunning)
    {
        runOneFrame();
    }

    close();
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

auto App::quit() -> void
{
    m->isRunning = false;
}

auto App::preInit() -> Bool
{
    if ( !m->window.open(m->config.title.c_str(), m->config.size.x, m->config.size.y, m->config.flags) )
        return false;

    backend::setCallbacks(PlatformCallbacks {
        .userptr = m,
        .mouseButtonCallback = [] (const MouseButtonEvent &e, Double timestamp, void *userdata) {
            auto impl = static_cast<Impl *>(userdata);
            impl->input.processEvent(e);
        },
        .mouseMotionCallback = [] (const MouseMotionEvent &e, Double timestamp, void *userdata) {
            auto impl = static_cast<Impl *>(userdata);
            impl->input.processEvent(e);
        },
        .mouseScrollCallback = [] (const MouseScrollEvent &e, Double timestamp, void *userdata) {
            auto impl = static_cast<Impl *>(userdata);
            impl->input.processEvent(e);
        },
        .keyCallback = [] (const KeyboardEvent &e, Double timestamp, void *userdata) {
            auto impl = static_cast<Impl *>(userdata);
            impl->input.processEvent(e);
        },
        .windowCallback = [] (const WindowEvent &e, Double timestamp, void *userdata) {
            auto impl = static_cast<Impl *>(userdata);
            switch (e.type)
            {
            case WindowEvent::Closed:
                impl->isRunning = false;
                break;
            default:
                break;
            }
        }
    });
    return true;
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

    m->lastTime = currentTime;
}

KAZE_NAMESPACE_END
