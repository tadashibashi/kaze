#include "PlatformBackend_glfw3.h"
#include <kaze/platform/PlatformBackend.h>
#include <kaze/platform/PlatformDefines.h>

#if   KAZE_TARGET_WINDOWS
#   define GLFW_EXPOSE_NATIVE_WIN32
#elif KAZE_TARGET_MACOS
#   define GLFW_EXPOSE_NATIVE_COCOA
#elif KAZE_TARGET_LINUX
#   if KAZE_USE_WAYLAND
#       define GLFW_EXPOSE_NATIVE_WAYLAND
#   else
#       define GLFW_EXPOSE_NATIVE_X11
#   endif
# elif KAZE_TARGET_EMSCRIPTEN
// webgl
#else
#   error GLFW does not support the current platform...
#endif

#include <GLFW/glfw3.h>

#if !KAZE_TARGET_EMSCRIPTEN
#include <GLFW/glfw3native.h>
#endif

#include <kaze/debug.h>

KAZE_NAMESPACE_BEGIN

namespace backend {

    NativePlatformData PlatformBackend::windowGetNativeInfo(void *window)
    {
    #if   KAZE_TARGET_WINDOWS
        return {
            .windowHandle = glfwGetWin32Window(WIN_CAST(window)),
            .displayType = nullptr,
        };
    #elif KAZE_TARGET_MACOS
        return {
            .windowHandle = glfwGetCocoaWindow(WIN_CAST(window)),
            .displayType = nullptr,
        };
    #elif KAZE_TARGET_LINUX
    #   if KAZE_USE_WAYLAND
        return {
            .windowHandle = glfwGetWaylandWindow(WIN_CAST(window)),
            .displayType = nullptr,
        };
    #   else
        return {
            .windowHandle = glfwGetX11Window(WIN_CAST(window)),
            .displayType = XOpenDisplay(nullptr),
        };
    #   endif
    #else
        return {
            .windowHandle = nullptr,
            .displayType = nullptr,
        };
    #endif
    }

