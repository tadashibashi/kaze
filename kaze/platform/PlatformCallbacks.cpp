#include "PlatformCallbacks.h"
#include "backend.h"

KAZE_NAMESPACE_BEGIN

static double getTime()
{
    double time;
    if ( !backend::getTime(&time) )
        return 0;
    return time;
}

auto PlatformCallbacks::emit(const KeyboardEvent &e, const double timestamp) const -> void {

    keyCallback(e, (timestamp != 0 ? timestamp : getTime()), userptr);
}

auto PlatformCallbacks::emit(const MouseButtonEvent &e, const double timestamp) const -> void {
    mouseButtonCallback(e,  (timestamp != 0 ? timestamp : getTime()), userptr);
}

auto PlatformCallbacks::emit(const MouseScrollEvent &e, const double timestamp) const -> void {
    mouseScrollCallback(e, (timestamp != 0 ? timestamp : getTime()), userptr);
}

auto PlatformCallbacks::emit(const MouseMotionEvent &e, const double timestamp) const -> void {
    mouseMotionCallback(e, (timestamp != 0 ? timestamp : getTime()), userptr);
}

auto PlatformCallbacks::emit(const GamepadConnectEvent &e, const double timestamp) const -> void {
    gamepadConnectCallback(e, (timestamp != 0 ? timestamp : getTime()), userptr);
}

auto PlatformCallbacks::emit(const GamepadButtonEvent &e, const double timestamp) const -> void {
    gamepadButtonCallback(e, (timestamp == 0 ? timestamp : getTime()), userptr);
}

auto PlatformCallbacks::emit(const GamepadAxisEvent &e, const double timestamp) const -> void {
    gamepadAxisCallback(e, (timestamp == 0 ? timestamp : getTime()), userptr);
}

auto PlatformCallbacks::emit(const WindowEvent &e, const double timestamp) const -> void {
    windowCallback(e,  (timestamp != 0 ? timestamp : getTime()), userptr);
}

auto PlatformCallbacks::emit(const FileDropEvent &e, const double timestamp) const -> void {
    fileDropCallback(e,  (timestamp != 0 ? timestamp : getTime()), userptr);
}

KAZE_NAMESPACE_END
