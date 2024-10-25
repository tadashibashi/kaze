#pragma once

#include "GamepadConstants.h"
#include "KeyboardConstants.h"
#include "MouseConstants.h"

#include <kaze/core/lib.h>
#include <kaze/core/math/Vec/Vec2.h>
#include <kaze/core/video/WindowConstants.h>

KAZE_NAMESPACE_BEGIN

/// Window-related events
struct WindowEvent
{
    enum Type
    {
        Resized,            ///< Resize in logical units
        ResizedFramebuffer, ///< Resize in OS pixel units (hi-dpi monitors may differ from logical units)
        Moved,              ///< User drags the window, returning the x and y coordinates
        FocusGained,        ///< Just gains input focus
        FocusLost,          ///< Just loses input focus
        Minimized,          ///< Window just became minimized
        Maximized,          ///< Window just maximized
        Restored,           ///< Window restored from being maximized or minimized
        Closed,             ///< Called before window closed
        MouseEntered,       ///< Mouse just entered the window
        MouseExited,        ///< Mouse just left the window

        Count
    } type;

    Int data0, data1;
    WindowHandle window;
};

/// File just got dropped in a window
struct FileDropEvent
{
    String path;     /// path to the file dropped
    WindowHandle window;    /// window file got dropped on
    Vec2f position;  /// position in the window of the drop
};

struct KeyboardEvent
{
    enum Type
    {
        Down,  ///< Key pressed down
        Up     ///< Key released
    } type;         ///< type of keyboard event

    Key key;        ///< key code
    Bool isRepeat;  ///< key repeated
    WindowHandle window;   ///< window associated with the event
};

struct MouseButtonEvent
{
    enum Type
    {
        Down, ///< Mouse button pressed down
        Up,   ///< Mouse button released
    } type;           ///< type of mouse button event

    MouseBtn button;  ///< mouse button
    WindowHandle window;     ///< window associated with the event
};

/// Describes scroll
struct MouseScrollEvent
{
    Vec2f offset;   ///< relative mouse scroll x and y axes
    WindowHandle window;   ///< window scroll occurred in
};

struct MouseMotionEvent
{
    Vec2f position;  ///< position of mouse in window, or relative motion if relative mode is on
    WindowHandle window;    ///< window the mouse motion event occurred
};

/// Describes a gamepad connection
struct GamepadConnectEvent {
    enum Type { Connected, Disconnected } type;
    Int id;
};

struct GamepadButtonEvent {
    Int controllerIndex;
    enum Type {
        Up,
        Down,
    } type;
    GamepadBtn button;
};

struct GamepadAxisEvent {
    Int controllerIndex;
    GamepadAxis axis;
    Float value;
};

KAZE_NAMESPACE_END
