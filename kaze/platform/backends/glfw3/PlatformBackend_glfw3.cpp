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
#include <kaze/platform/backends/WindowHandleContainer.h>

KAZE_NAMESPACE_BEGIN
    /// Log error on glfw error then return false
/// @param actionStr message containing the present-tense action describing the previous function's intended goal
#define ERR_CHECK(code, actionStr) do { \
    const char *message_for_error_check; \
    auto err = glfwGetError(&message_for_error_check); \
    if (err != GLFW_NO_ERROR && message_for_error_check) { \
        KAZE_CORE_ERRCODE(code, "{}:{}: Failed to {}: {}", __FILE__, __LINE__, (actionStr), message_for_error_check); \
        return false; \
    } \
} while(0)

/// Log on glfw error, and call a block of code for cleanup, then return false
/// @param actionStr message containing present-tense action describing the previous function's intended goal
/// @param cleanupBlock code wrapped in braces to call for cleanup
#define ERR_CHECK_CLEANUP(code, actionStr, cleanupBlock) do { \
    const char *message; \
    auto err = glfwGetError(&message); \
    if (err != GLFW_NO_ERROR && message) { \
        KAZE_CORE_ERR((code), "{}:{}: Failed to {}: {}", __FILE__, __LINE__, (actionStr), message); \
        cleanupBlock \
        return false; \
    } \
} while(0)

#if KAZE_DEBUG
/// Warn without returning
/// @param actionStr message containing the present-tense action describing the previous function's intended goal
#define WARN_CHECK(actionStr) do { \
    const char *message; \
    auto err = glfwGetError(&message); \
    if (err != GLFW_NO_ERROR && message) { \
        KAZE_CORE_WARN("{}:{}: Failed to {}: {}", __FILE__, __LINE__, (actionStr), message); \
    } \
} while(0)
#else
/// On non-debug mode, just consume the error, if any
#define WARN_CHECK(actionStr) do { \
    glfwGetError(nullptr); \
} while(0)
#endif

#define RETURN_IF_NULL(obj) do { if ( !static_cast<bool>(obj) ) { \
    KAZE_CORE_ERRCODE(Error::NullArgErr, "{}:{}: required parameter {} was null", __FILE__, __LINE__, #obj); \
    return false; \
} } while(0)

namespace backend {

    static WindowHandleContainer<WindowData> s_windows;

    NativePlatformData window::getNativeInfo(const WindowHandle window) noexcept
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

