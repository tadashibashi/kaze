#pragma once
#ifndef kaze_platform_platformevent_h_
#define kaze_platform_platformevent_h_

#include <kaze/kaze.h>
#include <kaze/input/InputEvents.h>
#include <kaze/math/Vec/Vec2.h>
#include <kaze/traits.h>

KAZE_NAMESPACE_BEGIN

/// Struct to pass to the backend to hook into backend events
struct PlatformCallbacks
{
    void *userptr{};
    funcptr_t<void(const KeyboardEvent &e, Double timestamp, void *userptr)>      keyCallback{[](auto...){}};
    funcptr_t<void(const MouseButtonEvent &e, Double timestamp, void *userptr)>   mouseButtonCallback{[](auto...){}};
    funcptr_t<void(const MouseScrollEvent &e, Double timestamp, void *userptr)>   mouseScrollCallback{[](auto...){}};
    funcptr_t<void(const MouseMotionEvent &e, Double timestamp, void *userptr)>   mouseMotionCallback{[](auto...){}};
    funcptr_t<void(const GamepadConnectEvent &e, Double timestamp, void *userptr)>gamepadConnectCallback{[](auto...){}};
    funcptr_t<void(const GamepadButtonEvent &e, Double timestamp, void *userptr)> gamepadButtonCallback{[](auto...){}};
    funcptr_t<void(const GamepadAxisEvent &e, Double timestamp, void *userptr)>   gamepadAxisCallback{[](auto...){}};
    funcptr_t<void(const WindowEvent &e, Double timestamp, void *userptr)>        windowCallback{[](auto...){}};
    funcptr_t<void(const FileDropEvent &e, Double timestamp, void *userptr)>      fileDropCallback{[](auto...){}};

    void emit(const KeyboardEvent &e, double timestamp = 0) const;
    void emit(const MouseButtonEvent &e, double timestamp = 0) const;
    void emit(const MouseScrollEvent &e, double timestamp = 0) const;
    void emit(const MouseMotionEvent &e, double timestamp = 0) const;
    void emit(const GamepadConnectEvent &e, double timestamp = 0) const;
    void emit(const GamepadButtonEvent &e, double timestamp = 0) const;
    void emit(const GamepadAxisEvent &e, double timestamp = 0) const;
    void emit(const WindowEvent &e, double timestamp = 0) const;
    void emit(const FileDropEvent &e, double timestamp = 0) const;
};

KAZE_NAMESPACE_END

#endif // kaze_platform_platformevent_h_
