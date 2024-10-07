/// @file Keyboard.h
/// Wrapper around backend keyboard events.
/// Provides ease of use for an app to poll keyboard state.
#pragma once
#ifndef kaze_input_keyboard_h_
#define kaze_input_keyboard_h_

#include "KeyboardConstants.h"

#include <kaze/kaze.h>
#include <kaze/video/WindowConstants.h>

KAZE_NAMESPACE_BEGIN
struct KeyboardEvent;

class Keyboard
{
public:
    [[nodiscard]] auto getWindow() const noexcept -> WindowHandle { return m_window; }
    auto setWindow(const WindowHandle window) noexcept -> void { m_window = window; }

    [[nodiscard]] bool isDown(Key key) const noexcept;
    [[nodiscard]] bool isUp(Key key) const noexcept { return !isDown(key); }
    [[nodiscard]] bool isJustDown(Key key) const noexcept;
    [[nodiscard]] bool isJustUp(Key key) const noexcept;

    void preProcessEvents();
    void processEvent(const KeyboardEvent &e);
    void postProcessEvents();
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
