#include "Keyboard.h"

#include <kaze/core/platform/backend/backend.h>

KAZE_NS_BEGIN

#define ASSERT_KEY_RANGE(key) KAZE_ASSERT(static_cast<Int>(key) >= 0 && static_cast<Int>(key) < static_cast<Int>(Key::Count))

auto Keyboard::isDown(Key key) const noexcept -> Bool
{
    ASSERT_KEY_RANGE(key);
    const auto &keyData = m_keyData[ static_cast<Int>(key) ];
    return keyData.isDown[m_currentIndex];
}

auto Keyboard::isJustDown(Key key) const noexcept -> Bool
{
    ASSERT_KEY_RANGE(key);
    const auto &keyData = m_keyData[ static_cast<Int>(key) ];
    return keyData.isDown[m_currentIndex] && !keyData.isDown[!m_currentIndex];
}

auto Keyboard::isJustUp(Key key) const noexcept -> Bool
{
    ASSERT_KEY_RANGE(key);
    const auto &keyData = m_keyData[ static_cast<Int>(key) ];
    return !keyData.isDown[m_currentIndex] && keyData.isDown[ !m_currentIndex ];
}

auto Keyboard::preProcessEvents() -> void
{
    m_currentIndex = !m_currentIndex;
}

auto Keyboard::processEvent(const KeyboardEvent &e) -> void
{
    // null window allows any key event; when a window is specified, it only allows events associated with it to pass
    if (m_window && e.window != m_window) return;
    ASSERT_KEY_RANGE(e.key);

    if ( (e.isDown && !e.isRepeat) || !e.isDown )
    {
        ++m_keyData[ static_cast<Int>(e.key) ].interactions;
    }
}

auto Keyboard::postProcessEvents() -> void
{
    for (int i = 0; auto &key : m_keyData)
    {
        if (key.interactions > 0)
        {
            --key.interactions;
            if (key.interactions == 0)
            {
                bool keyIsDown;
                if ( backend::keyboard::isDown(static_cast<Key>(i), &keyIsDown) )
                    key.isDown[m_currentIndex] = keyIsDown;
            }
            else
            {
                key.isDown[m_currentIndex] = !key.isDown[!m_currentIndex];
            }

        }
        else
        {
            key.isDown[m_currentIndex] = key.isDown[!m_currentIndex];
        }

        ++i;
    }
}

KAZE_NS_END
