#include "App.h"
#include "AppPluginMgr.h"

#include <chrono>
#include <kaze/core/input/CursorMgr.h>
#include <kaze/core/platform/backend/backend.h>
#include <kaze/core/platform/BackendInitGuard.h>
#include <kaze/core/platform/defines.h>

#include <kaze/tk/FramerateCounter.h>

#include <thread>

#if KAZE_PLATFORM_EMSCRIPTEN
#   include <emscripten/emscripten.h>
#endif

KAZE_TK_NAMESPACE_BEGIN

struct App::Impl
{
    explicit Impl(const AppInit &config, App *app) : config(config), app(app)
    {
        targetSPF = 1.0 / config.targetFPS;
    }

    Bool isRunning{};
    Double lastTime{}, deltaTime{};
    InputMgr input{};
    GraphicsMgr graphics{};
    AppPluginMgr plugins{};
    CursorMgr cursors{};

    Double targetSPF{};

    FramerateCounter framerate{{
        .samples = 60
    }};

    AppInit config;
    Window window;

    Bool wasResized{};
    Vec2i newSize{};
    float lastResizeTime{};
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
    m->framerate.reset();

#if KAZE_PLATFORM_EMSCRIPTEN
    emscripten_set_main_loop_arg([](void *userptr) {
        auto app = static_cast<App *>(userptr);
        app->oneTick();
    }, this, -1, 1);
#else
    m->isRunning = true;
    do {
        oneTick();
    } while (m->isRunning);
#endif
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

auto App::deltaTime() const noexcept -> Double
{
    return m->deltaTime;
}

auto App::time() const noexcept -> Double
{
    double time = -1.0;
    backend::getTime(&time);

    return static_cast<Double>(time);
}

auto App::fps() const noexcept -> Double
{
    return m->framerate.getAverageFps();
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

auto App::removePlugin(const AppPlugin &plugin) -> Bool
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
            if (impl->plugins.gpadAxisFilter(e, timestamp, impl->app))
            {
                impl->input.processEvent(e, timestamp);
                impl->plugins.gpadAxisEvent(e, timestamp, impl->app);
            }
        },
        .gamepadButtonCallback = [] (const GamepadButtonEvent &e, const Double timestamp, void *userdata) {
            const auto impl = static_cast<Impl *>(userdata);
            if (impl->plugins.gpadButtonFilter(e, timestamp, impl->app))
            {
                impl->input.processEvent(e, timestamp);
                impl->plugins.gpadButtonEvent(e, timestamp, impl->app);
            }
        },
        .gamepadConnectCallback = [] (const GamepadConnectEvent &e, const Double timestamp, void *userdata) {
            const auto impl = static_cast<Impl *>(userdata);
            if (impl->plugins.gpadConnectFilter(e, timestamp, impl->app))
            {
                impl->input.processEvent(e, timestamp);
                impl->plugins.gpadConnectEvent(e, timestamp, impl->app);
            }
        },
        .keyCallback = [] (const KeyboardEvent &e, const Double timestamp, void *userdata) {
            const auto impl = static_cast<Impl *>(userdata);
            if (impl->plugins.keyFilter(e, timestamp, impl->app))
            {
                impl->input.processEvent(e, timestamp);
                impl->plugins.keyEvent(e, timestamp, impl->app);
            }
        },
        .mouseButtonCallback = [] (const MouseButtonEvent &e, const Double timestamp, void *userdata) {
            const auto impl = static_cast<Impl *>(userdata);
            if (impl->plugins.mbuttonFilter(e, timestamp, impl->app))
            {
                impl->input.processEvent(e, timestamp);
                impl->plugins.mbuttonEvent(e, timestamp, impl->app);
            }
        },
        .mouseMotionCallback = [] (const MouseMotionEvent &e, const Double timestamp, void *userdata) {
            const auto impl = static_cast<Impl *>(userdata);
            if (impl->plugins.mmotionFilter(e, timestamp, impl->app))
            {
                impl->input.processEvent(e, timestamp);
                impl->plugins.mmotionEvent(e, timestamp, impl->app);
            }
        },
        .mouseScrollCallback = [] (const MouseScrollEvent &e, const Double timestamp, void *userdata) {
            const auto impl = static_cast<Impl *>(userdata);
            if (impl->plugins.mscrollFilter(e, timestamp, impl->app))
            {
                impl->input.processEvent(e, timestamp);
                impl->plugins.mscrollEvent(e, timestamp, impl->app);
            }
        },
        .textInputCallback = [] (const TextInputEvent &e, const Double timestamp, void *userdata) {
            const auto impl = static_cast<Impl *>(userdata);
            if (impl->plugins.textInputFilter(e, timestamp, impl->app))
            {
                impl->input.processEvent(e, timestamp);
                impl->plugins.textInputEvent(e, timestamp, impl->app);
            }
        },
        .windowCallback = [] (const WindowEvent &e, const Double timestamp, void *userdata) {
            const auto impl = static_cast<Impl *>(userdata);
            if (impl->plugins.windowFilter(e, timestamp, impl->app))
            {
                impl->app->processWindowEvent(e, timestamp);
                impl->plugins.windowEvent(e, timestamp, impl->app);
            }
        }
    });

    m->input.setWindow(window().getHandle());
    m->cursors.setWindow(window().getHandle());
    return KAZE_TRUE;
}

 /// Standard window behavior
auto App::processWindowEvent(const WindowEvent &e, const Double timestamp) -> void
{
    if (m->window.getHandle() != e.window)
        return;

    switch (e.type)
    {
    case WindowEvent::Closed:
        {
            m->isRunning = false;
        } break;

    case WindowEvent::ResizedFramebuffer:
        {
            m->graphics.reset(e.data0, e.data1);
            frame();
        #if KAZE_PLATFORM_APPLE
            m->graphics.renderFrame();
        #endif
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

auto App::doRender() -> void
{
    m->plugins.preRender(this);
    render();
    m->plugins.postRender.reverseInvoke(this);

    m->plugins.preRenderUI(this);
    renderUI();
    m->plugins.postRenderUI.reverseInvoke(this);
}

auto App::oneTick() -> void
{
    using Clock = std::chrono::high_resolution_clock;

    const auto now = Clock::now();

    double startTickTime = 0;
    backend::getTime(&startTickTime);
    m->deltaTime = startTickTime - m->lastTime;
    m->lastTime = startTickTime;
    m->framerate.frame();

    pollEvents();
    frame();

    const auto end = Clock::now();
    const std::chrono::duration<double> elapsed = end - now;
    const auto targetSPF = m->targetSPF;
    const auto endFrame = std::chrono::duration<double>(targetSPF) + now;
    if (elapsed.count() < targetSPF && m->framerate.getAverageSpf() < targetSPF)
    {
        std::this_thread::sleep_until(endFrame);
    }

    while(Clock::now() < endFrame);
}

auto App::doUpdate() -> void
{
    m->plugins.preUpdate(this);
    update();
    m->plugins.postUpdate.reverseInvoke(this);
}

auto App::frame() -> void
{
    m->plugins.preFrame(this);
    doUpdate();
    doRender();
    m->graphics.frame();
    m->plugins.postFrame.reverseInvoke(this);

    if (m->wasResized)
        m->graphics.reset(m->newSize.x, m->newSize.y);
}

KAZE_TK_NAMESPACE_END
