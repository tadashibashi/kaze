/// @file PlatformBackend_globals.cpp
/// @description
/// Implementation for constant conversions and global variables
#include "PlatformBackend_globals.h"

KAZE_NAMESPACE_BEGIN

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
}


KAZE_NAMESPACE_END
