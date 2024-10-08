/// @file Keyboard.h
/// Keyboard class
#pragma once
#ifndef kaze_input_keyboard_h_
#define kaze_input_keyboard_h_

#include "KeyboardConstants.h"

#include <kaze/kaze.h>
#include <kaze/video/WindowConstants.h>

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
    /// @param window window to set
    auto setWindow(const WindowHandle window) noexcept -> void { m_window = window; }

    /// Check if a key is currently pressed down
    /// @param key the key to check
    /// @return whether the key is currently pressed down
    [[nodiscard]]
    auto isDown(Key key) const noexcept -> Bool;
    /// Check if a key is currently un-pressed
    /// @param key key to check
    /// @return whether the key is unpressed
    [[nodiscard]]
    auto isUp(Key key) const noexcept -> Bool { return !isDown(key); }
    /// Check if a key was just pressed down this frame
    /// @param key key to check
    /// @return whether the key was just pressed down this frame
    [[nodiscard]]
    auto isJustDown(Key key) const noexcept -> Bool;
    /// Check if a key was just released this frame
    /// @param key key to check
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

#endif // kaze_input_keyboard_h_
