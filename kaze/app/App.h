/// @file App.h
/// Application class
#pragma once
#ifndef kaze_app_app_h_
#define kaze_app_app_h_

#include <kaze/kaze.h>
#include <kaze/core/Action.h>
#include <kaze/input/InputMgr.h>
#include <kaze/math/Vec/Vec2.h>
#include <kaze/video/GraphicsMgr.h>
#include <kaze/video/Window.h>


KAZE_NAMESPACE_BEGIN

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
    /// @returns the time since the app started, in seconds, or a value < 0 of it failed.
    [[nodiscard]]
    auto time() const noexcept -> Double;

    /// Get the input manager
    [[nodiscard]]
    auto input() const noexcept -> const InputMgr &;

    /// Get the window object
    [[nodiscard]]
    auto window() const noexcept -> const Window &;

    /// Get the window object
    [[nodiscard]]
    auto window() noexcept -> Window &;

    [[nodiscard]]
    auto graphics() const noexcept -> const GraphicsMgr &;

    [[nodiscard]]
    auto graphics() noexcept -> GraphicsMgr &;

    /// Quit the application after this frame is over
    auto quit() -> void;
private:
    // ----- Overridable callbacks -----
    virtual auto init() -> Bool { return KAZE_TRUE; }
    virtual auto update() -> void {}
    virtual auto draw() -> void {}
    virtual auto close() -> void {}
    virtual auto processWindowEvent(const WindowEvent &e, Double timestamp) -> void;

    // ----- Private implementation -----
    auto preInit() -> Bool;
    auto pollEvents() -> void;
    auto runOneFrame() -> void;

    struct Impl;
    Impl *m;
};

KAZE_NAMESPACE_END

#endif // kaze_app_app_h_
