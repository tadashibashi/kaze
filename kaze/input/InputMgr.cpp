#include "InputMgr.h"

#include <kaze/platform/backend.h>
#include <kaze/input/Gamepad.h>
#include <kaze/input/Keyboard.h>
#include <kaze/input/Mouse.h>

#include <kaze/debug.h>

#define ASSERT_CONTROLLER_INDEX(index) KAZE_ASSERT((index) >= 0 && ((index) < GamepadMaxSlots))

KAZE_NAMESPACE_BEGIN

struct InputMgr::Impl {
    WindowHandle window{};
    Keyboard keyboard{};
    Mouse mouse{};
    Array<Gamepad, GamepadMaxSlots> gamepads{};

    Impl()
    {
        for (int i = 0; i < GamepadMaxSlots; ++i)
        {
            gamepads[i].id(i);
        }
    }
};

InputMgr::InputMgr() : m(new Impl) {}
InputMgr::~InputMgr() { delete m; }

auto InputMgr::setWindow(const WindowHandle window) noexcept -> InputMgr &
{
    m->window = window;
    m->mouse.setWindow(window);
    m->keyboard.setWindow(window);
    return *this;
}

auto InputMgr::getWindow() noexcept -> WindowHandle
{
    return m->window;
}

auto InputMgr::getWindow() const noexcept -> ConstWindowHandle
{
    return m->window;
}

auto InputMgr::processEvent(const KeyboardEvent &e) noexcept -> void
{
    m->keyboard.processEvent(e);
}

auto InputMgr::processEvent(const MouseMotionEvent &e) noexcept -> void
{
    m->mouse.processEvent(e);
}

auto InputMgr::processEvent(const MouseScrollEvent &e) noexcept -> void
{
    m->mouse.processEvent(e);
}

auto InputMgr::processEvent(const MouseButtonEvent &e) noexcept-> void
{
    m->mouse.processEvent(e);
}

auto InputMgr::preProcessEvents() noexcept -> void
{
    m->keyboard.preProcessEvents();
    m->mouse.preProcessEvents();
}

auto InputMgr::postProcessEvents() noexcept -> void
{
    m->keyboard.postProcessEvents();
    m->mouse.postProcessEvents();
}

// ----- Keyboard -----

auto InputMgr::isDown(Key key) const noexcept -> Bool
{
    return m->keyboard.isDown(key);
}

auto InputMgr::isUp(Key key) const noexcept -> Bool
{
    return m->keyboard.isUp(key);
}

auto InputMgr::isJustDown(Key key) const noexcept -> Bool
{
    return m->keyboard.isJustDown(key);
}

auto InputMgr::isJustUp(Key key) const noexcept -> Bool
{
    return m->keyboard.isJustUp(key);
}

// ----- Gamepad -----

auto InputMgr::isDown(Int index, GamepadBtn button) const noexcept -> Bool
{
    ASSERT_CONTROLLER_INDEX(index);
    return m->gamepads[index].isDown(button);
}

auto InputMgr::isUp(Int index, GamepadBtn button) const noexcept -> Bool
{
    ASSERT_CONTROLLER_INDEX(index);
    return m->gamepads[index].isUp(button);
}

auto InputMgr::isJustDown(Int index, GamepadBtn key) const noexcept -> Bool
{
    ASSERT_CONTROLLER_INDEX(index);
    return m->gamepads[index].isJustDown(key);
}

auto InputMgr::isJustUp(Int index, GamepadBtn key) const noexcept -> Bool
{
    ASSERT_CONTROLLER_INDEX(index);
    return m->gamepads[index].isJustUp(key);
}

auto InputMgr::getAxis(Int index, GamepadAxis axis) const noexcept -> Float
{
    ASSERT_CONTROLLER_INDEX(index);
    return m->gamepads[index].getAxis(axis);
}

auto InputMgr::getAxes(Int index, GamepadAxis axisX, GamepadAxis axisY, Float deadzone) const noexcept -> Vec2f
{
    ASSERT_CONTROLLER_INDEX(index);
    return m->gamepads[index].getAxes(axisX, axisY, deadzone);
}

auto InputMgr::getAxisMoved(Int index, GamepadAxis axis, Float deadzone) const noexcept -> Bool
{
    ASSERT_CONTROLLER_INDEX(index);
    return m->gamepads[index].getAxisMoved(axis, deadzone);
}

auto InputMgr::getAxesMoved(Int index, GamepadAxis axisX, GamepadAxis axisY, Float deadzone) const noexcept -> Bool
{
    ASSERT_CONTROLLER_INDEX(index);
    return m->gamepads[index].getAxesMoved(axisX, axisY, deadzone);
}

// ----- Mouse -----

auto InputMgr::isDown(MouseBtn button) const noexcept -> Bool
{
    return m->mouse.isDown(button);
}

auto InputMgr::isUp(MouseBtn button) const noexcept -> Bool
{
    return m->mouse.isUp(button);
}

auto InputMgr::isJustDown(MouseBtn button) const noexcept -> Bool
{
    return m->mouse.isJustDown(button);
}

auto InputMgr::isJustUp(MouseBtn button) const noexcept -> Bool
{
    return m->mouse.isJustUp(button);
}

auto InputMgr::mouseCoords() const noexcept -> Vec2f
{
    KAZE_ASSERT(m->window);

    Vec2f position;
    backend::mouse::getRelativePosition(m->window, &position.x, &position.y);
    return position;
}

auto InputMgr::mouseGlobalCoords() const noexcept -> Vec2f
{
    Vec2f position;
    backend::mouse::getGlobalPosition(&position.x, &position.y);
    return position;
}

KAZE_NAMESPACE_END
