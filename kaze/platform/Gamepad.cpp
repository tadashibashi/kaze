#include "Gamepad.h"
#include "PlatformBackend.h"

KAZE_NAMESPACE_BEGIN

using backend::PlatformBackend;

Bool Gamepad::isUp(const GamepadBtn btn) const noexcept
{
    return !PlatformBackend::gamepadIsButtonDown(m_id, btn);
}

Bool Gamepad::isDown(const GamepadBtn btn) const noexcept
{
    return PlatformBackend::gamepadIsButtonDown(m_id, btn);
}

Bool Gamepad::isJustDown(const GamepadBtn btn) const noexcept
{
    return PlatformBackend::gamepadIsButtonJustDown(m_id, btn);
}

Bool Gamepad::isJustUp(const GamepadBtn btn) const noexcept
{
    return PlatformBackend::gamepadIsButtonJustUp(m_id, btn);
}

Float Gamepad::getAxis(const GamepadAxis axis, const Float deadzone) const noexcept
{
    const auto value = PlatformBackend::gamepadGetAxis(m_id, axis);
    return mathf::abs(value) <= deadzone ? 0 : value;
}

Bool Gamepad::didAxisMove(const GamepadAxis axis, const Float deadzone) const noexcept
{
    return PlatformBackend::gamepadDidAxisMove(m_id, axis, deadzone);
}

Bool Gamepad::isConnected() const noexcept
{
    return PlatformBackend::gamepadIsConnected(m_id);
}

Vec2f Gamepad::getAxes(const GamepadAxis axisX, const GamepadAxis axisY, const Float deadzone) const noexcept
{
    const Vec2f result { getAxis(axisX), getAxis(axisY) };
    return (result.magnitude() <= deadzone) ? Vec2f() : result;
}

Bool Gamepad::didAxesMove(const GamepadAxis axisX, const GamepadAxis axisY, const Float deadzone) const noexcept
{
    return PlatformBackend::gamepadDidAxesMove(m_id, axisX, axisY, deadzone);
}

KAZE_NAMESPACE_END
