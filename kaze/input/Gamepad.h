#pragma once
#ifndef kaze_input_gamepad_h_
#define kaze_input_gamepad_h_

#include "GamepadConstants.h"

#include <kaze/kaze.h>
#include <kaze/math/Vec/Vec2.h>


KAZE_NAMESPACE_BEGIN

class Gamepad {
public:
    Gamepad() noexcept : m_id(0) {}
    explicit Gamepad(const Int id) noexcept : m_id(id) { }
    [[nodiscard]] Bool  isUp(GamepadBtn btn) const noexcept;
    [[nodiscard]] Bool  isDown(GamepadBtn btn) const noexcept;
    [[nodiscard]] Bool  isJustDown(GamepadBtn btn) const noexcept;
    [[nodiscard]] Bool  isJustUp(GamepadBtn btn) const noexcept;
    [[nodiscard]] Float getAxis(GamepadAxis axis, Float deadzone = 0) const noexcept;

    /// Check if 1D axis changed values since last frame
    /// @param[in] axis axis to check
    /// @param[in] deadzone deadzone
    /// @returns whether axis moved since last frame
    [[nodiscard]] Bool  getAxisMoved(GamepadAxis axis, Float deadzone = 0) const noexcept;

    /// Whether physical game controller is connected for this controller id
    [[nodiscard]] Bool  isConnected() const noexcept;

    /// Get a two-dimensional axis with deadzone.
    /// @param[in] axisX    index of x-axis to get
    /// @param[in] axisY    index of y-axis to get
    /// @param[in] deadzone measured against resultant length of the provided X and Y axes;
    ///                 any length at or beneath this value is clipped to 0.
    /// @returns a vector containing both axes
    [[nodiscard]] Vec2f getAxes(GamepadAxis axisX, GamepadAxis axisY, Float deadzone = 0) const noexcept;
    [[nodiscard]] Bool  getAxesMoved(GamepadAxis axisX, GamepadAxis axisY, Float deadzone = 0) const noexcept;

    [[nodiscard]] Int id() const noexcept { return m_id; }
    void id(int value) noexcept { m_id = value; }
private:
    Int m_id;
};


KAZE_NAMESPACE_END

#endif // kaze_input_gamepad_h_