    static Array<Uint16, static_cast<Uint>(Key::Count)> s_keyToGlfwKey = {
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
    static Array<Uint16, GLFW_KEY_LAST + 1> s_glfwKeyToKey = {0};

    static Array<Uint8, static_cast<unsigned>(GamepadBtn::Count)> s_gamepadButtonToGlfw = {
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
    static Array<Uint8, GLFW_GAMEPAD_BUTTON_LAST + 1> s_glfwToGamepadButton = {0};

    static Array<Uint8, static_cast<Uint8>(GamepadAxis::Count)> s_gamepadAxisToGlfw = {
        GLFW_GAMEPAD_AXIS_LEFT_X,
        GLFW_GAMEPAD_AXIS_LEFT_Y,
        GLFW_GAMEPAD_AXIS_RIGHT_X,
        GLFW_GAMEPAD_AXIS_RIGHT_Y,
        GLFW_GAMEPAD_AXIS_LEFT_TRIGGER,
        GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER,
    };

    static Array<Uint8, GLFW_GAMEPAD_AXIS_LAST + 1> s_glfwToGamepadAxis{0};

    namespace {
        struct GamepadData {
            bool isConnected{};
            GLFWgamepadstate states[2] = {};
            int currentState{};

            void resetStates() noexcept
            {
                for (auto &state : states)
                {
                    std::memset(state.axes, 0, sizeof(state.axes));
                    std::memset(state.buttons, 0, sizeof(state.buttons));
                }

                currentState = 0;
            }

            [[nodiscard]] const GLFWgamepadstate &getCurrentState() const noexcept { return states[currentState]; }
            [[nodiscard]] const GLFWgamepadstate &getLastState() const noexcept { return states[!currentState]; }
        };
    }

    static GamepadData s_gamepads[GLFW_JOYSTICK_LAST + 1] = {};


    // ===== Static callbacks ===============================================
    static void glfwWindowCloseCallback(GLFWwindow *window)
    {
        events.emit(WindowEvent {
            .type = WindowEvent::Closed,
            .window = window,
        });
    }

    static void glfwScrollCallback(GLFWwindow *window, const double xoffset, const double yoffset)
    {
        events.emit(MouseScrollEvent {
        .offset = {
            static_cast<float>(xoffset),
            static_cast<float>(yoffset)
        },
        .window = window,
        });
    }

    static void glfwCursorPosCallback(GLFWwindow *window, const double x, const double y)
    {
        events.emit(MouseMotionEvent {
           .position = {
               static_cast<Float>(x),
               static_cast<Float>(y),
           },
           .window = window,
        });
    }

    static void glfwKeyCallback(GLFWwindow *window, const int key, const int scancode, const int action, const int mods)
    {
        events.emit(KeyboardEvent {
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
        events.emit(MouseButtonEvent {
            .type = action == GLFW_RELEASE ? MouseButtonEvent::Up : MouseButtonEvent::Down,
            .button = toMouseBtn(button),
            .window = window,
        });
    }

    static void glfwCursorEnterCallback(GLFWwindow *window, const int entered)
    {
        events.emit(WindowEvent {
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
            events.emit(GamepadConnectEvent {
                .id = jid,
                .type = GamepadConnectEvent::Connected
            });
        }
        else if (event == GLFW_DISCONNECTED)
        {
            gamepad.isConnected = false;
            gamepad.resetStates();
            events.emit(GamepadConnectEvent {
                .id = jid,
                .type = GamepadConnectEvent::Disconnected
            });
        }
    }

    static void glfwWindowSizeCallback(GLFWwindow *window, const int x, const int y)
    {
        events.emit(WindowEvent {
            .type = WindowEvent::Resized,
            .data0 = x,
            .data1 = y,
            .window = window,
        });
    }

    static void glfwFramebufferSize(GLFWwindow *window, const int x, const int y)
    {
        events.emit(WindowEvent {
            .type = WindowEvent::ResizedFramebuffer,
            .data0 = x,
            .data1 = y,
            .window = window,
        });
    }

    static void glfwWindowPosCallback(GLFWwindow *window, const int x, const int y)
    {
        events.emit(WindowEvent {
            .type = WindowEvent::Moved,
            .data0 = x,
            .data1 = y,
            .window = window,
        });
    }

    static void glfwWindowFocusCallback(GLFWwindow *window, const int focus)
    {
        events.emit(WindowEvent {
            .type = focus == 0 ? WindowEvent::FocusLost : WindowEvent::FocusGained,
            .window = window,
        });
    }

    static void glfwWindowMaximizeCallback(GLFWwindow *window, const int maximize)
    {
        events.emit(WindowEvent {
            .type = maximize == 0 ? WindowEvent::Restored : WindowEvent::Maximized,
            .window = window,
        });
    }

    static void glfwWindowIconifyCallback(GLFWwindow *window, const int iconify)
    {
        events.emit(WindowEvent {
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

                events.emit(FileDropEvent {
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

    static const char *getGlfwErrorStr(int *outCode = nullptr)
    {
        const char *message;
        const auto code = glfwGetError(&message);
        if (outCode)
            *outCode = code;

        return message;
    }

    /// Reset glfw error state
    static void clearGlfwError()
    {
        glfwGetError(nullptr);
    }

    /// Private helper to safely grab window data associated with window
    /// @param [in]  window  window to get data associated to
    /// @param [out] outData retrieves pointer to data
    /// @returns true if successfully retrieved, and false if missing or unsuccessfully retrieved
    static auto getWindowData(const WindowHandle window, WindowData **outData) -> bool
    {
        KAZE_ASSERT(window);
        KAZE_ASSERT(outData);

        WindowData *data;
        if ( !s_windows.getData(window, &data) )
            return false;
        if ( !data )
        {
            KAZE_CORE_ERRCODE(Error::BE_LogicError, "missing window data");
            return false;
        }

        *outData = data;
        return true;
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

        for (int i = 0; i <= GLFW_JOYSTICK_LAST; ++i)
        {
            if (glfwJoystickPresent(i) && glfwJoystickIsGamepad(i))
            {
                s_gamepads[i].isConnected = true;
            }
        }

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
            auto &[isConnected, states, currentState] = s_gamepads[i];
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
                            .type = buttons[b] ? GamepadButtonEvent::Down : GamepadButtonEvent::Up,
                            .controllerIndex = i,
                            .button = static_cast<GamepadBtn>(s_glfwToGamepadButton[b]),
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
                            .axis = static_cast<GamepadAxis>(s_glfwToGamepadAxis[a]),
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
            KAZE_CORE_ERRCODE(Error::BE_RuntimeError, "Failed to get clipboard string: {}", getGlfwErrorStr());
            return false;
        }

        *outText = text;
        return true;
    }

    auto setClipboard(const char *text) noexcept -> bool
    {
        RETURN_IF_NULL(text);

        glfwSetClipboardString(nullptr, text); ERR_CHECK(Error::BE_RuntimeError, "set clipboard string");
        return true;
    }

    auto window::open(const char *title, const int width, const int height, const WindowInit::Flags flags,
                      WindowHandle *outWindow) noexcept -> bool
    {
        RETURN_IF_NULL(outWindow);
        clearGlfwError(); // just in case there is any leftover error

        // Set up window hints
        glfwWindowHint(GLFW_DECORATED, !static_cast<bool>(flags & WindowInit::Borderless)); WARN_CHECK("set decoracted window hint");
        glfwWindowHint(GLFW_RESIZABLE, static_cast<bool>(flags & WindowInit::Resizable));   WARN_CHECK("set resizable window hint");
        glfwWindowHint(GLFW_MAXIMIZED, static_cast<bool>(flags & WindowInit::Maximized));   WARN_CHECK("set maximized window hint");
        glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, static_cast<bool>(flags & WindowInit::Transparent)); WARN_CHECK("set transparent framebuffer window hint");
        glfwWindowHint(GLFW_FLOATING, static_cast<bool>(flags & WindowInit::Floating));     WARN_CHECK("set floating window hint");
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);      WARN_CHECK("set window visible hint to false"); // hide window at first until everything has been processed
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);  WARN_CHECK("set window client api to none");

        const auto window = glfwCreateWindow(width, height, title, nullptr, nullptr); ERR_CHECK(Error::BE_RuntimeError, "create GLFWwindow");
        KAZE_ASSERT(window);

        // Set up window callbacks
        glfwSetWindowCloseCallback    ( window, glfwWindowCloseCallback ); WARN_CHECK("set window close callback");
        glfwSetKeyCallback            ( window, glfwKeyCallback );         WARN_CHECK("set window key callback");
        glfwSetScrollCallback         ( window, glfwScrollCallback );      WARN_CHECK("set window scroll callback");
        glfwSetMouseButtonCallback    ( window, glfwMouseButtonCallback ); WARN_CHECK("set window mouse button callback");
        glfwSetCursorPosCallback      ( window, glfwCursorPosCallback );   WARN_CHECK("set window cursor pos callback");
        glfwSetCursorEnterCallback    ( window, glfwCursorEnterCallback ); WARN_CHECK("set window cursor enter callback");
        glfwSetWindowPosCallback      ( window, glfwWindowPosCallback );   WARN_CHECK("set window pos callback");
        glfwSetWindowSizeCallback     ( window, glfwWindowSizeCallback );  WARN_CHECK("set window size callback");
        glfwSetFramebufferSizeCallback( window, glfwFramebufferSize );     WARN_CHECK("set window framebuffer size callback");
        glfwSetWindowFocusCallback    ( window, glfwWindowFocusCallback ); WARN_CHECK("set window focus callback");
        glfwSetWindowMaximizeCallback ( window, glfwWindowMaximizeCallback ); WARN_CHECK("set window maximize callback");
        glfwSetWindowIconifyCallback  ( window, glfwWindowIconifyCallback ); WARN_CHECK("set window iconify/minimize callback");
        glfwSetDropCallback           ( window, glfwDropCallback );        WARN_CHECK("set window drop callback");


        if (flags & WindowInit::Fullscreen)
        {
            if (!setFullscreen(window, true))
            {
                KAZE_CORE_WARN("Failed to set fullscreen mode: {}", getError().message);
            }
        }

        if ( !(flags & WindowInit::Hidden) )
        {
            glfwShowWindow(window); WARN_CHECK("show window");
        }

        if ( !s_windows.emplace(window, {}) )
        {
            glfwDestroyWindow(window); ERR_CHECK(Error::BE_RuntimeError, "clean up window after failed data emplacement");
            return false;
        }

        *outWindow = window;

        clearGlfwError();
        return true;
    }

    auto window::close(const WindowHandle window) noexcept -> bool
    {
        if ( !window )
        {
            KAZE_CORE_ERRCODE(Error::BE_InvalidWindowHandle, "WindowHandle was null");
            return false;
        }

        bool wasErased;
        if ( !s_windows.erase(window, &wasErased) )
            return false;

        if ( !wasErased )
        {
            KAZE_CORE_ERRCODE(Error::BE_InvalidWindowHandle, "WindowHandle was invalid");
            return false;
        }

        glfwDestroyWindow(WIN_CAST(window)); ERR_CHECK(Error::BE_RuntimeError, "destroy window");

        return true;
    }

    auto window::isOpen(const WindowHandle window, bool *outIsOpen) noexcept -> bool
    {
        RETURN_IF_NULL(window);
        return s_windows.contains(window, outIsOpen);
    }

    auto window::setUserData(WindowHandle window, void *userdata) noexcept -> bool
    {
        WindowData *data;
        if ( !getWindowData(window, &data) )
            return false;

        data->userdata = userdata;
        return true;
    }

    auto window::getUserData(const WindowHandle window, void **outUserdata) noexcept -> bool
    {
        RETURN_IF_NULL(window);
        RETURN_IF_NULL(outUserdata);

        WindowData *data;
        if ( !getWindowData(window, &data) )
            return false;

        *outUserdata = data->userdata;
        return true;
    }

    auto window::setTitle(const WindowHandle window, const char *title) noexcept -> bool
    {
        RETURN_IF_NULL(window);
        RETURN_IF_NULL(title);
        glfwSetWindowTitle(WIN_CAST(window), title); ERR_CHECK(Error::BE_RuntimeError, "set the window title");

        return true;
    }

    auto window::getTitle(const WindowHandle window, const char **outTitle) noexcept -> bool
    {
        RETURN_IF_NULL(window);
        RETURN_IF_NULL(outTitle);

        const auto title = glfwGetWindowTitle(WIN_CAST(window));
        if ( !title )
        {
            KAZE_CORE_ERRCODE(Error::BE_RuntimeError, "Failed to get window title: {}", getGlfwErrorStr());
            return false;
        }

        *outTitle = title;
        return true;
    }

    auto window::getFramebufferSize(const WindowHandle window, int *outWidth, int *outHeight) noexcept -> bool
    {
        RETURN_IF_NULL(window);

        glfwGetFramebufferSize(WIN_CAST(window), outWidth, outHeight); ERR_CHECK(Error::BE_RuntimeError, "get framebuffer size");

        return true;
    }

    auto window::setSize(const WindowHandle window, const int width, const int height) noexcept -> bool
    {
        RETURN_IF_NULL(window);

        glfwSetWindowSize(WIN_CAST(window), width, height);
        ERR_CHECK(Error::BE_RuntimeError, "set logical window size");

        return true;
    }

    auto window::getSize(const WindowHandle window, int *outWidth, int *outHeight) noexcept -> bool
    {
        RETURN_IF_NULL(window);

        glfwGetWindowSize(WIN_CAST(window), outWidth, outHeight);
        ERR_CHECK(Error::BE_RuntimeError, "get logical window size");

        return true;
    }

    static auto windowIsNativeFullscreen(const WindowHandle window, bool *outFullscreen) noexcept -> bool
    {
        RETURN_IF_NULL(window);
        RETURN_IF_NULL(outFullscreen);

#if KAZE_TARGET_MACOS
        if ( !getWindowCocoaFullScreen(WIN_CAST(window), outFullscreen) )
            return false;
#else
        const auto tempFullscreen = static_cast<bool>(glfwGetWindowMonitor(WINCAST(window)));
        ERR_CHECK(Error::BE_RuntimeError, "get fullscreen monitor");

        *outFullscreen = tempFullscreen;
#endif

        return true;
    }

    static auto windowIsDesktopFullscreen(const WindowHandle window, bool *outFullscreen) noexcept -> bool
    {
        RETURN_IF_NULL(window);
        RETURN_IF_NULL(outFullscreen);

        WindowData *data;
        if ( !getWindowData(window, &data) )
            return false;

        *outFullscreen = data->isDesktopFullscreen;
        return true;
    }

    auto window::isFullscreen(const WindowHandle window, bool *outFullscreen) noexcept -> bool
    {
        RETURN_IF_NULL(window);
        RETURN_IF_NULL(outFullscreen);

        bool desktopFullscreen{}, nativeFullscreen{};
        if ( !windowIsDesktopFullscreen(window, &desktopFullscreen) ||
            !windowIsNativeFullscreen(window, &nativeFullscreen) )
        {
            return false;
        }

        *outFullscreen = desktopFullscreen || nativeFullscreen;
        return true;
    }

    static auto setFullscreenNative(const WindowHandle window, bool fullscreen) noexcept -> bool
    {
        RETURN_IF_NULL(window);

#if KAZE_TARGET_MACOS
        return setWindowCocoaFullScreen(WIN_CAST(window), fullscreen);
#else
        WindowData *data;
        if ( !getWindowData(window, &data))
            return false;

        bool curFullscreen;
        if ( !backend::window::isFullscreen(window, &curFullscreen) )
            return false;

        if (fullscreen)
        {
            auto monitor = glfwGetPrimaryMonitor(); ERR_CHECK(Error::BE_RuntimeError, "get primary monitor");
            KAZE_ASSERT(monitor);

            auto mode = glfwGetVideoMode(monitor); ERR_CHECK(Error::BE_RuntimeError, "get video mode");


            window::getPosition(window, &data->last.rect.x, &data->last.rect.y);
            window::getSize(window, &data->last.rect.w, &data->last.rect.h);

            glfwSetWindowMonitor(WIN_CAST(window), monitor,
                0, 0, mode->width, mode->height, mode->refreshRate);
        }
        else
        {
            glfwSetWindowMonitor(WIN_CAST(window), nullptr,
                data->last.rect.x, data->last.rect.y,
                data->last.rect.w, data->last.rect.h,
                GLFW_DONT_CARE);
        }
#endif
    }

    static auto setFullscreenDesktop(const WindowHandle window, bool fullscreen) noexcept -> bool
    {
        KAZE_ASSERT(window);

        WindowData *data;
        if ( !getWindowData(window, &data) )
            return false;

        if (fullscreen)
        {
            const auto monitor = glfwGetPrimaryMonitor(); ERR_CHECK(Error::BE_RuntimeError, "get primary monitor");
            const auto mode = glfwGetVideoMode(monitor);  ERR_CHECK(Error::BE_RuntimeError, "get video mode for monitor");

            if ( !window::getPosition(window, &data->last.rect.x, &data->last.rect.y) )
                return false;
            if ( !window::getSize(window, &data->last.rect.w, &data->last.rect.h) )
                return false;
            bool bordered;
            if ( !window::isBordered(window, &bordered) )
                return false;

            if ( !window::setBordered(window, false) )
                return false;

            glfwSetWindowMonitor(WIN_CAST(window), nullptr,
                0, 0, mode->width, mode->height, GLFW_DONT_CARE);
            ERR_CHECK(Error::BE_RuntimeError, "set window monitor to null");

            data->last.decorated = bordered;
            data->isDesktopFullscreen = true;
        }
        else
        {
            glfwSetWindowMonitor(WIN_CAST(window), nullptr,
                data->last.rect.x, data->last.rect.y, data->last.rect.w, data->last.rect.h, GLFW_DONT_CARE);
            window::setBordered(window, data->last.decorated);
            data->isDesktopFullscreen = false;
        }

        return true;
    }

    auto window::setFullscreen(const WindowHandle window, const bool value) noexcept -> bool
    {
        bool curFullscreen;
        if ( !window::isFullscreen(window, &curFullscreen) )
            return false;

        if (curFullscreen == value) return true; // no need to adjust screen if its the same as current state

        WindowData *data;
        if ( !getWindowData(window, &data) )
            return false;

        if (data->fullscreenMode == FullscreenMode::Desktop)
        {
            setFullscreenDesktop(window, value);
        }
        else if (data->fullscreenMode == FullscreenMode::Native)
        {
            setFullscreenNative(window, value);
        }
        else
        {
            // this probably shouldn't ever occur
            KAZE_CORE_ERRCODE(Error::BE_LogicError, "window data fullscreen mode value is invalid");
            return false;
        }

        return true;
    }

    auto window::setFullscreenMode(const WindowHandle window, const FullscreenMode mode) noexcept -> bool
    {
        WindowData *data;
        if ( !getWindowData(window, &data) )
            return false;

        const auto curMode = data->fullscreenMode;
        if (curMode == mode) // no need to change mode if it's already the same as the target mode
            return true;

        if (curMode == FullscreenMode::Desktop)
        {
            if (data->isDesktopFullscreen)
            {
                if ( !setFullscreen(window, false) )
                    return false;

                data->fullscreenMode = mode;

                if ( !setFullscreen(window, true) )
                {
                    data->fullscreenMode = curMode; // revert back to original mode
                    setFullscreen(window, true);    // try to revert back to original
                    return false;
                }
            }
            else
            {
                data->fullscreenMode = mode;
            }
        }
        else if (curMode == FullscreenMode::Native)
        {
            bool isNativeFullscreen;
            if ( !isFullscreen(window, &isNativeFullscreen) )
                return false;

            if (isNativeFullscreen)
            {
                if ( !setFullscreen(window, false) )
                    return false;

                data->fullscreenMode = mode;

                if ( !setFullscreen(window, true) )
                {
                    data->fullscreenMode = curMode;
                    setFullscreen(window, true);   // try to revert back to original
                    return false;
                }
            }
            else
            {
                data->fullscreenMode = mode;
            }
        }

        return true;
    }

    auto window::getFullscreenMode(WindowHandle window, FullscreenMode *outMode) noexcept -> bool
    {
        RETURN_IF_NULL(window);
        RETURN_IF_NULL(outMode);

        WindowData *data;
        if ( !getWindowData(window, &data) )
            return false;

        *outMode = data->fullscreenMode;
        return true;
    }

    auto window::isBordered(const WindowHandle window, bool *outBordered) noexcept -> bool
    {
        RETURN_IF_NULL(window);
        RETURN_IF_NULL(outBordered);

        const auto bordered = static_cast<bool>(glfwGetWindowAttrib(WIN_CAST(window), GLFW_DECORATED));
        ERR_CHECK(Error::BE_RuntimeError, "check if window is decorated");

        *outBordered = bordered;
        return true;
    }

    auto window::setBordered(const WindowHandle window, const bool bordered) noexcept -> bool
    {
        RETURN_IF_NULL(window);

        glfwSetWindowAttrib(WIN_CAST(window), GLFW_DECORATED, bordered);
        ERR_CHECK(Error::BE_RuntimeError, "set window decorated attribute");

        return true;
    }

    auto window::isMinimized(const WindowHandle window, bool *outMinimized) noexcept -> bool
    {
        RETURN_IF_NULL(window);
        RETURN_IF_NULL(outMinimized);

        const auto minimized = static_cast<bool>(glfwGetWindowAttrib(WIN_CAST(window), GLFW_ICONIFIED));
        ERR_CHECK(Error::BE_RuntimeError, "get iconified/minimized attribute");

        *outMinimized = minimized;
        return true;
    }

    auto window::minimize(const WindowHandle window) noexcept -> bool
    {
        RETURN_IF_NULL(window);

        glfwIconifyWindow(WIN_CAST(window));
        ERR_CHECK(Error::BE_RuntimeError, "iconify/minimize window");

        return true;
    }

    auto window::isMaximized(const WindowHandle window, bool *outMaximized) noexcept -> bool
    {
        RETURN_IF_NULL(window);
        RETURN_IF_NULL(outMaximized);

        const auto maximized = static_cast<bool>(glfwGetWindowAttrib(WIN_CAST(window), GLFW_MAXIMIZED));
        ERR_CHECK(Error::BE_RuntimeError, "check if window is maximized");

        *outMaximized = maximized;
        return true;
    }

    auto window::maximize(const WindowHandle window) noexcept -> bool
    {
        RETURN_IF_NULL(window);

        glfwMaximizeWindow(WIN_CAST(window));
        ERR_CHECK(Error::BE_RuntimeError, "maximize window");

        return true;
    }

    auto window::restore(const WindowHandle window) noexcept -> bool
    {
        RETURN_IF_NULL(window);

        glfwRestoreWindow(WIN_CAST(window));
        ERR_CHECK(Error::BE_RuntimeError, "restore window");

        return true;
    }

    auto window::getPosition(const WindowHandle window, int *x, int *y) noexcept -> bool
    {
        RETURN_IF_NULL(window);

        glfwGetWindowPos(WIN_CAST(window), x, y);
        ERR_CHECK(Error::BE_RuntimeError, "get window position");

        return true;
    }

    auto window::setPosition(const WindowHandle window, const int x, const int y) noexcept -> bool
    {
        RETURN_IF_NULL(window);

        glfwSetWindowPos(WIN_CAST(window), x, y);
        ERR_CHECK(Error::BE_RuntimeError, "set window position");

        return true;
    }

    auto window::getResizable(const WindowHandle window, bool *outResizable) noexcept -> bool
    {
        RETURN_IF_NULL(window);
        RETURN_IF_NULL(outResizable);

        const auto resizable = static_cast<bool>(glfwGetWindowAttrib(WIN_CAST(window), GLFW_RESIZABLE));
        ERR_CHECK(Error::BE_RuntimeError, "get window resizable attribute");

        *outResizable = resizable;
        return true;
    }

    auto window::setResizable(const WindowHandle window, const bool resizable) noexcept -> bool
    {
        RETURN_IF_NULL(window);

        glfwSetWindowAttrib(WIN_CAST(window), GLFW_RESIZABLE, resizable);
        ERR_CHECK(Error::BE_RuntimeError, "set window resizable attribute");

        return true;
    }

    auto window::isHidden(const WindowHandle window, bool *outHidden) noexcept -> bool
    {
        RETURN_IF_NULL(window);
        RETURN_IF_NULL(outHidden);

        const auto visible = static_cast<bool>(glfwGetWindowAttrib(WIN_CAST(window), GLFW_VISIBLE));
        ERR_CHECK(Error::BE_RuntimeError, "get window visible attribute");

        *outHidden = !visible;
        return true;
    }

    auto window::setHidden(const WindowHandle window, const bool hidden) noexcept -> bool
    {
        RETURN_IF_NULL(window);

        if (hidden)
        {
            glfwHideWindow(WIN_CAST(window));
            ERR_CHECK(Error::BE_RuntimeError, "hide window");
        }
        else
        {
            glfwShowWindow(WIN_CAST(window));
            ERR_CHECK(Error::BE_RuntimeError, "show window");
        }

        return true;
    }

    auto window::isHovered(const WindowHandle window, bool *outHovered) noexcept -> bool
    {
        RETURN_IF_NULL(window);
        RETURN_IF_NULL(outHovered);

        const auto hovered = static_cast<bool>(glfwGetWindowAttrib(WIN_CAST(window), GLFW_HOVERED));
        ERR_CHECK(Error::BE_RuntimeError, "get window hovered attribute");

        *outHovered = hovered;
        return true;
    }

    auto window::isFloating(const WindowHandle window, bool *outFloating) noexcept -> bool
    {
        RETURN_IF_NULL(window);
        RETURN_IF_NULL(outFloating);

        const auto floating = static_cast<bool>(glfwGetWindowAttrib(WIN_CAST(window), GLFW_FLOATING));
        ERR_CHECK(Error::BE_RuntimeError, "get window floating attribute");

        *outFloating = floating;
        return true;
    }

    auto window::setFloating(const WindowHandle window, const bool floating) noexcept -> bool
    {
        RETURN_IF_NULL(window);

        glfwSetWindowAttrib(WIN_CAST(window), GLFW_FLOATING, floating);
        ERR_CHECK(Error::BE_RuntimeError, "set window floating attribute");

        return true;
    }

    auto window::isTransparent(const WindowHandle window, bool *outTransparent) noexcept -> bool
    {
        RETURN_IF_NULL(window);
        RETURN_IF_NULL(outTransparent);

        const auto transparent = static_cast<bool>(glfwGetWindowAttrib(WIN_CAST(window), GLFW_TRANSPARENT_FRAMEBUFFER));
        ERR_CHECK(Error::BE_RuntimeError, "get window transparent framebuffer attribute");

        *outTransparent = transparent;
        return true;
    }

    auto window::setTransparent(const WindowHandle window, const bool transparent) noexcept -> bool
    {
        RETURN_IF_NULL(window);

        glfwSetWindowAttrib(WIN_CAST(window), GLFW_TRANSPARENT_FRAMEBUFFER, transparent);
        ERR_CHECK(Error::BE_RuntimeError, "set window transparent framebuffer attribute");

        return true;
    }

    auto window::isFocused(const WindowHandle window, bool *outFocused) noexcept -> bool
    {
        RETURN_IF_NULL(window);
        RETURN_IF_NULL(outFocused);

        const auto focused = static_cast<bool>(glfwGetWindowAttrib(WIN_CAST(window), GLFW_FOCUSED));
        ERR_CHECK(Error::BE_RuntimeError, "get window focused attribute");

        *outFocused = focused;
        return true;
    }

    auto window::focus(const WindowHandle window) noexcept -> bool
    {
        RETURN_IF_NULL(window);

        glfwFocusWindow(WIN_CAST(window));
        ERR_CHECK(Error::BE_RuntimeError, "focus window");

        return true;
    }

    auto window::setMinSize(const WindowHandle window, const int minWidth, const int minHeight) noexcept -> bool
    {
        RETURN_IF_NULL(window);

        WindowData *data;
        if ( !getWindowData(window, &data) )
            return false;

        glfwSetWindowSizeLimits(WIN_CAST(window),
            minWidth, minHeight, data->last.maxSize.x, data->last.maxSize.y);
        ERR_CHECK(Error::BE_RuntimeError, "set window size limits");

        data->last.minSize = { minWidth, minHeight };
        return true;
    }

    auto window::setMaxSize(const WindowHandle window, const int maxWidth, const int maxHeight) noexcept -> bool
    {
        RETURN_IF_NULL(window);

        WindowData *data;
        if ( !getWindowData(window, &data) )
            return false;

        glfwSetWindowSizeLimits(WIN_CAST(window),
            data->last.minSize.x, data->last.minSize.y, maxWidth, maxHeight);
        ERR_CHECK(Error::BE_RuntimeError, "set window size limits");

        data->last.maxSize = { maxWidth, maxHeight };
        return true;
    }

    auto window::getMinSize(const WindowHandle window, int *outMinWidth, int *outMinHeight) noexcept -> bool
    {
        RETURN_IF_NULL(window);

        WindowData *data;
        if ( !getWindowData(window, &data) )
            return false;

        if (outMinWidth)
            *outMinWidth = data->last.minSize.x;
        if (outMinHeight)
            *outMinHeight = data->last.minSize.y;
        return true;
    }

    auto window::getMaxSize(const WindowHandle window, int *outMaxWidth, int *outMaxHeight) noexcept -> bool
    {
        RETURN_IF_NULL(window);

        WindowData *data;
        if ( !getWindowData(window, &data) )
            return false;

        if (outMaxWidth)
            *outMaxWidth = data->last.maxSize.x;
        if (outMaxHeight)
            *outMaxHeight = data->last.maxSize.y;
        return true;
    }

    auto window::setShowCursorMode(const WindowHandle window, const bool show) noexcept -> bool
    {
        RETURN_IF_NULL(window);

        glfwSetInputMode(WIN_CAST(window), GLFW_CURSOR_HIDDEN, !show);
        ERR_CHECK(Error::BE_RuntimeError, "set cursor hidden mode");

        return true;
    }

    auto window::getShowCursorMode(const WindowHandle window, bool *outShow) noexcept -> bool
    {
        RETURN_IF_NULL(window);
        RETURN_IF_NULL(outShow);

        const auto showCursor = !static_cast<bool>(glfwGetInputMode(WIN_CAST(window), GLFW_CURSOR_HIDDEN));
        ERR_CHECK(Error::BE_RuntimeError, "get cursor hidden mode");

        *outShow = showCursor;
        return true;
    }

    auto window::setCaptureCursorMode(const WindowHandle window, const bool capture) noexcept -> bool
    {
        RETURN_IF_NULL(window);

        glfwSetInputMode(WIN_CAST(window), GLFW_CURSOR_CAPTURED, capture);
        ERR_CHECK(Error::BE_RuntimeError, "set cursor captured mode");

        return true;
    }

    auto window::getCaptureCursorMode(const WindowHandle window, bool *outCapture) noexcept -> bool
    {
        RETURN_IF_NULL(window);
        RETURN_IF_NULL(outCapture);

        const auto capture = static_cast<bool>(glfwGetInputMode(WIN_CAST(window), GLFW_CURSOR_CAPTURED));
        ERR_CHECK(Error::BE_RuntimeError, "get cursor captured mode");

        *outCapture = capture;
        return true;
    }

    auto keyboard::isDown(Key key, bool *outDown) noexcept -> bool
    {
        RETURN_IF_NULL(outDown);

        const auto window = glfwGetCurrentContext();
        if ( !window )
        {
            KAZE_CORE_ERRCODE(Error::BE_RuntimeError, "failed to get current window context, no window was open");
            return false;
        }

        const auto down = glfwGetKey(window, s_keyToGlfwKey[ static_cast<Int>(key) ]) == GLFW_PRESS;
        ERR_CHECK(Error::BE_RuntimeError, "get key down");

        *outDown = down;
        return true;
    }

    auto mouse::getRelativePosition(const WindowHandle window, float *outX, float *outY) noexcept -> bool
    {
        RETURN_IF_NULL(window);

        double tempX=0, tempY=0;
        glfwGetCursorPos(WIN_CAST(window), &tempX, &tempY);
        ERR_CHECK(Error::BE_RuntimeError, "get cursor position");

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
            glfwGetCursorPos(window, &tempX, &tempY); ERR_CHECK(Error::BE_RuntimeError, "get cursor position");

            int winX=0, winY=0;
            glfwGetWindowPos(window, &winX, &winY);   ERR_CHECK(Error::BE_RuntimeError, "get window position");

            tempX += static_cast<double>(winX);
            tempY += static_cast<double>(winY);
        }
        else
        {
            KAZE_CORE_ERRCODE(Error::BE_RuntimeError, "Failed to get current context: {}", getGlfwErrorStr());
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

        bool connected = glfwJoystickPresent(index); ERR_CHECK(Error::BE_RuntimeError, "check joystick is present");
        if ( !connected )
        {
            *outConnected = false;
            return true;
        }

        connected = glfwJoystickIsGamepad(index);   ERR_CHECK(Error::BE_RuntimeError, "check if joystick is gamepad");

        *outConnected = connected;
        return true;
    }

    auto gamepad::isDown(const int index, const GamepadBtn button, bool *outDown) noexcept -> bool
    {
        GP_INDEX_IN_RANGE(index);
        RETURN_IF_NULL(outDown);

        *outDown = s_gamepads[index].getCurrentState().buttons[ s_gamepadButtonToGlfw[static_cast<int>(button)] ];
        return true;
    }

    auto gamepad::isJustDown(const int index, const GamepadBtn button, bool *outJustDown) noexcept -> bool
    {
        GP_INDEX_IN_RANGE(index);
        RETURN_IF_NULL(outJustDown);

        const auto glfwButton = s_gamepadButtonToGlfw[static_cast<int>(button)];
        const auto &gamepad = s_gamepads[index];
        *outJustDown = gamepad.getCurrentState().buttons[glfwButton] &&
            !gamepad.getLastState().buttons[glfwButton];
        return true;
    }

    auto gamepad::isJustUp(const int index, const GamepadBtn button, bool *outJustUp) noexcept -> bool
    {
        GP_INDEX_IN_RANGE(index);
        RETURN_IF_NULL(outJustUp);

        const auto glfwButton = s_gamepadButtonToGlfw[static_cast<int>(button)];
        const auto &gamepad = s_gamepads[index];
        *outJustUp = !gamepad.getCurrentState().buttons[glfwButton] &&
            gamepad.getLastState().buttons[glfwButton];
        return true;
    }

    auto gamepad::getAxis(const int index, const GamepadAxis axis, float *outValue) noexcept -> bool
    {
        GP_INDEX_IN_RANGE(index);
        RETURN_IF_NULL(outValue);

        *outValue = s_gamepads[index].getCurrentState().axes[ s_gamepadAxisToGlfw[static_cast<Uint8>(axis)] ];
        return true;
    }

    auto gamepad::getAxisMoved(const int index, const GamepadAxis axis, const float deadzone, bool *outMoved) noexcept -> bool
    {
        GP_INDEX_IN_RANGE(index);
        RETURN_IF_NULL(outMoved);

        const auto glfwAxis = s_gamepadAxisToGlfw[static_cast<Uint8>(axis)];
        auto value = s_gamepads[index].getCurrentState().axes[glfwAxis];
        auto lastValue = s_gamepads[index].getLastState().axes[glfwAxis];

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

        *outMoved = valueX != valueLastX || valueY != valueLastY;
        return true;
    }

}

KAZE_NAMESPACE_END
