#include "Gamepad.h"
#include <kaze/platform/backend.h>

KAZE_NAMESPACE_BEGIN

auto Gamepad::isUp(const GamepadBtn btn) const noexcept -> Bool
{
    bool btnIsDown;
    if ( !backend::gamepad::isDown(m_id, btn, &btnIsDown) )
        return false;
    return !btnIsDown;
}

auto Gamepad::isDown(const GamepadBtn btn) const noexcept -> Bool
{
    bool btnIsDown;
    if ( !backend::gamepad::isDown(m_id, btn, &btnIsDown) )
        return false;
    return btnIsDown;
}

auto Gamepad::isJustDown(const GamepadBtn btn) const noexcept -> Bool
{
    bool btnJustDown;
    if ( !backend::gamepad::isJustDown(m_id, btn, &btnJustDown) )
        return false;
    return btnJustDown;
}

auto Gamepad::isJustUp(const GamepadBtn btn) const noexcept -> Bool
{
    bool btnJustUp;
    if ( !backend::gamepad::isJustUp(m_id, btn, &btnJustUp) )
        return false;
    return btnJustUp;
}

auto Gamepad::getAxis(const GamepadAxis axis, const Float deadzone) const noexcept -> Float
{
    float value;
    if ( !backend::gamepad::getAxis(m_id, axis, &value) )
        return 0;
    return mathf::abs(value) <= deadzone ? 0 : value;
}

auto Gamepad::getAxisMoved(const GamepadAxis axis, const Float deadzone) const noexcept -> Bool
{
    bool axisMoved;
    if ( !backend::gamepad::getAxisMoved(m_id, axis, deadzone, &axisMoved) )
        return false;
    return axisMoved;
}

auto Gamepad::isConnected() const noexcept -> Bool
{
    bool gamepadConnected;
    if ( !backend::gamepad::isConnected(m_id, &gamepadConnected) )
        return false;
    return gamepadConnected;
}

auto Gamepad::getAxes(const GamepadAxis axisX, const GamepadAxis axisY, const Float deadzone) const noexcept -> Vec2f
{
    const Vec2f result { getAxis(axisX), getAxis(axisY) };
    return (result.magnitude() <= deadzone) ? Vec2f() : result;
}

auto Gamepad::getAxesMoved(const GamepadAxis axisX, const GamepadAxis axisY, const Float deadzone) const noexcept -> Bool
{
    bool axesMoved;
    if ( !backend::gamepad::getAxesMoved(m_id, axisX, axisY, deadzone, &axesMoved) )
        return false;

    return axesMoved;
}

KAZE_NAMESPACE_END
