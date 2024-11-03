/// \file common_glfw3.cpp
/// Top-level Glfw3 backend implementation
#include "common_glfw3.h"
#include "window_glfw3.h"

#include "GLFW/glfw3.h"

#include <build/desktop/Debug/_deps/glfw3-src/include/GLFW/glfw3.h>
#include <kaze/core/platform/backend/backend.h>
#include <kaze/core/platform/defines.h>

KAZE_NS_BEGIN

namespace backend {
    WindowHandleContainer<WindowData> windows{};
    Array<GamepadData, GLFW_JOYSTICK_LAST + 1> gamepads{};

    static Array<int, static_cast<Uint>(Key::Count)> s_keyToGlfwKey = {
        GLFW_KEY_ESCAPE,
        GLFW_KEY_ENTER,
        GLFW_KEY_TAB,
        GLFW_KEY_SPACE,
        GLFW_KEY_BACKSPACE,
        GLFW_KEY_UP,
        GLFW_KEY_DOWN,
        GLFW_KEY_LEFT,
        GLFW_KEY_RIGHT,
        GLFW_KEY_INSERT,
        GLFW_KEY_DELETE,
        GLFW_KEY_HOME,
        GLFW_KEY_END,
        GLFW_KEY_PAGE_UP,
        GLFW_KEY_PAGE_DOWN,
        GLFW_KEY_PRINT_SCREEN,
        GLFW_KEY_EQUAL,
        GLFW_KEY_MINUS,
        GLFW_KEY_LEFT_BRACKET,
        GLFW_KEY_RIGHT_BRACKET,
        GLFW_KEY_SEMICOLON,
        GLFW_KEY_APOSTROPHE,
        GLFW_KEY_COMMA,
        GLFW_KEY_PERIOD,
        GLFW_KEY_SLASH,
        GLFW_KEY_BACKSLASH,
        GLFW_KEY_GRAVE_ACCENT,
        GLFW_KEY_CAPS_LOCK,
        GLFW_KEY_NUM_LOCK,

        GLFW_KEY_F1,
        GLFW_KEY_F2,
        GLFW_KEY_F3,
        GLFW_KEY_F4,
        GLFW_KEY_F5,
        GLFW_KEY_F6,
        GLFW_KEY_F7,
        GLFW_KEY_F8,
        GLFW_KEY_F9,
        GLFW_KEY_F10,
        GLFW_KEY_F11,
        GLFW_KEY_F12,
        GLFW_KEY_F13,
        GLFW_KEY_F14,
        GLFW_KEY_F15,
        GLFW_KEY_F16,
        GLFW_KEY_F17,
        GLFW_KEY_F18,
        GLFW_KEY_F19,
        GLFW_KEY_F20,
        GLFW_KEY_F21,
        GLFW_KEY_F22,
        GLFW_KEY_F23,
        GLFW_KEY_F24,

        GLFW_KEY_LEFT_SHIFT,
        GLFW_KEY_RIGHT_SHIFT,
        GLFW_KEY_LEFT_ALT,
        GLFW_KEY_RIGHT_ALT,
        GLFW_KEY_LEFT_CONTROL,
        GLFW_KEY_RIGHT_CONTROL,
        GLFW_KEY_LEFT_SUPER,
        GLFW_KEY_RIGHT_SUPER,

        GLFW_KEY_KP_0,
        GLFW_KEY_KP_1,
        GLFW_KEY_KP_2,
        GLFW_KEY_KP_3,
        GLFW_KEY_KP_4,
        GLFW_KEY_KP_5,
        GLFW_KEY_KP_6,
        GLFW_KEY_KP_7,
        GLFW_KEY_KP_8,
        GLFW_KEY_KP_9,

        GLFW_KEY_KP_ADD,
        GLFW_KEY_KP_SUBTRACT,
        GLFW_KEY_KP_MULTIPLY,
        GLFW_KEY_KP_DIVIDE,
        GLFW_KEY_KP_DECIMAL,
        GLFW_KEY_KP_ENTER,

        GLFW_KEY_0,
        GLFW_KEY_1,
        GLFW_KEY_2,
        GLFW_KEY_3,
        GLFW_KEY_4,
        GLFW_KEY_5,
        GLFW_KEY_6,
        GLFW_KEY_7,
        GLFW_KEY_8,
        GLFW_KEY_9,

        GLFW_KEY_A,
        GLFW_KEY_B,
        GLFW_KEY_C,
        GLFW_KEY_D,
        GLFW_KEY_E,
        GLFW_KEY_F,
        GLFW_KEY_G,
        GLFW_KEY_H,
        GLFW_KEY_I,
        GLFW_KEY_J,
        GLFW_KEY_K,
        GLFW_KEY_L,
        GLFW_KEY_M,
        GLFW_KEY_N,
        GLFW_KEY_O,
        GLFW_KEY_P,
        GLFW_KEY_Q,
        GLFW_KEY_R,
        GLFW_KEY_S,
        GLFW_KEY_T,
        GLFW_KEY_U,
        GLFW_KEY_V,
        GLFW_KEY_W,
        GLFW_KEY_X,
        GLFW_KEY_Y,
        GLFW_KEY_Z,
    };
    static Array<int, GLFW_KEY_LAST + 1> s_glfwKeyToKey = {0};

