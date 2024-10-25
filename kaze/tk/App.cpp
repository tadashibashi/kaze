#include "App.h"
#include "AppPluginMgr.h"

#include <kaze/core/input/CursorMgr.h>
#include <kaze/core/platform/backend/backend.h>
#include <kaze/core/platform/BackendInitGuard.h>

KAZE_TK_NAMESPACE_BEGIN

struct App::Impl
{
    explicit Impl(const AppInit &config, App *app) : config(config), app(app) { }

    Bool isRunning{};
    Double lastTime{}, deltaTime{};
    InputMgr input{};
    GraphicsMgr graphics{};
    AppPluginMgr plugins{};
    CursorMgr cursors{};

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

    m->plugins.init(this);

    m->isRunning = true;
    do {
        runOneFrame();
    } while (m->isRunning);

    close();
    m->plugins.close(this);
    postClose();
}

auto App::postClose() -> void
{
    m->cursors.clear();
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

auto App::cursors() const noexcept -> const CursorMgr &
{
    return m->cursors;
}

auto App::cursors() noexcept -> CursorMgr &
{
    return m->cursors;
}

auto App::addPlugin(const AppPlugin &plugin) -> App &
{
    m->plugins.addPlugin(plugin);
    return *this;
}

auto App::removePlugin(const StringView name) -> Bool
{
    return m->plugins.removePlugin(name);
}
auto kaze::tk::App::removePlugin(const AppPlugin &plugin) -> Bool
{
    return removePlugin(plugin.name());
}

auto App::quit() -> void
{
    m->isRunning = false;
}

auto App::preInit() -> Bool
{
    if ( !m->window.open(m->config.title.c_str(), m->config.size.x, m->config.size.y, m->config.flags | WindowInit::Hidden) )
        return KAZE_FALSE;

    if ( !m->graphics.init({
        .window = m->window.getHandle(),
        .clearColor = m->config.clearColor,
        .maxTransientVBufferSize = m->config.maxTransientVBufferSize,
        .maxTransientIBufferSize = m->config.maxTransientIBufferSize,
    }) )
    {
        return KAZE_FALSE;
    }

    m->window.setHidden(false);

    backend::setCallbacks({
        .userptr = m,
        .gamepadAxisCallback = [] (const GamepadAxisEvent &e, const Double timestamp, void *userdata) {
            const auto impl = static_cast<Impl *>(userdata);
            impl->input.processEvent(e, timestamp);
            impl->plugins.gamepadAxisEvent(e, impl->app);
        },
        .gamepadButtonCallback = [] (const GamepadButtonEvent &e, const Double timestamp, void *userdata) {
            const auto impl = static_cast<Impl *>(userdata);
            impl->input.processEvent(e, timestamp);
            impl->plugins.gamepadButtonEvent(e, impl->app);
        },
        .gamepadConnectCallback = [] (const GamepadConnectEvent &e, const Double timestamp, void *userdata) {
            const auto impl = static_cast<Impl *>(userdata);
            impl->input.processEvent(e, timestamp);
            impl->plugins.gamepadConnectEvent(e, impl->app);
        },
        .keyCallback = [] (const KeyboardEvent &e, const Double timestamp, void *userdata) {
            const auto impl = static_cast<Impl *>(userdata);
            impl->input.processEvent(e, timestamp);
            impl->plugins.keyboardEvent(e, impl->app);
        },
        .mouseButtonCallback = [] (const MouseButtonEvent &e, const Double timestamp, void *userdata) {
            const auto impl = static_cast<Impl *>(userdata);
            impl->input.processEvent(e, timestamp);
            impl->plugins.mouseButtonEvent(e, impl->app);
        },
        .mouseMotionCallback = [] (const MouseMotionEvent &e, const Double timestamp, void *userdata) {
            const auto impl = static_cast<Impl *>(userdata);
            impl->input.processEvent(e, timestamp);
            impl->plugins.mouseMotionEvent(e, impl->app);
        },
        .mouseScrollCallback = [] (const MouseScrollEvent &e, const Double timestamp, void *userdata) {
            const auto impl = static_cast<Impl *>(userdata);
            impl->input.processEvent(e, timestamp);
            impl->plugins.mouseScrollEvent(e, impl->app);
        },
        .textInputCallback = [] (const TextInputEvent &e, const Double timestamp, void *userdata) {
            const auto impl = static_cast<Impl *>(userdata);
            impl->input.processEvent(e, timestamp);
            impl->plugins.textInputEvent(e, impl->app);
        },
        .windowCallback = [] (const WindowEvent &e, const Double timestamp, void *userdata) {
            const auto impl = static_cast<Impl *>(userdata);

            impl->plugins.windowEvent(e, impl->app);
            impl->app->processWindowEvent(e, timestamp);
        }
    });

    m->input.setWindow(window().getHandle());
    m->cursors.setWindow(window().getHandle());
    return KAZE_TRUE;
}

 /// Standard window behavior
auto App::processWindowEvent(const WindowEvent &e, const Double timestamp) -> void
{
    switch (e.type)
    {
    case WindowEvent::Closed:
        if (m->window.getHandle() == e.window)
            m->isRunning = false;
        break;
    case WindowEvent::ResizedFramebuffer:
    {
        if (m->window.getHandle() == e.window)
        {
            m->graphics.reset(e.data0, e.data1);
            renderAll();
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

auto App::renderAll() -> void
{
    m->plugins.preRender(this);
    render();
    m->plugins.postRender.reverseInvoke(this);

    m->plugins.preRenderUI(this);
    renderUI();
    m->plugins.postRenderUI.reverseInvoke(this);
}

void App::runOneFrame()
{
    double currentTime = 0;
    backend::getTime(&currentTime);
    m->deltaTime = currentTime - m->lastTime;

    pollEvents();

    m->plugins.preFrame(this);
    m->plugins.preUpdate(this);
    update();
    m->plugins.postUpdate.reverseInvoke(this);

    renderAll();

    m->graphics.frame();
    m->plugins.postFrame.reverseInvoke(this);

    m->lastTime = currentTime;
}

KAZE_TK_NAMESPACE_END
