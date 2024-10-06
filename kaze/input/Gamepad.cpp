#include "Gamepad.h"
#include <kaze/platform/backend.h>

KAZE_NAMESPACE_BEGIN

Bool Gamepad::isUp(const GamepadBtn btn) const noexcept
{
    bool btnIsDown;
    if ( !backend::gamepad::isDown(m_id, btn, &btnIsDown) )
        return false;
    return !btnIsDown;
}

Bool Gamepad::isDown(const GamepadBtn btn) const noexcept
{
    bool btnIsDown;
    if ( !backend::gamepad::isDown(m_id, btn, &btnIsDown) )
        return false;
    return btnIsDown;
}

Bool Gamepad::isJustDown(const GamepadBtn btn) const noexcept
{
    bool btnJustDown;
    if ( !backend::gamepad::isJustDown(m_id, btn, &btnJustDown) )
        return false;
    return btnJustDown;
}

Bool Gamepad::isJustUp(const GamepadBtn btn) const noexcept
{
    bool btnJustUp;
    if ( !backend::gamepad::isJustUp(m_id, btn, &btnJustUp) )
        return false;
    return btnJustUp;
}

Float Gamepad::getAxis(const GamepadAxis axis, const Float deadzone) const noexcept
{
    float value;
    if ( !backend::gamepad::getAxis(m_id, axis, &value) )
        return 0;
    return mathf::abs(value) <= deadzone ? 0 : value;
}

Bool Gamepad::getAxisMoved(const GamepadAxis axis, const Float deadzone) const noexcept
{
    bool axisMoved;
    if ( !backend::gamepad::getAxisMoved(m_id, axis, deadzone, &axisMoved) )
        return false;
    return axisMoved;
}

Bool Gamepad::isConnected() const noexcept
{
    bool gamepadConnected;
    if ( !backend::gamepad::isConnected(m_id, &gamepadConnected) )
        return false;
    return gamepadConnected;
}

Vec2f Gamepad::getAxes(const GamepadAxis axisX, const GamepadAxis axisY, const Float deadzone) const noexcept
{
    const Vec2f result { getAxis(axisX), getAxis(axisY) };
    return (result.magnitude() <= deadzone) ? Vec2f() : result;
}

Bool Gamepad::getAxesMoved(const GamepadAxis axisX, const GamepadAxis axisY, const Float deadzone) const noexcept
{
    bool axesMoved;
    if ( !backend::gamepad::getAxesMoved(m_id, axisX, axisY, deadzone, &axesMoved) )
        return false;

    return axesMoved;
}

KAZE_NAMESPACE_END