    static Array<int, static_cast<unsigned>(GamepadBtn::Count)> s_gamepadButtonToGlfw = {
        GLFW_GAMEPAD_BUTTON_A,
        GLFW_GAMEPAD_BUTTON_B,
        GLFW_GAMEPAD_BUTTON_X,
        GLFW_GAMEPAD_BUTTON_Y,
        GLFW_GAMEPAD_BUTTON_LEFT_BUMPER,
        GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER,
        GLFW_GAMEPAD_BUTTON_LEFT_THUMB,
        GLFW_GAMEPAD_BUTTON_RIGHT_THUMB,
        GLFW_GAMEPAD_BUTTON_BACK,
        GLFW_GAMEPAD_BUTTON_START,
        GLFW_GAMEPAD_BUTTON_GUIDE,
        GLFW_GAMEPAD_BUTTON_DPAD_LEFT,
        GLFW_GAMEPAD_BUTTON_DPAD_RIGHT,
        GLFW_GAMEPAD_BUTTON_DPAD_UP,
        GLFW_GAMEPAD_BUTTON_DPAD_DOWN,
    };
    static Array<int, GLFW_GAMEPAD_BUTTON_LAST + 1> s_glfwToGamepadButton = {0};

    static Array<int, static_cast<Uint8>(GamepadAxis::Count)> s_gamepadAxisToGlfw = {
        GLFW_GAMEPAD_AXIS_LEFT_X,
        GLFW_GAMEPAD_AXIS_LEFT_Y,
        GLFW_GAMEPAD_AXIS_RIGHT_X,
        GLFW_GAMEPAD_AXIS_RIGHT_Y,
        GLFW_GAMEPAD_AXIS_LEFT_TRIGGER,
        GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER,
    };

    static Array<Uint8, GLFW_GAMEPAD_AXIS_LAST + 1> s_glfwToGamepadAxis{0};

    void initGlobals() noexcept
    {
        // set up input query arrays
        if (s_glfwKeyToKey[GLFW_KEY_Z] == 0)
        {
            for (auto i = 0; const auto key : s_keyToGlfwKey)
            {
                s_glfwKeyToKey[key] = i++;
            }
        }

        if (s_glfwToGamepadButton[GLFW_GAMEPAD_BUTTON_LAST] == 0)
        {
            for (auto i = 0; const auto btn : s_gamepadButtonToGlfw)
            {
                s_glfwToGamepadButton[btn] = i++;
            }
        }

        if (s_glfwToGamepadAxis[GLFW_GAMEPAD_AXIS_LAST] == 0)
        {
            for (auto i = 0; const auto axis : s_gamepadAxisToGlfw)
            {
                s_glfwToGamepadAxis[axis] = i++;
            }
        }

        // Check if any gamepads are present on initialization
        for (int i = 0; i <= GLFW_JOYSTICK_LAST; ++i)
        {
            if (glfwJoystickPresent(i) && glfwJoystickIsGamepad(i))
            {
                gamepads[i].isConnected = true;
            }
        }
    }

