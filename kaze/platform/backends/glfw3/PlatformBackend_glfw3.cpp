/// @file PlatformBackend_glfw3.cpp
/// @description
/// Contains general top-level Glfw3 backend implementation
#include <kaze/platform/PlatformBackend.h>
#include <kaze/platform/PlatformDefines.h>

#include "PlatformBackend_defines.h"
#include "PlatformBackend_globals.h"

#include <GLFW/glfw3.h>

#include <kaze/debug.h>
#include <kaze/errors.h>

KAZE_NAMESPACE_BEGIN

namespace backend {

    // ===== Static callbacks ===============================================

    static void glfwJoystickCallback(const int jid, const int event)
    {
        auto &gamepad = gamepads[jid];
        if (event == GLFW_CONNECTED)
        {
            gamepad.isConnected = true;
            events.emit(GamepadConnectEvent {
                .type = GamepadConnectEvent::Connected,
                .id = jid,
            });
        }
        else if (event == GLFW_DISCONNECTED)
        {
            gamepad.isConnected = false;
            gamepad.resetStates();
            events.emit(GamepadConnectEvent {
                .type = GamepadConnectEvent::Disconnected,
                .id = jid,
            });
        }
    }

    bool init() noexcept
    {
        if ( !glfwInit() )
        {
            KAZE_CORE_ERRCODE(Error::BE_InitErr, "Failed to initialize glfw3: {}", getGlfwErrorStr());
            return false;
        }

        glfwSetJoystickCallback(glfwJoystickCallback);
        if (const auto err = getGlfwErrorStr())
        {
            KAZE_CORE_WARN("Failed to set joystick callback: {}", err); // warn, since platform may not support gamepads
        }

        backend::initGlobals();

        clearGlfwError();
        return true;
    }

    auto shutdown() noexcept -> void
    {
        glfwTerminate();
    }

    auto getTime(double *outTime) noexcept -> bool
    {
        RETURN_IF_NULL(outTime);

        *outTime = glfwGetTime();
        return true;
    }

    auto pollEvents() noexcept -> bool
    {
        glfwPollEvents();

        for (int i = 0; i <= GLFW_JOYSTICK_LAST; ++i)
        {
            auto &[isConnected, states, currentState] = gamepads[i];
            if (isConnected)
            {
                currentState = !currentState; // flip current state
                glfwGetGamepadState(i, &states[currentState]);

                // fire callbacks for any new buttons
                const auto &buttons = states[currentState].buttons;
                const auto &lastButtons = states[!currentState].buttons;
                for (int b = 0; b < std::size(buttons); ++b)
                {
                    if (buttons[b] != lastButtons[b])
                    {
                        events.emit(GamepadButtonEvent {
                            .controllerIndex = i,
                            .type = buttons[b] ? GamepadButtonEvent::Down : GamepadButtonEvent::Up,
                            .button = backend::toGamepadBtn(b),
                        });
                    }
                }

                const auto &axes = states[currentState].axes;
                const auto &lastAxes = states[!currentState].axes;
                for (int a = 0; a < std::size(axes); ++a)
                {
                    if (axes[a] != lastAxes[a])
                    {
                        events.emit(GamepadAxisEvent {
                            .controllerIndex = i,
                            .axis = backend::toGamepadAxis(a),
                            .value = axes[a]
                        });
                    }
                }
            }
        }

        return true;
    }

    auto getClipboard(const char **outText) noexcept -> bool
    {
        RETURN_IF_NULL(outText);

        const auto text = glfwGetClipboardString(nullptr);
        if ( !text )
        {
            KAZE_CORE_ERRCODE(Error::BE_RuntimeErr, "Failed to get clipboard string: {}", getGlfwErrorStr());
            return false;
        }

        *outText = text;
        return true;
    }

    auto setClipboard(const char *text) noexcept -> bool
    {
        RETURN_IF_NULL(text);

        glfwSetClipboardString(nullptr, text); ERR_CHECK(Error::BE_RuntimeErr, "set clipboard string");
        return true;
    }

    auto keyboard::isDown(Key key, bool *outDown) noexcept -> bool
    {
        RETURN_IF_NULL(outDown);

        const auto window = glfwGetCurrentContext();
        if ( !window )
        {
            KAZE_CORE_ERRCODE(Error::BE_RuntimeErr, "failed to get current window context, no window was open");
            return false;
        }

        const auto down = glfwGetKey(window, backend::toGLFWkey(key)) == GLFW_PRESS;
        ERR_CHECK(Error::BE_RuntimeErr, "get key down");

        *outDown = down;
        return true;
    }

    auto mouse::getRelativePosition(const WindowHandle window, float *outX, float *outY) noexcept -> bool
    {
        RETURN_IF_NULL(window);

        double tempX=0, tempY=0;
        glfwGetCursorPos(WIN_CAST(window), &tempX, &tempY);
        ERR_CHECK(Error::BE_RuntimeErr, "get cursor position");

        if (outX)
            *outX = static_cast<float>(tempX);
        if (outY)
            *outY = static_cast<float>(tempY);

        return true;
    }

