#pragma once
#ifndef kaze_input_gamepadconstants_h_
#define kaze_input_gamepadconstants_h_
#include <kaze/kaze.h>

KAZE_NAMESPACE_BEGIN

constexpr int GamepadMaxSlots = 16;

/// Clickable button on a game controller
enum class GamepadBtn {
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
enum class GamepadAxis {
    LeftX = 0,     ///< Left stick X horizontal axis
    LeftY,         ///< Left stick Y vertical axis
    RightX,        ///< Right stick X horizontal axis
    RightY,        ///< Right stick Y vertical axis
    LeftTrigger,   ///< Left-bottom trigger lever (joyhat)
    RightTrigger,  ///< Right-bottom trigger lever (joyhat)

    Count
};

KAZE_NAMESPACE_END

#endif // kaze_input_gamepadconstants_h_
