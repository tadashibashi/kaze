#pragma once
#include <kaze/core/lib.h>

KAZE_NS_BEGIN

constexpr int GamepadMaxSlots = 16;

/// Clickable button on a game controller
enum class GamepadBtn {
    Unknown = -1,
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
    Unknown = -1,
    LeftX = 0,     ///< Left stick X horizontal axis
    LeftY,         ///< Left stick Y vertical axis
    RightX,        ///< Right stick X horizontal axis
    RightY,        ///< Right stick Y vertical axis
    LeftTrigger,   ///< Left-bottom trigger lever (joyhat)
    RightTrigger,  ///< Right-bottom trigger lever (joyhat)

    Count
};

KAZE_NS_END

