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

void PlatformCallbacks::emit(const KeyboardEvent &e, const Double timestamp) const {

    keyCallback(e, (timestamp != 0 ? timestamp : getTime()), userptr);
}

void PlatformCallbacks::emit(const MouseButtonEvent &e, const Double timestamp) const {
    mouseButtonCallback(e,  (timestamp != 0 ? timestamp : getTime()), userptr);
}

void PlatformCallbacks::emit(const MouseScrollEvent &e, const Double timestamp) const {
    mouseScrollCallback(e, (timestamp != 0 ? timestamp : getTime()), userptr);
}

void PlatformCallbacks::emit(const MouseMotionEvent &e, const Double timestamp) const {
    mouseMotionCallback(e, (timestamp != 0 ? timestamp : getTime()), userptr);
}

void PlatformCallbacks::emit(const GamepadConnectEvent &e, const Double timestamp) const {
    gamepadConnectCallback(e, (timestamp != 0 ? timestamp : getTime()), userptr);
}

void PlatformCallbacks::emit(const GamepadButtonEvent &e, const Double timestamp) const {
    gamepadButtonCallback(e, (timestamp == 0 ? timestamp : getTime()), userptr);
}

void PlatformCallbacks::emit(const GamepadAxisEvent &e, const Double timestamp) const {
    gamepadAxisCallback(e, (timestamp == 0 ? timestamp : getTime()), userptr);
}

void PlatformCallbacks::emit(const WindowEvent &e, const Double timestamp) const {
    windowCallback(e,  (timestamp != 0 ? timestamp : getTime()), userptr);
}

void PlatformCallbacks::emit(const FileDropEvent &e, const Double timestamp) const {
    fileDropCallback(e,  (timestamp != 0 ? timestamp : getTime()), userptr);
}
KAZE_NAMESPACE_END