    const char *getGlfwErrorStr(int *outCode) noexcept
    {
        const char *message;
        const auto code = glfwGetError(&message);
        if (outCode)
            *outCode = code;

        return message;
    }

    /// Reset glfw error state
    void clearGlfwError() noexcept
    {
        glfwGetError(nullptr);
    }

    int toGLFWaxis(GamepadAxis axis) noexcept
    {
        return s_gamepadAxisToGlfw[ static_cast<int>(axis) ];
    }

    int toGLFWbutton(GamepadBtn button) noexcept
    {
        return s_gamepadButtonToGlfw[ static_cast<int>(button) ];
    }

    GamepadAxis toGamepadAxis(int glfwAxis) noexcept
    {
        return static_cast<GamepadAxis>(s_glfwToGamepadAxis[glfwAxis]);
    }

    GamepadBtn toGamepadBtn(int glfwButton) noexcept
    {
        return static_cast<GamepadBtn>(s_gamepadButtonToGlfw[glfwButton]);
    }

    int toGLFWkey(Key key) noexcept
    {
        return s_keyToGlfwKey[ static_cast<int>(key) ];
    }

    Key toKey(int glfwKey) noexcept
    {
        return static_cast<Key>(s_glfwKeyToKey[glfwKey]);
    }


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

    auto init() noexcept -> bool
    {
        if ( !glfwInit() )
        {
            KAZE_PUSH_ERR(Error::BE_InitErr, "Failed to initialize glfw3: {}", getGlfwErrorStr());
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
        for (auto &[handle, data] : windows.data())
        {
            data.relCursorPos = {};
        }

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
            KAZE_PUSH_ERR(Error::BE_RuntimeErr, "Failed to get clipboard string: {}", getGlfwErrorStr());
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

    static Array<Uint, static_cast<Uint>(CursorType::Count)> s_toGlfwCursorType = {
        GLFW_ARROW_CURSOR,
        GLFW_IBEAM_CURSOR,
        GLFW_CROSSHAIR_CURSOR,
        GLFW_RESIZE_ALL_CURSOR,
        GLFW_RESIZE_NS_CURSOR,
        GLFW_RESIZE_EW_CURSOR,
        GLFW_RESIZE_NWSE_CURSOR,
        GLFW_RESIZE_NESW_CURSOR,
        GLFW_POINTING_HAND_CURSOR,
        GLFW_NOT_ALLOWED_CURSOR
    };

    auto cursor::createStandard(CursorType type, CursorHandle *outCursor) noexcept -> bool
    {
        RETURN_IF_NULL(outCursor);

        const auto cursor = glfwCreateStandardCursor(s_toGlfwCursorType[static_cast<int>(type)]);
        if ( !cursor)
        {
            KAZE_PUSH_ERR(Error::BE_RuntimeErr, "Failed to create cursor: {}", getGlfwErrorStr());
            return false;
        }

        *outCursor = {.handle = cursor};
        return true;
    }

    auto cursor::createCustom(ImageHandle image, int anchorX, int anchorY, CursorHandle *outCursor) noexcept -> bool
    {
        RETURN_IF_NULL(image.handle);
        RETURN_IF_NULL(image.data());
        RETURN_IF_NULL(outCursor);

        GLFWimage glfwImage = {
            .width = static_cast<int>(image.width()),
            .height = static_cast<int>(image.height()),
            .pixels = static_cast<unsigned char *>(image.data())
        };

        const auto cursor = glfwCreateCursor(&glfwImage, anchorX, anchorY);
        if ( !cursor )
        {
            KAZE_PUSH_ERR(Error::BE_RuntimeErr, "Failed to create custom cursor: {}", getGlfwErrorStr());
            return false;
        }

        *outCursor = {.handle = cursor};
        return true;
    }

    auto cursor::getCursor(WindowHandle window, CursorHandle *outCursor) noexcept -> bool
    {
        RETURN_IF_NULL(window);
        RETURN_IF_NULL(outCursor);

        WindowData *data;
        if ( !getWindowData(window, &data) )
            return false;

        *outCursor = data->cursor;
        return true;
    }

    auto cursor::setCursor(WindowHandle window, CursorHandle cursor) noexcept -> bool
    {
        RETURN_IF_NULL(window);

        WindowData *data;
        if ( !getWindowData(window, &data) )
            return false;

        data->cursor = cursor;

        glfwSetCursor(WIN_CAST(window), static_cast<GLFWcursor *>(cursor.handle));
        return true;
    }

    auto cursor::destroy(CursorHandle cursor) noexcept -> bool
    {
        if (cursor.handle)
        {
            glfwDestroyCursor(static_cast<GLFWcursor *>(cursor.handle));
        }

        return true;
    }

    auto keyboard::isDown(Key key, bool *outDown) noexcept -> bool
    {
        RETURN_IF_NULL(outDown);

        const auto window = windows.getMainWindow();
        if ( !window )
        {
            KAZE_PUSH_ERR(Error::BE_RuntimeErr, "failed to get current window context, no window was open");
            return false;
        }

        const auto down = glfwGetKey(WIN_CAST(window), backend::toGLFWkey(key)) == GLFW_PRESS;
        ERR_CHECK(Error::BE_RuntimeErr, "get key down");

        *outDown = down;
        return true;
    }

    auto cursor::getRelativePosition(const WindowHandle window, float *outX, float *outY) noexcept -> bool
    {
        RETURN_IF_NULL(window);

        double tempX=0, tempY=0;
        glfwGetCursorPos(WIN_CAST(window), &tempX, &tempY);
        ERR_CHECK(Error::BE_RuntimeErr, "get cursor position");

        float scaleX, scaleY;
        if ( !window::getContentScale(window, &scaleX, &scaleY) )
            return false;

        if (outX)
            *outX = static_cast<float>(tempX) * scaleX;
        if (outY)
            *outY = static_cast<float>(tempY) * scaleY;

        return true;
    }

    auto cursor::getGlobalPosition(float *outX, float *outY) noexcept -> bool
    {
        double tempX=0, tempY=0;
        if (const auto window = static_cast<GLFWwindow *>(windows.getMainWindow()))
        {
            glfwGetCursorPos(window, &tempX, &tempY); ERR_CHECK(Error::BE_RuntimeErr, "get cursor position");

            int winX=0, winY=0;
            glfwGetWindowPos(window, &winX, &winY);   ERR_CHECK(Error::BE_RuntimeErr, "get window position");

            tempX += static_cast<double>(winX);
            tempY += static_cast<double>(winY);

            float scaleX, scaleY;
            if ( !window::getContentScale(window, &scaleX, &scaleY) )
                return false;

            tempX *= scaleX;
            tempY *= scaleY;
        }
        else
        {
            KAZE_PUSH_ERR(Error::BE_RuntimeErr, "Failed to get current context: {}", getGlfwErrorStr());
            return false;
        }


        if (outX)
            *outX = static_cast<float>(tempX);
        if (outY)
            *outY = static_cast<float>(tempY);

        return true;
    }

    auto cursor::isDown(WindowHandle window, MouseBtn button, bool *outDown) noexcept -> bool
    {
        RETURN_IF_NULL(window);
        RETURN_IF_NULL(outDown);

        *outDown = glfwGetMouseButton(WIN_CAST(window), static_cast<int>(button)) == GLFW_PRESS;
        ERR_CHECK(Error::BE_RuntimeErr, "get mouse button state");

        return true;
    }


#define GP_INDEX_IN_RANGE(index) \
    do { if ( !((index) >= 0 && (index) < GamepadMaxSlots) ) { \
        KAZE_PUSH_ERR(Error::OutOfRange, "gamepad index `{}` is out of range", (index)); \
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

        if (deadzone > 0)
        {
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
        }

        *outMoved = valueX != valueLastX || valueY != valueLastY;
        return true;
    }
}

KAZE_NS_END
