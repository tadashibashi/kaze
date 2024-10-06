#include "Mouse.h"

#include <kaze/platform/backend.h>

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
        bool isCapture;
        if (backend::window::getCaptureCursorMode(e.window, &isCapture))
        {
            if (isCapture)
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
                backend::mouse::isDown(m_window, static_cast<MouseBtn>(i), &button.value[0]);
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
    backend::mouse::getGlobalPosition(&position.x, &position.y);
    return position;
}

auto Mouse::setCaptureMode(bool captureMode) noexcept -> Mouse &
{
    backend::window::setCaptureCursorMode(m_window, captureMode);
    return *this;
}

auto Mouse::getCaptureMode() const noexcept -> Bool
{
    bool captureMode;
    if ( !backend::window::getCaptureCursorMode(m_window, &captureMode) )
        return false;
    return captureMode;
}

KAZE_NAMESPACE_END