    static Uint16 s_keyToGlfwKey[] = {
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
    static_assert(std::size(s_keyToGlfwKey) == static_cast<Uint16>(Key::Count)-1); // -1 for unknown key sentry value

    static Uint16 s_glfwKeyToKey[GLFW_KEY_LAST + 1] = {0};

    static Uint8 s_gamepadButtonToGlfw[] = {
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
    static_assert(std::size(s_gamepadButtonToGlfw) == static_cast<Uint8>(GamepadBtn::Count));

    static Uint8 s_glfwToGamepadButton[GLFW_GAMEPAD_BUTTON_LAST + 1] = {0};

    static Uint8 s_gamepadAxisToGlfw[] = {
        GLFW_GAMEPAD_AXIS_LEFT_X,
        GLFW_GAMEPAD_AXIS_LEFT_Y,
        GLFW_GAMEPAD_AXIS_RIGHT_X,
        GLFW_GAMEPAD_AXIS_RIGHT_Y,
        GLFW_GAMEPAD_AXIS_LEFT_TRIGGER,
        GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER,
    };
    static_assert(std::size(s_gamepadAxisToGlfw) == static_cast<Uint8>(GamepadAxis::Count));

    namespace {
        struct GamepadData {
            bool isConnected{};
            GLFWgamepadstate states[2] = {};
            int currentState{};

            void resetStates()
            {
                for (auto &state : states)
                {
                    std::memset(state.axes, 0, sizeof(state.axes));
                    std::memset(state.buttons, 0, sizeof(state.buttons));
                }

                currentState = 0;
            }

            [[nodiscard]] const GLFWgamepadstate &getCurrentState() const { return states[currentState]; }
            [[nodiscard]] const GLFWgamepadstate &getLastState() const { return states[!currentState]; }
        };
    }

    static GamepadData s_gamepads[GLFW_JOYSTICK_LAST + 1] = {};


    // ===== Static callbacks ===============================================
    static void glfwWindowCloseCallback(GLFWwindow *window)
    {
        PlatformBackend::events.emit(WindowEvent {
            .type = WindowEvent::Closed,
            .window = window,
        });
    }

    #if KAZE_DEBUG
    static void glfwErrorCallback(const int error, const char *description)
    {
        KAZE_CORE_ERR("GLFW Error: {}: {}", error, description);
    }
    #endif

    static void glfwScrollCallback(GLFWwindow *window, const double xoffset, const double yoffset)
    {
        PlatformBackend::events.emit(MouseScrollEvent {
        .offset = {
            static_cast<float>(xoffset),
            static_cast<float>(yoffset)
        },
        .window = window,
        });
    }

    static void glfwCursorPosCallback(GLFWwindow *window, const double x, const double y)
    {
        PlatformBackend::events.emit(MouseMotionEvent {
           .position = {
               static_cast<Float>(x),
               static_cast<Float>(y),
           },
           .window = window,
        });
    }

    static void glfwKeyCallback(GLFWwindow *window, const int key, const int scancode, const int action, const int mods)
    {
        PlatformBackend::events.emit(KeyboardEvent {
            .type = (action == GLFW_RELEASE ? KeyboardEvent::Up : KeyboardEvent::Down),
            .key = static_cast<Key>(s_glfwKeyToKey[key]),
            .isRepeat = (action == GLFW_REPEAT),
            .window = static_cast<void *>(window)
        });
    }

    static MouseBtn toMouseBtn(Uint button)
    {
        if (button > GLFW_MOUSE_BUTTON_LAST)
            return MouseBtn::Count;
        return static_cast<MouseBtn>(button);
    }

    static void glfwMouseButtonCallback(GLFWwindow *window, const int button, const int action, const int mods)
    {
        PlatformBackend::events.emit(MouseButtonEvent {
            .type = action == GLFW_RELEASE ? MouseButtonEvent::Up : MouseButtonEvent::Down,
            .button = toMouseBtn(button),
            .window = window,
        });
    }

    static void glfwCursorEnterCallback(GLFWwindow *window, const int entered)
    {
        PlatformBackend::events.emit(WindowEvent {
            .type = entered == 0 ? WindowEvent::MouseExited : WindowEvent::MouseEntered,
            .window = window,
        });
    }

    static void glfwJoystickCallback(const int jid, const int event)
    {
        auto &gamepad = s_gamepads[jid];
        if (event == GLFW_CONNECTED)
        {
            gamepad.isConnected = true;
            PlatformBackend::events.emit(GamepadConnectEvent {
                .id = jid,
                .type = GamepadConnectEvent::Connected
            });
        }
        else if (event == GLFW_DISCONNECTED)
        {
            gamepad.isConnected = false;
            gamepad.resetStates();
            PlatformBackend::events.emit(GamepadConnectEvent {
                .id = jid,
                .type = GamepadConnectEvent::Disconnected
            });
        }
    }

    static void glfwWindowSizeCallback(GLFWwindow *window, const int x, const int y)
    {
        PlatformBackend::events.emit(WindowEvent {
            .type = WindowEvent::Resized,
            .data0 = x,
            .data1 = y,
            .window = window,
        });
    }

    static void glfwFramebufferSize(GLFWwindow *window, const int x, const int y)
    {
        PlatformBackend::events.emit(WindowEvent {
            .type = WindowEvent::ResizedFramebuffer,
            .data0 = x,
            .data1 = y,
            .window = window,
        });
    }

    static void glfwWindowPosCallback(GLFWwindow *window, const int x, const int y)
    {
        PlatformBackend::events.emit(WindowEvent {
            .type = WindowEvent::Moved,
            .data0 = x,
            .data1 = y,
            .window = window,
        });
    }

    static void glfwWindowFocusCallback(GLFWwindow *window, const int focus)
    {
        PlatformBackend::events.emit(WindowEvent {
            .type = focus == 0 ? WindowEvent::FocusLost : WindowEvent::FocusGained,
            .window = window,
        });
    }

    static void glfwWindowMaximizeCallback(GLFWwindow *window, const int maximize)
    {
        PlatformBackend::events.emit(WindowEvent {
            .type = maximize == 0 ? WindowEvent::Restored : WindowEvent::Maximized,
            .window = window,
        });
    }

    static void glfwWindowIconifyCallback(GLFWwindow *window, const int iconify)
    {
        PlatformBackend::events.emit(WindowEvent {
            .type = iconify == 0 ? WindowEvent::Restored : WindowEvent::Minimized,
            .window = window,
        });
    }

    static void glfwDropCallback(GLFWwindow *window, int count, const char **paths)
    {
        for (int i = 0; i < count; ++i)
        {
            if (paths[i])
            {
                double cursorX, cursorY;
                glfwGetCursorPos(window, &cursorX, &cursorY);

                PlatformBackend::events.emit(FileDropEvent {
                    .path = paths[i],
                    .window = window,
                    .position = {
                        static_cast<float>(cursorX),
                        static_cast<float>(cursorY)
                    }
                });
            }
        }
    }


    Bool PlatformBackend::init() noexcept
    {
    #if KAZE_DEBUG
        glfwSetErrorCallback(glfwErrorCallback);
    #endif

        if (!glfwInit())
        {
            return KAZE_FALSE;
        }

        glfwSetJoystickCallback(glfwJoystickCallback);

        // set up input query arrays
        if (s_glfwKeyToKey[GLFW_KEY_Z] == 0)
        {
            for (auto i = 1; const auto key : s_keyToGlfwKey)
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

        for (int i = 0; i <= GLFW_JOYSTICK_LAST; ++i)
        {
            if (glfwJoystickPresent(i) && glfwJoystickIsGamepad(i))
            {
                s_gamepads[i].isConnected = true;
            }
        }

        return KAZE_TRUE;
    }

    void PlatformBackend::shutdown() noexcept
    {
        glfwTerminate();
    }

    Double PlatformBackend::getTime() noexcept
    {
        return glfwGetTime();
    }

    void PlatformBackend::pollEvents()
    {
        glfwPollEvents();

        for (int i = 0; i <= GLFW_GAMEPAD_AXIS_LAST; ++i)
        {
            auto &[isConnected, states, currentState] = s_gamepads[i];
            if (isConnected)
            {
                currentState = !currentState; // flip current state
                glfwGetGamepadState(i, &states[currentState]);
            }
        }
    }

    void *PlatformBackend::windowCreate(const char *title, const size_t width, const size_t height, const WindowInit::Flags initFlags)
    {
        glfwWindowHint(GLFW_DECORATED, !static_cast<bool>(initFlags & WindowInit::Borderless));
        glfwWindowHint(GLFW_RESIZABLE, static_cast<bool>(initFlags & WindowInit::Resizable));
        glfwWindowHint(GLFW_MAXIMIZED, static_cast<bool>(initFlags & WindowInit::Maximized));
        glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, static_cast<bool>(initFlags & WindowInit::Transparent));
        glfwWindowHint(GLFW_FLOATING, static_cast<bool>(initFlags & WindowInit::Floating));
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE); // hide window at first until everything has been processed
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        const auto window = glfwCreateWindow(
            static_cast<Int>(width), static_cast<Int>(height),
            title, nullptr, nullptr);

        glfwSetWindowUserPointer(window, new WindowData);

        // Setup callbacks
        glfwSetWindowCloseCallback(window, glfwWindowCloseCallback);
        glfwSetKeyCallback(window, glfwKeyCallback);
        glfwSetScrollCallback(window, glfwScrollCallback);
        glfwSetMouseButtonCallback(window, glfwMouseButtonCallback);
        glfwSetCursorPosCallback(window, glfwCursorPosCallback);
        glfwSetCursorEnterCallback(window, glfwCursorEnterCallback);
        glfwSetWindowPosCallback(window, glfwWindowPosCallback);
        glfwSetWindowSizeCallback(window, glfwWindowSizeCallback);
        glfwSetFramebufferSizeCallback(window, glfwFramebufferSize);
        glfwSetWindowFocusCallback(window, glfwWindowFocusCallback);
        glfwSetWindowMaximizeCallback(window, glfwWindowMaximizeCallback);
        glfwSetWindowIconifyCallback(window, glfwWindowIconifyCallback);
        glfwSetDropCallback(window, glfwDropCallback);

        if (initFlags & WindowInit::Fullscreen)
        {
            windowSetNativeFullscreen(window, true);
        }

        if ( !(initFlags & WindowInit::Hidden) )
        {
            glfwShowWindow(window);
        }


        return window;
    }

    void PlatformBackend::windowDestroy(void *window)
    {
        const auto glfwWindow = WIN_CAST(window);
        const auto data = static_cast<WindowData *>(glfwGetWindowUserPointer(glfwWindow));

        glfwDestroyWindow(glfwWindow);
        delete data;
    }

    bool PlatformBackend::windowIsOpen(void* window)
    {
        glfwWindowShouldClose(WIN_CAST(window));
        return glfwGetError(nullptr) == GLFW_NO_ERROR;
    }

    void PlatformBackend::windowSetUserData(void *window, void* data)
    {
        const auto windata = WINDATA_CAST(window);
        windata->userdata = data;
    }

    void *PlatformBackend::windowGetUserData(void *window)
    {
        const auto windata = WINDATA_CAST(window);
        return windata->userdata;
    }

    void PlatformBackend::windowSetTitle(void *window, const Cstring title)
    {
        glfwSetWindowTitle(WIN_CAST(window), title);
    }

    const char* PlatformBackend::windowGetTitle(void* window) noexcept
    {
        return glfwGetWindowTitle(WIN_CAST(window));
    }

    void PlatformBackend::windowGetDisplaySize(void *window, int *x, int *y) noexcept
    {
        glfwGetFramebufferSize(WIN_CAST(window), x, y);
    }

    void PlatformBackend::windowSetSize(void *window, const int x, const int y)
    {
        glfwSetWindowSize(WIN_CAST(window), x, y);
    }

    void PlatformBackend::windowGetSize(void *window, int *x, int *y) noexcept
    {
        glfwGetWindowSize(WIN_CAST(window), x, y);
    }

    bool PlatformBackend::windowIsNativeFullscreen(void *window) noexcept
    {
    #if KAZE_TARGET_MACOS
        return getWindowCocoaFullScreen(WIN_CAST(window));
    #else
        return static_cast<bool>(glfwGetWindowMonitor(WINCAST(window)));
    #endif
    }

    void PlatformBackend::windowSetNativeFullscreen(void *window, const Bool value)
    {
        if (windowIsNativeFullscreen(window) == value) return;

        auto glfwWindow = WIN_CAST(window);
        auto data = static_cast<WindowData *>(glfwGetWindowUserPointer(glfwWindow));
    #if KAZE_TARGET_MACOS

        if (data->isDesktopFullscreen)
            windowSetDesktopFullscreen(window, false);
        setWindowCocoaFullScreen(glfwWindow, value);
    #else

        if (value)
        {
            auto monitor = glfwGetPrimaryMonitor();
            auto mode = glfwGetVideoMode(monitor);

            if (!data->isDesktopFullscreen) // only update last state if non-fullscreen
            {
                windowGetPosition(glfwWindow, &data->last.rect.x, &data->last.rect.y);
                windowGetSize(glfwWindow, &data->last.rect.w, &data->last.rect.h);
            }
            else
            {
                windowSetDesktopFullscreen(window, false);
            }

            glfwSetWindowMonitor(glfwWindow, monitor,
                0, 0, mode->width, mode->height, mode->refreshRate);
        }
        else
        {
            glfwSetWindowMonitor(WINCAST(window), nullptr,
                data-last.rect.x, data->last.rect.y,
                data->last.rect.w, data->last.rect.h,
                GLFW_DONT_CARE);
        }
    #endif
    }

    bool PlatformBackend::windowIsDesktopFullscreen(void *window) noexcept
    {
        const auto data = WINDATA_CAST(window);
        KAZE_ASSERT(data != nullptr);

        return data->isDesktopFullscreen;
    }

    void PlatformBackend::windowSetDesktopFullscreen(void *window, const bool value)
    {
        if (value == windowIsDesktopFullscreen(window)) return;

        const auto data = WINDATA_CAST(window);
        KAZE_ASSERT(data != nullptr);

        if (value)
        {
            const auto monitor = glfwGetPrimaryMonitor();
            const auto mode = glfwGetVideoMode(monitor);

            if (windowIsNativeFullscreen(window))
                windowSetNativeFullscreen(window, false);

            windowGetPosition(window, &data->last.rect.x, &data->last.rect.y);
            windowGetSize(window, &data->last.rect.w, &data->last.rect.h);
            data->last.decorated = !windowIsBorderless(window);


            windowSetBorderless(window, true);
            glfwSetWindowMonitor(WIN_CAST(window), nullptr,
                0, 0, mode->width, mode->height, GLFW_DONT_CARE);
            data->isDesktopFullscreen = true;
        }
        else
        {
            glfwSetWindowMonitor(WIN_CAST(window), nullptr,
                data->last.rect.x, data->last.rect.y, data->last.rect.w, data->last.rect.h, GLFW_DONT_CARE);
            windowSetBorderless(window, !data->last.decorated);
            data->isDesktopFullscreen = false;
        }

    }

    bool PlatformBackend::windowIsBorderless(void *window) noexcept
    {
        return !static_cast<bool>(glfwGetWindowAttrib(WIN_CAST(window), GLFW_DECORATED));
    }

    void PlatformBackend::windowSetBorderless(void *window, const bool value)
    {
        glfwSetWindowAttrib(WIN_CAST(window), GLFW_DECORATED, !value);
    }

    bool PlatformBackend::windowIsMinimized(void *window) noexcept
    {
        return static_cast<bool>(glfwGetWindowAttrib(WIN_CAST(window), GLFW_ICONIFIED));
    }

    void PlatformBackend::windowMinimize(void *window)
    {
        glfwIconifyWindow(WIN_CAST(window));
    }

    bool PlatformBackend::windowIsMaximized(void *window) noexcept
    {
        return static_cast<bool>(glfwGetWindowAttrib(WIN_CAST(window), GLFW_MAXIMIZED));
    }

    void PlatformBackend::windowMaximize(void *window)
    {
        glfwMaximizeWindow(WIN_CAST(window));
    }

    void PlatformBackend::windowRestore(void *window)
    {
        glfwRestoreWindow(WIN_CAST(window));
    }

    void PlatformBackend::windowGetPosition(void *window, int *x, int *y) noexcept
    {
        glfwGetWindowPos(WIN_CAST(window), x, y);
    }

    void PlatformBackend::windowSetPosition(void *window, const int x, const int y) noexcept
    {
        glfwSetWindowPos(WIN_CAST(window), x, y);
    }

    bool PlatformBackend::windowGetResizable(void *window) noexcept
    {
        return static_cast<bool>(glfwGetWindowAttrib(WIN_CAST(window), GLFW_RESIZABLE));
    }

    void PlatformBackend::windowSetResizable(void *window, const bool value)
    {
        glfwSetWindowAttrib(WIN_CAST(window), GLFW_RESIZABLE, value);
    }

    bool PlatformBackend::windowIsHidden(void* window) noexcept
    {
        return static_cast<bool>(glfwGetWindowAttrib(WIN_CAST(window), GLFW_VISIBLE));
    }

    void PlatformBackend::windowSetHidden(void* window, const bool value)
    {
        if (value)
        {
            glfwHideWindow(WIN_CAST(window));
        }
        else
        {
            glfwShowWindow(WIN_CAST(window));
        }
    }

    bool PlatformBackend::windowIsHovered(void *window) noexcept
    {
        return static_cast<bool>(glfwGetWindowAttrib(WIN_CAST(window), GLFW_HOVERED));
    }

    bool PlatformBackend::windowIsFloating(void *window) noexcept
    {
        return static_cast<bool>(glfwGetWindowAttrib(WIN_CAST(window), GLFW_FLOATING));
    }

    void PlatformBackend::windowSetFloating(void *window, const bool value)
    {
        glfwSetWindowAttrib(WIN_CAST(window), GLFW_FLOATING, static_cast<int>(value));
    }

    bool PlatformBackend::windowIsTransparent(void *window) noexcept
    {
        return static_cast<bool>(glfwGetWindowAttrib(WIN_CAST(window), GLFW_TRANSPARENT_FRAMEBUFFER));
    }

    void PlatformBackend::windowSetTransparent(void *window, const bool value)
    {
        glfwSetWindowAttrib(WIN_CAST(window), GLFW_TRANSPARENT_FRAMEBUFFER, static_cast<int>(value));
    }

    bool PlatformBackend::windowIsFocused(void *window) noexcept
    {
        return static_cast<bool>(glfwGetWindowAttrib(WIN_CAST(window), GLFW_FOCUSED));
    }

    void PlatformBackend::windowFocus(void *window)
    {
        glfwFocusWindow(WIN_CAST(window));
    }

    void PlatformBackend::windowSetMinimumSize(void *window, const int minWidth, const int minHeight)
    {
        const auto data = WINDATA_CAST(window);
        glfwSetWindowSizeLimits(WIN_CAST(window),
            minWidth, minHeight, data->last.maxSize.x, data->last.maxSize.y);
        data->last.minSize = { minWidth, minHeight };
    }

    void PlatformBackend::windowSetMaximumSize(void *window, const int maxWidth, const int maxHeight)
    {
        const auto data = WINDATA_CAST(window);
        glfwSetWindowSizeLimits(WIN_CAST(window),
            data->last.minSize.x, data->last.minSize.y, maxWidth, maxHeight);
        data->last.maxSize = { maxWidth, maxHeight };
    }

    void PlatformBackend::windowGetMinimumSize(void *window, int *minWidth, int *minHeight)
    {
        const auto data = WINDATA_CAST(window);
        if (minWidth)
            *minWidth = data->last.minSize.x;
        if (minHeight)
            *minHeight = data->last.minSize.y;
    }

    void PlatformBackend::windowGetMaximumSize(void *window, int *maxWidth, int *maxHeight)
    {
        const auto data = WINDATA_CAST(window);
        if (maxWidth)
            *maxWidth = data->last.maxSize.x;
        if (maxHeight)
            *maxHeight = data->last.maxSize.y;
    }

    bool PlatformBackend::gamepadIsConnected(const int index) noexcept
    {
        return glfwJoystickPresent(index) && glfwJoystickIsGamepad(index);
    }

    bool PlatformBackend::gamepadIsButtonDown(const int index, const GamepadBtn button)
    {

        return s_gamepads[index].getCurrentState().buttons[ s_gamepadButtonToGlfw[static_cast<int>(button)] ];
    }

    bool PlatformBackend::gamepadIsButtonJustDown(const int index, const GamepadBtn button)
    {
        const auto glfwButton = s_gamepadButtonToGlfw[static_cast<int>(button)];
        const auto &gamepad = s_gamepads[index];
        return gamepad.getCurrentState().buttons[glfwButton] &&
            !gamepad.getLastState().buttons[glfwButton];
    }

    bool PlatformBackend::gamepadIsButtonJustUp(const int index, const GamepadBtn button)
    {
        const auto glfwButton = s_gamepadButtonToGlfw[static_cast<int>(button)];
        const auto &gamepad = s_gamepads[index];
        return !gamepad.getCurrentState().buttons[glfwButton] &&
            gamepad.getLastState().buttons[glfwButton];
    }

    float PlatformBackend::gamepadGetAxis(const int index, const GamepadAxis axis)
    {
        return s_gamepads[index].getCurrentState().axes[ s_gamepadAxisToGlfw[static_cast<Uint8>(axis)] ];
    }

    bool PlatformBackend::gamepadDidAxisMove(const int index, const GamepadAxis axis, const float deadzone)
    {
        const auto glfwAxis = s_gamepadAxisToGlfw[static_cast<Uint8>(axis)];
        auto value = s_gamepads[index].getCurrentState().axes[glfwAxis];
        auto lastValue = s_gamepads[index].getLastState().axes[glfwAxis];

        if (mathf::abs(value) <= deadzone)
            value = 0;

        if (mathf::abs(lastValue) <= deadzone)
            lastValue = 0;

        return value != lastValue;
    }

    bool PlatformBackend::gamepadDidAxesMove(const int index, const GamepadAxis axisX, const GamepadAxis axisY, const float deadzone)
    {
        const auto glfwAxisX = s_gamepadAxisToGlfw[static_cast<Uint8>(axisX)];
        const auto glfwAxisY = s_gamepadAxisToGlfw[static_cast<Uint8>(axisY)];

        auto valueX = s_gamepads[index].getCurrentState().axes[glfwAxisX];
        auto valueY = s_gamepads[index].getCurrentState().axes[glfwAxisY];
        auto valueLastX = s_gamepads[index].getLastState().axes[glfwAxisX];
        auto valueLastY = s_gamepads[index].getLastState().axes[glfwAxisY];

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

        return valueX != valueLastX || valueY != valueLastY;
    }

    const char *PlatformBackend::getClipboard()
    {
        return glfwGetClipboardString(nullptr);
    }

    void PlatformBackend::setClipboard(const char *text)
    {
        glfwSetClipboardString(nullptr, text);
    }

}

KAZE_NAMESPACE_END