    auto mouse::getGlobalPosition(float *outX, float *outY) noexcept -> bool
    {
        double tempX=0, tempY=0;
        if (const auto window = glfwGetCurrentContext())
        {
            glfwGetCursorPos(window, &tempX, &tempY); ERR_CHECK(Error::BE_RuntimeErr, "get cursor position");

            int winX=0, winY=0;
            glfwGetWindowPos(window, &winX, &winY);   ERR_CHECK(Error::BE_RuntimeErr, "get window position");

            tempX += static_cast<double>(winX);
            tempY += static_cast<double>(winY);
        }
        else
        {
            KAZE_CORE_ERRCODE(Error::BE_RuntimeErr, "Failed to get current context: {}", getGlfwErrorStr());
            return false;
        }

        if (outX)
            *outX = static_cast<float>(tempX);
        if (outY)
            *outY = static_cast<float>(tempY);

        return true;
    }

#define GP_INDEX_IN_RANGE(index) \
    do { if ( !((index) >= 0 && (index) < MaxGamepadSlots) ) { \
        KAZE_CORE_ERRCODE(Error::OutOfRange, "gamepad index `{}` is out of range", (index)); \
        return false; \
    } } while(0)

    auto gamepad::isConnected(const int index, bool *outConnected) noexcept -> bool
    {
        GP_INDEX_IN_RANGE(index);
        RETURN_IF_NULL(outConnected);

        bool connected = glfwJoystickPresent(index); ERR_CHECK(Error::BE_RuntimeErr, "check joystick is present");
        if ( !connected )
        {
            *outConnected = false;
            return true;
        }

        connected = glfwJoystickIsGamepad(index);   ERR_CHECK(Error::BE_RuntimeErr, "check if joystick is gamepad");

        *outConnected = connected;
        return true;
    }

    auto gamepad::isDown(const int index, const GamepadBtn button, bool *outDown) noexcept -> bool
    {
        GP_INDEX_IN_RANGE(index);
        RETURN_IF_NULL(outDown);

        *outDown = gamepads[index].getCurrentState().buttons[ backend::toGLFWbutton(button) ];
        return true;
    }

    auto gamepad::isJustDown(const int index, const GamepadBtn button, bool *outJustDown) noexcept -> bool
    {
        GP_INDEX_IN_RANGE(index);
        RETURN_IF_NULL(outJustDown);

        const auto glfwButton = backend::toGLFWbutton(button);
        const auto &gamepad = gamepads[index];
        *outJustDown = gamepad.getCurrentState().buttons[glfwButton] &&
            !gamepad.getLastState().buttons[glfwButton];
        return true;
    }

    auto gamepad::isJustUp(const int index, const GamepadBtn button, bool *outJustUp) noexcept -> bool
    {
        GP_INDEX_IN_RANGE(index);
        RETURN_IF_NULL(outJustUp);

        const auto glfwButton = backend::toGLFWbutton(button);
        const auto &gamepad = gamepads[index];
        *outJustUp = !gamepad.getCurrentState().buttons[glfwButton] &&
            gamepad.getLastState().buttons[glfwButton];
        return true;
    }

    auto gamepad::getAxis(const int index, const GamepadAxis axis, float *outValue) noexcept -> bool
    {
        GP_INDEX_IN_RANGE(index);
        RETURN_IF_NULL(outValue);

        *outValue = gamepads[index].getCurrentState().axes[ backend::toGLFWaxis(axis) ];
        return true;
    }

    auto gamepad::getAxisMoved(const int index, const GamepadAxis axis, const float deadzone, bool *outMoved) noexcept -> bool
    {
        GP_INDEX_IN_RANGE(index);
        RETURN_IF_NULL(outMoved);

        const auto glfwAxis = backend::toGLFWaxis(axis);
        auto value = gamepads[index].getCurrentState().axes[glfwAxis];
        auto lastValue = gamepads[index].getLastState().axes[glfwAxis];

        if (mathf::abs(value) <= deadzone)
            value = 0;

        if (mathf::abs(lastValue) <= deadzone)
            lastValue = 0;

        *outMoved = value != lastValue;
        return true;
    }

    auto gamepad::getAxesMoved(const int index, const GamepadAxis axisX, const GamepadAxis axisY, const float deadzone, bool *outMoved) noexcept -> bool
    {
        GP_INDEX_IN_RANGE(index);
        RETURN_IF_NULL(outMoved);

        const auto glfwAxisX = toGLFWaxis(axisX);
        const auto glfwAxisY = toGLFWaxis(axisY);

        auto valueX = gamepads[index].getCurrentState().axes[glfwAxisX];
        auto valueY = gamepads[index].getCurrentState().axes[glfwAxisY];
        auto valueLastX = gamepads[index].getLastState().axes[glfwAxisX];
        auto valueLastY = gamepads[index].getLastState().axes[glfwAxisY];

        if (mathf::distance(0.f, 0.f, valueX, valueY) < deadzone)
        {
            valueX = 0;
            valueY = 0;
        }

        if (mathf::distance(0.f, 0.f, valueLastX, valueLastY) < deadzone)
        {
            valueLastX = 0;
            valueLastY = 0;
        }

        *outMoved = valueX != valueLastX || valueY != valueLastY;
        return true;
    }
}

KAZE_NAMESPACE_END
