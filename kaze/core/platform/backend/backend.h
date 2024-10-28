#pragma once
#include <kaze/core/lib.h>
#include <kaze/core/input/InputEvents.h>
#include <kaze/core/input/CursorConstants.h>
#include <kaze/core/input/GamepadConstants.h>
#include <kaze/core/input/KeyboardConstants.h>
#include <kaze/core/input/MouseConstants.h>

#include <kaze/core/video/ImageHandle.h>
#include <kaze/core/video/WindowConstants.h>

#include <kaze/core/traits.h>

#include "cursor.h"   // IWYU pragma: export
#include "gamepad.h"  // IWYU pragma: export
#include "keyboard.h" // IWYU pragma: export
#include "window.h"   // IWYU pragma: export

KAZE_NAMESPACE_BEGIN

class Image;

namespace backend {
    /// Struct to pass to the backend to hook into backend events
    struct PlatformCallbacks
    {
        void *userptr{};
        funcptr_t<void(const FileDropEvent &e, double timestamp, void *userptr)>       fileDropCallback{[](auto...){}};
        funcptr_t<void(const GamepadAxisEvent &e, double timestamp, void *userptr)>    gamepadAxisCallback{[](auto...){}};
        funcptr_t<void(const GamepadButtonEvent &e, double timestamp, void *userptr)>  gamepadButtonCallback{[](auto...){}};
        funcptr_t<void(const GamepadConnectEvent &e, double timestamp, void *userptr)> gamepadConnectCallback{[](auto...){}};
        funcptr_t<void(const KeyboardEvent &e, double timestamp, void *userptr)>       keyCallback{[](auto...){}};
        funcptr_t<void(const MouseButtonEvent &e, double timestamp, void *userptr)>    mouseButtonCallback{[](auto...){}};
        funcptr_t<void(const MouseMotionEvent &e, double timestamp, void *userptr)>    mouseMotionCallback{[](auto...){}};
        funcptr_t<void(const MouseScrollEvent &e, double timestamp, void *userptr)>    mouseScrollCallback{[](auto...){}};
        funcptr_t<void(const TextInputEvent &e, double timestamp, void *userptr)>      textInputCallback{[](auto...){}};
        funcptr_t<void(const WindowEvent &e, double timestamp, void *userptr)>         windowCallback{[](auto...){}};

        auto emit(const KeyboardEvent &e, double timestamp = 0) const -> void;
        auto emit(const MouseButtonEvent &e, double timestamp = 0) const -> void;
        auto emit(const MouseScrollEvent &e, double timestamp = 0) const -> void;
        auto emit(const MouseMotionEvent &e, double timestamp = 0) const -> void;
        auto emit(const GamepadConnectEvent &e, double timestamp = 0) const -> void;
        auto emit(const GamepadButtonEvent &e, double timestamp = 0) const -> void;
        auto emit(const GamepadAxisEvent &e, double timestamp = 0) const -> void;
        auto emit(const WindowEvent &e, double timestamp = 0) const -> void;
        auto emit(const TextInputEvent &e, double timestamp = 0) const -> void;
        auto emit(const FileDropEvent &e, double timestamp = 0) const -> void;
    };
    extern PlatformCallbacks events;

    /// Initialize the backend
    /// \returns whether initialization succeeded
    auto init() noexcept -> bool;

    /// Shutdown/terminate the backend
    auto shutdown() noexcept -> void;

    /// Get time in seconds since init was first called.
    /// Most modern platforms and backends support nanosecond granularity.
    auto getTime(double *outTime) noexcept -> bool;

    /// Poll events for the current frame
    /// \returns whether the operation succeeded.
    auto pollEvents() noexcept -> bool;

    /// Set callbacks for input event handling.
    /// \param[in] callbacks   callback struct with pointers to set
    inline auto setCallbacks(const PlatformCallbacks &callbacks) noexcept -> void
    {
        events = callbacks;
    }

    /// Get system clipboard text, or empty string if none available.
    /// \param[out] outText   retrieves text string
    /// \returns whether retrieval was successful.
    auto getClipboard(const char **outText) noexcept -> bool;
    /// Set system clipboard text
    /// \param[in]  text   text to set
    /// \returns whether operation was successful.
    auto setClipboard(const char *text) noexcept -> bool;
}



KAZE_NAMESPACE_END
