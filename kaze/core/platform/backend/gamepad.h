#pragma once
#ifndef kaze_core_platform_backend_gamepad_h_
#define kaze_core_platform_backend_gamepad_h_

#include <kaze/core/lib.h>
#include <kaze/core/input/GamepadConstants.h>

KAZE_NAMESPACE_BEGIN

namespace backend::gamepad {
    /// Get whether a gamepad is connected in a slot (0 - 15)
    /// \param[in]  index         slot index: (0-15) are valid
    /// \param[out] outConnected  retrieves whether the gamepad is connected
    /// \returns whether the retrieval was successful.
    auto isConnected(int index, bool *outConnected) noexcept -> bool;

    /// Get whether a gamepad button is currently pressed down
    /// \param[in]  index          slot index: (0-15) are valid
    /// \param[in]  button         button index to check
    /// \param[out] outDown        retrieves whether the button is currently down;
    ///                            if a controller is not connected in this slot, `false` will always be set.
    /// \returns whether the retrieval was successful.
    auto isDown(int index, GamepadBtn button, bool *outDown) noexcept -> bool;

    /// Get whether a gamepad button was just pressed down this frame.
    /// \param[in]  index        slot index: (0-15) are valid
    /// \param[in]  button       button index to check
    /// \param[out] outJustDown  retrieves whether the button was just pressed down this frame;
    ///                          if a controller is not connected in this slot, `false` will always be set.
    /// \returns whether the retrieval was successful.
    auto isJustDown(int index, GamepadBtn button, bool *outJustDown) noexcept -> bool;

    /// Get whether a gamepad button was just released this frame
    /// \param[in]  index        slot index: (0-15) are valid
    /// \param[in]  button       button index to check
    /// \param[out] outJustUp    retrieves whether the button was just released this frame;
    ///                          if a controller is not connected in this slot, `false` will always be set.
    /// \returns whether the retrieval was successful.
    auto isJustUp(int index, GamepadBtn button, bool *outJustUp) noexcept -> bool;

    /// Get a gamepad axis value
    /// \param[in]  index       slot index: (0-15) are valid
    /// \param[in]  axis        axis index to check
    /// \param[out] outValue    retrieves the current axis value;
    ///                          if a controller is not connected in this slot, `0` will always be set.
    /// \returns whether the retrieval was successful.
    auto getAxis(int index, GamepadAxis axis, float *outValue) noexcept -> bool;

    /// Get whether an axis has moved/changed values since last frame
    /// \param[in]  index       slot index: (0-15) are valid
    /// \param[in]  axis        axis index to check
    /// \param[in]  deadzone    any absolute value <= to this threshold are forced to 0
    /// \param[out] outMoved    retrieves whether the axis has changed since last frame;
    ///                         if a controller is not connected in this slot, `0` will always be set.
    /// \returns whether the retrieval was successful.
    auto getAxisMoved(int index, GamepadAxis axis, float deadzone, bool *outMoved) noexcept -> bool;

    /// Check if either of the provided axes moved since last frame
    /// \param[in]  index     controller slot index (0-15)
    /// \param[in]  axisX     x axis to check
    /// \param[in]  axisY     y axis to check
    /// \param[in]  deadzone  any absolute value of the magnitude formed between both axes <= to this threshold are
    ///                       forced to zero.
    /// \param[out] outMoved  retrieves whether the axis has changed since last frame;
    ///                       if a controller is not connected in this slot, `0` will always be set.
    /// \returns whether the retrieval was successful.
    auto getAxesMoved(int index, GamepadAxis axisX, GamepadAxis axisY, float deadzone, bool *outMoved) noexcept -> bool;
}

KAZE_NAMESPACE_END

#endif // kaze_core_platform_backend_gamepad_h_
