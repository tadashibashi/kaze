/// \file App.h
/// Application class
#pragma once
#ifndef kaze_tk_app_h_
#define kaze_tk_app_h_

#include <kaze/tk/lib.h>
#include <kaze/tk/AppPlugin.h>
#include <kaze/core/Action.h>
#include <kaze/core/input/InputMgr.h>
#include <kaze/core/math/Vec/Vec2.h>
#include <kaze/core/video/GraphicsMgr.h>
#include <kaze/core/video/Window.h>

KAZE_TK_NAMESPACE_BEGIN

/// Application init object
struct AppInit {
    String title           {String("")}; ///< App title bar text
    Vec2i size             {640, 480};   ///< Initial window size
    Color clearColor       {100, 154, 206, 235}; ///< Default background clear color
    WindowInit::Flags flags{};           ///< Initial window attribute flags, can be or'd together
    Size maxTransientVBufferSize {4000 * 1024};
    Size maxTransientIBufferSize {6000 * 1024};
};

/// The App class provides a convenient framework, an optional implementation of encapsulating backend functionality.
class App
{
public:
    explicit App(const AppInit &config);
    virtual ~App() = default;

    // no copy
    KAZE_NO_COPY(App);

    Action<const WindowEvent &, Double> onWindowEvent;

    auto run() -> void;

    /// Time since the last frame
    [[nodiscard]]
    auto deltaTime() const noexcept -> Double;

    /// Time since the application started
    /// \returns the time since the app started, in seconds, or a value < 0 of it failed.
    [[nodiscard]]
    auto time() const noexcept -> Double;

    /// \returns the app input manager
    [[nodiscard]]
    auto input() const noexcept -> const InputMgr &;

    /// \returns the app window object
    [[nodiscard]]
    auto window() const noexcept -> const Window &;

    /// \returns the app window object
    [[nodiscard]]
    auto window() noexcept -> Window &;

    /// \returns GraphicsMgr for this application
    [[nodiscard]]
    auto graphics() const noexcept -> const GraphicsMgr &;

    /// \returns GraphicsMgr for this application
    [[nodiscard]]
    auto graphics() noexcept -> GraphicsMgr &;

    [[nodiscard]]
    auto cursors() const noexcept -> const CursorMgr &;
    [[nodiscard]]
    auto cursors() noexcept -> CursorMgr &;

    /// Inject plugin to the app
    /// \param[in] name  plugin id
    /// \returns this object
    auto addPlugin(const AppPlugin &plugin) -> App &;

    /// Remove plugin from the app by id
    auto removePlugin(StringView name) -> Bool;

    /// Remove plugin from the app by AppPlugin
    auto removePlugin(const AppPlugin &plugin) -> Bool;

    /// Quit the application after this frame is over
    auto quit() -> void;
private:
    // ----- Overridable callbacks -----
    virtual auto init() -> Bool { return KAZE_TRUE; }
    virtual auto update() -> void {}
    virtual auto render() -> void {}
    virtual auto renderUI() -> void {}
    virtual auto close() -> void {}
    virtual auto processWindowEvent(const WindowEvent &e, Double timestamp) -> void;

    // ----- Private implementation -----
    auto preInit() -> Bool;
    auto pollEvents() -> void;
    auto runOneFrame() -> void;
    auto postClose() -> void;

    struct Impl;
    Impl *m;
};

KAZE_TK_NAMESPACE_END

#endif // kaze_tk_app_h_
