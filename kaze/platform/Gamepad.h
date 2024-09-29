#pragma once
#ifndef kaze_platform_gamepad_h_
#define kaze_platform_gamepad_h_

#include <kaze/kaze.h>
#include <kaze/math/Vec/Vec2.h>

KAZE_NAMESPACE_BEGIN
    /// Clickable button on a game controller
enum class GamepadBtn : Uint {
    A = 0,       ///< Bottom of ABXY buttons
    B,           ///< Right of ABXY buttons
    X,           ///< Left of ABXY buttons
    Y,           ///< Top of ABXY buttons
    LeftBumper,  ///< Left-top trigger button
    RightBumper, ///< Right-top trigger button
    LeftStick,   ///< Left stick when pressed down
    RightStick,  ///< Right stick when pressed down
    Back,        ///< UI go back
    Start,       ///< UI progress forward
    Guide,       ///< System menu button
    Left,        ///< D-pad left
    Right,       ///< D-pad right
    Up,          ///< D-pad up
    Down,        ///< D-pad down

    Count
};

/// Gradual axis inputs on a game controller
enum class GamepadAxis : Uint {
    LeftX = 0,     ///< Left stick X horizontal axis
    LeftY,         ///< Left stick Y vertical axis
    RightX,        ///< Right stick X horizontal axis
    RightY,        ///< Right stick Y vertical axis
    LeftTrigger,   ///< Left-bottom trigger lever (joyhat)
    RightTrigger,  ///< Right-bottom trigger lever (joyhat)

    Count
};

class Gamepad {
public:
    explicit Gamepad(const Int id) : m_id(id) { }
    [[nodiscard]] Bool isUp(GamepadBtn btn) const noexcept;
    [[nodiscard]] Bool isDown(GamepadBtn btn) const noexcept;
    [[nodiscard]] Bool isJustDown(GamepadBtn btn) const noexcept;
    [[nodiscard]] Bool isJustUp(GamepadBtn btn) const noexcept;
    [[nodiscard]] Float getAxis(GamepadAxis axis, Float deadzone = 0) const noexcept;

    /// Check if 1D axis changed values since last frame
    /// @param axis axis to check
    /// @param deadzone deadzone
    [[nodiscard]] Bool didAxisMove(GamepadAxis axis, Float deadzone = 0) const noexcept;

    /// Whether physical game controller is connected for this controller id
    [[nodiscard]] Bool isConnected() const noexcept;

    /// Get a two-dimensional axis with deadzone.
    /// @param axisX    index of x-axis to get
    /// @param axisY    index of y-axis to get
    /// @param deadzone measured against resultant length of the provided X and Y axes;
    ///                 any length at or beneath this value is clipped to 0.
    [[nodiscard]] Vec2f getAxes(GamepadAxis axisX, GamepadAxis axisY, Float deadzone = 0) const noexcept;
    [[nodiscard]] Bool didAxesMove(GamepadAxis axisX, GamepadAxis axisY, Float deadzone = 0) const noexcept;

    [[nodiscard]] Int id() const { return m_id; }
private:
    Int m_id;
};

KAZE_NAMESPACE_END

#endif
