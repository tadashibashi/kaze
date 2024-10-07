#pragma once
#ifndef kaze_app_inputmgr_h_
#define kaze_app_inputmgr_h_

#include <kaze/kaze.h>
#include <kaze/input/GamepadConstants.h>
#include <kaze/input/KeyboardConstants.h>
#include <kaze/input/MouseConstants.h>
#include <kaze/input/InputEvents.h>

#include <kaze/math/Vec/Vec2.h>
#include <kaze/video/WindowConstants.h>

KAZE_NAMESPACE_BEGIN

/// Manages input for a window
class InputMgr {
public:
    InputMgr();
    ~InputMgr();

    auto setWindow(WindowHandle window) noexcept -> InputMgr &;
    auto getWindow() const noexcept -> ConstWindowHandle;
    auto getWindow() noexcept -> WindowHandle;

    auto preProcessEvents() noexcept -> void;
    auto postProcessEvents() noexcept -> void;
    auto processEvent(const KeyboardEvent &e) noexcept -> void;
    auto processEvent(const MouseMotionEvent &e) noexcept -> void;
    auto processEvent(const MouseScrollEvent &e) noexcept -> void;
    auto processEvent(const MouseButtonEvent &e) noexcept -> void;

    // ----- Keyboard -----

    [[nodiscard]]
    auto isDown(Key key) const noexcept -> Bool;

    [[nodiscard]]
    auto isUp(Key key) const noexcept -> Bool;

    [[nodiscard]]
    auto isJustDown(Key key) const noexcept -> Bool;

    [[nodiscard]]
    auto isJustUp(Key key) const noexcept -> Bool;

    // ----- Gamepad -----

    [[nodiscard]]
    auto isDown(Int index, GamepadBtn button) const noexcept -> Bool;

    [[nodiscard]]
    auto isUp(Int index, GamepadBtn button) const noexcept -> Bool;

    [[nodiscard]]
    auto isJustDown(Int index, GamepadBtn key) const noexcept -> Bool;

    [[nodiscard]]
    auto isJustUp(Int index, GamepadBtn key) const noexcept -> Bool;

    [[nodiscard]]
    auto getAxis(Int index, GamepadAxis axis) const noexcept -> Float;

    [[nodiscard]]
    auto getAxes(Int index, GamepadAxis axisX, GamepadAxis axisY, Float deadzone = 0) const noexcept -> Vec2f;

    [[nodiscard]]
    auto getAxisMoved(Int index, GamepadAxis axis, Float deadzone = 0) const noexcept -> Bool;

    /// Check if one or both provided gamepad axes moved since last frame.
    /// @param index
    /// @param axisX
    /// @param axisY
    /// @param deadzone if the length formed by both axes is <= to this value, both values are clamped to 0
    /// @return
    [[nodiscard]]
    auto getAxesMoved(Int index, GamepadAxis axisX, GamepadAxis axisY, Float deadzone = 0) const noexcept -> Bool;

    // ----- Mouse -----

    /// Check if a mouse button is currently pressed down
    /// @param button the button to query
    /// @returns whether the button is currently pressed down
    [[nodiscard]]
    auto isDown(MouseBtn button) const noexcept -> Bool;

    /// Check if a mouse button is currently unpressed
    /// @param button the button to query
    /// @returns whether the button is currently unpressed
    [[nodiscard]]
    auto isUp(MouseBtn button) const noexcept -> Bool;

    /// Check if a mouse button was just pressed
    /// @param button the button to query
    /// @return whether the button has just been pressed this frame.
    [[nodiscard]]
    auto isJustDown(MouseBtn button) const noexcept -> Bool;

    /// Check if a mouse button was just released
    /// @param button the button to query
    /// @returns whether the button has just been released this frame.
    [[nodiscard]]
    auto isJustUp(MouseBtn button) const noexcept -> Bool;

    /// Get mouse position relative to the provided window
    [[nodiscard]]
    auto mouseCoords() const noexcept -> Vec2f;

    /// Get the global mouse position
    [[nodiscard]]
    auto mouseGlobalCoords() const noexcept -> Vec2f;

private:
    struct Impl;
    Impl *m;
};

KAZE_NAMESPACE_END

#endif
