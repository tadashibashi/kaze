#pragma once
#ifndef kaze_core_input_inputmgr_h_
#define kaze_core_input_inputmgr_h_

#include <kaze/core/lib.h>
#include <kaze/core/Action.h>
#include <kaze/core/input/GamepadConstants.h>
#include <kaze/core/input/KeyboardConstants.h>
#include <kaze/core/input/MouseConstants.h>
#include <kaze/core/input/InputEvents.h>

#include <kaze/core/math/Vec/Vec2.h>
#include <kaze/core/video/WindowConstants.h>

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
    auto processEvent(const KeyboardEvent &e, Double timestamp) noexcept -> void;
    auto processEvent(const MouseMotionEvent &e, Double timestamp) noexcept -> void;
    auto processEvent(const MouseScrollEvent &e, Double timestamp) noexcept -> void;
    auto processEvent(const MouseButtonEvent &e, Double timestamp) noexcept -> void;
    auto processEvent(const GamepadAxisEvent &e, Double timestamp) noexcept -> void;
    auto processEvent(const GamepadButtonEvent &e, Double timestamp) noexcept -> void;
    auto processEvent(const GamepadConnectEvent &e, Double timestamp) noexcept -> void;

    Action<const KeyboardEvent &, Double> onKeyboard;              ///< occurs on a keyup or keydown; 2nd argument is seconds since app start
    Action<const MouseMotionEvent &, Double> onMouseMotion;        ///< occurs on mouse move; 2nd argument is seconds since app start
    Action<const MouseScrollEvent &, Double> onMouseScroll;        ///<
    Action<const MouseButtonEvent &, Double> onMouseButton;
    Action<const GamepadButtonEvent &, Double> onGamepadButton;
    Action<const GamepadAxisEvent &, Double> onGamepadAxis;
    Action<const GamepadConnectEvent &, Double> onGamepadConnect;

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
    /// \param[in] index      index of the gamepad slot (0-15)
    /// \param[in] axisX      x-axis to query
    /// \param[in] axisY      y-axis to query
    /// \param[in] deadzone   the length threshhold beneath which both axes' combined length will be measured
    ///                       against where if <= `deadzone`, both axis values will be forced to 0.
    /// \returns a vector containing both axes.
    [[nodiscard]]
    auto getAxesMoved(Int index, GamepadAxis axisX, GamepadAxis axisY, Float deadzone = 0) const noexcept -> Bool;

    // ----- Mouse -----

    /// Check if a mouse button is currently pressed down
    /// \param[in] button   the button to query
    /// \returns whether the button is currently pressed down
    [[nodiscard]]
    auto isDown(MouseBtn button) const noexcept -> Bool;

    /// Check if a mouse button is currently unpressed
    /// \param[in] button   the button to query
    /// \returns whether the button is currently unpressed
    [[nodiscard]]
    auto isUp(MouseBtn button) const noexcept -> Bool;

    /// Check if a mouse button was just pressed
    /// \param[in] button   the button to query
    /// @return whether the button has just been pressed this frame.
    [[nodiscard]]
    auto isJustDown(MouseBtn button) const noexcept -> Bool;

    /// Check if a mouse button was just released
    /// \param[in] button   the button to query
    /// \returns whether the button has just been released this frame.
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

#endif // kaze_core_input_inputmgr_h_
