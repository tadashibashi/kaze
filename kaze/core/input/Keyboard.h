/// \file Keyboard.h
/// Keyboard class
#pragma once

#include "KeyboardConstants.h"

#include <kaze/core/lib.h>
#include <kaze/core/video/WindowConstants.h>

KAZE_NAMESPACE_BEGIN
struct KeyboardEvent;

/// Provides ease of use for an app to poll keyboard state.
class Keyboard
{
public:
    /// Get the associated window
    [[nodiscard]]
    auto getWindow() const noexcept -> WindowHandle { return m_window; }

    /// Set the associated window
    /// \param[in] window   window to set
    auto setWindow(const WindowHandle window) noexcept -> void { m_window = window; }

    /// Check if a key is currently pressed down
    /// \param[in] key   the key to query
    /// @return whether the key is currently pressed down
    [[nodiscard]]
    auto isDown(Key key) const noexcept -> Bool;
    /// Check if a key is currently un-pressed
    /// \param[in] key   key to query
    /// @return whether the key is unpressed
    [[nodiscard]]
    auto isUp(Key key) const noexcept -> Bool { return !isDown(key); }
    /// Check if a key was just pressed down this frame
    /// \param[in] key   key to query
    /// @return whether the key was just pressed down this frame
    [[nodiscard]]
    auto isJustDown(Key key) const noexcept -> Bool;
    /// Check if a key was just released this frame
    /// \param[in] key   key to query
    /// @return whether the key was just released this frame
    [[nodiscard]]
    auto isJustUp(Key key) const noexcept -> Bool;

    auto preProcessEvents() -> void;
    auto processEvent(const KeyboardEvent &e) -> void;
    auto postProcessEvents() -> void;
private:
    int m_currentIndex{};

    struct KeyData
    {
        bool isDown[2]{};
        Uint8 interactions{};
    };

    Array<KeyData, static_cast<int>(Key::Count)> m_keyData{};
    WindowHandle m_window{};
};

KAZE_NAMESPACE_END
