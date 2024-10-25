/// \file Mouse.h
/// Contains Mouse class
#pragma once

#include "InputEvents.h"
#include "MouseConstants.h"

#include <kaze/core/lib.h>
#include <kaze/core/math/Vec/Vec2.h>
#include <kaze/core/video/WindowConstants.h>

KAZE_NAMESPACE_BEGIN

/// @class Mouse
/// Wrapper around mouse polling functionality.
/// Must pass a window to associate with
class Mouse {
public:
    auto setWindow(const WindowHandle window) noexcept -> Mouse & { m_window = window; return *this; }
    auto getWindow() const noexcept -> WindowHandle { return m_window; }

    [[nodiscard]]
    auto isDown(MouseBtn button) const noexcept -> Bool;

    [[nodiscard]]
    auto isUp(MouseBtn button) const noexcept -> Bool;

    [[nodiscard]]
    auto isJustDown(MouseBtn button) const noexcept -> Bool;

    [[nodiscard]]
    auto isJustUp(MouseBtn button) const noexcept -> Bool;

    [[nodiscard]]
    auto getScroll() const noexcept -> Vec2f { return m_scroll; }

    auto preProcessEvents() -> void;
    auto processEvent(const MouseScrollEvent &e) -> void;
    auto processEvent(const MouseButtonEvent &e) -> void;
    auto processEvent(const MouseMotionEvent &e) -> void;
    auto postProcessEvents() -> void;

    auto getPosition() const noexcept -> Vec2f;
    auto getGlobalPosition() const noexcept -> Vec2f;
    auto getMotion() const noexcept -> Vec2f; ///< if relative mode is on it will be the last amt

    auto setCaptureMode(bool captureMode) noexcept -> Mouse &;
    auto getCaptureMode() const noexcept -> Bool;
private:
    struct ButtonData {
        Bool value[2]{};
        Uint8 interactions{0};
    };

    Array<ButtonData, static_cast<int>(MouseBtn::Count)> m_buttons{};
    Vec2f m_scroll{};
    Vec2f m_motion{};
    WindowHandle m_window{};
};

KAZE_NAMESPACE_END