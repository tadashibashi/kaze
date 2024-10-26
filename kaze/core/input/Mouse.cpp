#include "Mouse.h"

#include <kaze/core/platform/backend/backend.h>

KAZE_NAMESPACE_BEGIN

auto Mouse::isDown(MouseBtn button) const noexcept -> Bool
{
    return m_buttons[ static_cast<int>(button) ].value[0];
}

auto Mouse::isUp(MouseBtn button) const noexcept -> Bool
{
    return !m_buttons[ static_cast<int>(button) ].value[0];
}

auto Mouse::isJustDown(MouseBtn button) const noexcept -> Bool
{
    return m_buttons[ static_cast<int>(button) ].value[0] &&
        !m_buttons[ static_cast<int>(button) ].value[1];
}

auto Mouse::isJustUp(MouseBtn button) const noexcept -> Bool
{
    return !m_buttons[ static_cast<int>(button) ].value[0] &&
        m_buttons[ static_cast<int>(button) ].value[1];
}

auto Mouse::preProcessEvents() -> void
{
    m_scroll = {};
    m_motion = {};
}

auto Mouse::processEvent(const MouseScrollEvent &e) -> void
{
    if (e.window == m_window)
    {
        m_scroll += e.offset;
    }
}

auto Mouse::processEvent(const MouseButtonEvent &e) -> void
{
    if (e.window == m_window)
    {
        ++m_buttons[static_cast<int>(e.button)].interactions;
    }
}

auto Mouse::processEvent(const MouseMotionEvent &e) -> void
{
    if (e.window == m_window)
    {
        if (getMode() == CursorMode::Capture)
        {
            m_motion += e.position;
        }
    }
}

auto Mouse::postProcessEvents() -> void
{
    for (int i = 0; i < m_buttons.size(); ++i)
    {
        auto &button = m_buttons[i];
        if (button.interactions > 0)
        {
            const auto curVal = button.value[0];

            --button.interactions;
            if (button.interactions == 0)
            {
                backend::cursor::isDown(m_window, static_cast<MouseBtn>(i), &button.value[0]);
            }
            else
            {
                button.value[0] = !button.value[1];
                button.value[1] = curVal;
            }

        }
        else
        {
            button.value[1] = button.value[0];
        }
    }
}

auto Mouse::getPosition() const noexcept -> Vec2f
{
    return m_motion;
}

auto Mouse::getGlobalPosition() const noexcept -> Vec2f
{
    Vec2f position;
    backend::cursor::getGlobalPosition(&position.x, &position.y);
    return position;
}

auto Mouse::setMode(const CursorMode mode) noexcept -> Mouse &
{
    backend::window::setCursorMode(m_window, mode);
    return *this;
}

auto Mouse::getMode() const noexcept -> CursorMode
{
    auto captureMode = CursorMode::Count;
    backend::window::getCursorMode(m_window, &captureMode);

    return captureMode;
}

KAZE_NAMESPACE_END
