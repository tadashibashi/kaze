#pragma once
#ifndef kaze_platform_keyboard_h_
#define kaze_platform_keyboard_h_

#include <kaze/kaze.h>

#include "Key.h"

KAZE_NAMESPACE_BEGIN
    struct KeyboardEvent;

class Keyboard
{
public:
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
};

KAZE_NAMESPACE_END

#endif // kaze_platform_keyboard_h_
