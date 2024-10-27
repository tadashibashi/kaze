/// \file Window_glfw3.cpp
/// Implementation file for GLFW3 window-related backend functions
#include "window_glfw3.h"
#include "common_glfw3.h"

#include <kaze/core/platform/backend/backend.h>
#include <kaze/core/platform/backend/window.h>
#include <kaze/core/platform/defines.h>

#include <GLFW/glfw3.h>

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

#if !KAZE_TARGET_EMSCRIPTEN
#include <GLFW/glfw3native.h>
#endif

KAZE_NAMESPACE_BEGIN

namespace backend {
    window::NativePlatformData window::getNativeInfo(const WindowHandle window) noexcept
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

    /// Private helper to safely grab window data associated with window
    /// \param[in]  window  window to get data associated to
    /// \param[out] outData retrieves pointer to data
    /// \returns true if successfully retrieved, and false if missing or unsuccessfully retrieved
    auto getWindowData(const WindowHandle window, WindowData **outData) noexcept -> bool
    {
        KAZE_ASSERT(window);
        KAZE_ASSERT(outData);

        WindowData *data;
        if ( !windows.getData(window, &data) )
            return false;
        if ( !data )
        {
            KAZE_CORE_ERRCODE(Error::BE_LogicError, "missing window data");
            return false;
        }

        *outData = data;
        return true;
    }

    // ===== Window Callbacks =====

    static auto glfwWindowCloseCallback(GLFWwindow *window) -> void
    {
        events.emit(WindowEvent {
            .type = WindowEvent::Closed,
            .window = window,
        });
    }

    static auto glfwScrollCallback(GLFWwindow *window, const double xoffset, const double yoffset) -> void
    {
        events.emit(MouseScrollEvent {
            .offset = {
                static_cast<float>(xoffset),
                static_cast<float>(yoffset)
            },
            .window = window,
        });
    }

    static auto glfwCursorPosCallback(GLFWwindow *window, const double x, const double y) -> void
    {
        WindowData *data;
        if ( !getWindowData(window, &data) )
            return;

        int w, h;
        glfwGetWindowSize(window, &w, &h);

        if (!data->isCapture)
        {
            if (x >= 0 && x < w && y >= 0 && y < h) // this matches sdl3's behavior
            {
                events.emit(MouseMotionEvent {
                   .position = {
                       static_cast<Float>(x),
                       static_cast<Float>(y),
                   },
                   .relative = {
                        static_cast<Float>(x - data->lastCursorPos.x),
                        static_cast<Float>(y - data->lastCursorPos.y),
                   },
                   .window = window,
                });
            }

            data->lastCursorPos = {x, y};
        }
        else
        {
            events.emit(MouseMotionEvent {
                .position = {
                    static_cast<Float>(x),
                    static_cast<Float>(y),
                },
                .relative = {
                    static_cast<Float>(x - w/2.0),
                    static_cast<Float>(y - h/2.0),
                },
                .window = window,
            });

            glfwSetCursorPos(window, w/2.0, h/2.0);
            data->relCursorPos += Vec2d{x, y};
        }
    }

    static auto glfwKeyCallback(GLFWwindow *window, const int key, const int scancode, const int action, const int mods) -> void
    {
        events.emit(KeyboardEvent {
            .key = backend::toKey(key),
            .isDown = (action == GLFW_PRESS),
            .isRepeat = (action == GLFW_REPEAT),
            .window = static_cast<void *>(window)
        });
    }


    static auto glfwWindowSizeCallback(GLFWwindow *window, const int x, const int y) -> void
    {
        events.emit(WindowEvent {
            .type = WindowEvent::Resized,
            .data0 = x,
            .data1 = y,
            .window = window,
        });
    }

    static auto glfwFramebufferSize(GLFWwindow *window, const int x, const int y) -> void
    {
        events.emit(WindowEvent {
            .type = WindowEvent::ResizedFramebuffer,
            .data0 = x,
            .data1 = y,
            .window = window,
        });
    }

    static auto glfwWindowPosCallback(GLFWwindow *window, const int x, const int y) -> void
    {
        events.emit(WindowEvent {
            .type = WindowEvent::Moved,
            .data0 = x,
            .data1 = y,
            .window = window,
        });
    }

    static auto glfwWindowFocusCallback(GLFWwindow *window, const int focus) -> void
    {
        events.emit(WindowEvent {
            .type = focus == 0 ? WindowEvent::FocusLost : WindowEvent::FocusGained,
            .window = window,
        });
    }

    static auto glfwWindowMaximizeCallback(GLFWwindow *window, const int maximize) -> void
    {
        events.emit(WindowEvent {
            .type = maximize == 0 ? WindowEvent::Restored : WindowEvent::Maximized,
            .window = window,
        });
    }

    static auto glfwWindowIconifyCallback(GLFWwindow *window, const int iconify) -> void
    {
        events.emit(WindowEvent {
            .type = iconify == 0 ? WindowEvent::Restored : WindowEvent::Minimized,
            .window = window,
        });
    }

    static auto glfwDropCallback(GLFWwindow *window, int count, const char **paths) -> void
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


    static auto toMouseBtn(Uint button) -> MouseBtn
    {
        if (button > GLFW_MOUSE_BUTTON_LAST)
            return MouseBtn::Count;
        return static_cast<MouseBtn>(button);
    }

    static auto glfwMouseButtonCallback(GLFWwindow *window, const int button, const int action, const int mods) -> void
    {
        events.emit(MouseButtonEvent {
            .button = toMouseBtn(button),
            .isDown = action == GLFW_PRESS,
            .window = window,
        });
    }

    static auto glfwCursorEnterCallback(GLFWwindow *window, const int entered) -> void
    {
        events.emit(WindowEvent {
            .type = entered == 0 ? WindowEvent::MouseExited : WindowEvent::MouseEntered,
            .window = window,
        });
    }

    static auto glfwTextInputCallback(GLFWwindow *window, const uint32_t codepoint) -> void
    {
        events.emit(TextInputEvent {
            .codepoint = codepoint,
            .window = window,
        });
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

        const auto window = glfwCreateWindow(width, height, title, nullptr, nullptr); ERR_CHECK(Error::BE_RuntimeErr, "create GLFWwindow");
        if ( !window )
        {
            KAZE_CORE_ERRCODE(Error::BE_RuntimeErr, "Failed to create window: {}", getGlfwErrorStr());
            return false;
        }

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
        glfwSetCharCallback           ( window, glfwTextInputCallback );   WARN_CHECK("set window char callback");

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

        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);

        if ( !windows.emplace(window, {
            .lastCursorPos = {mouseX, mouseY}
        }) )
        {
            glfwDestroyWindow(window); ERR_CHECK(Error::BE_RuntimeErr, "clean up window after failed data emplacement");
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
        if ( !windows.erase(window, &wasErased) )
            return false;

        if ( !wasErased )
        {
            KAZE_CORE_ERRCODE(Error::BE_InvalidWindowHandle, "WindowHandle was invalid");
            return false;
        }

        glfwDestroyWindow(WIN_CAST(window)); ERR_CHECK(Error::BE_RuntimeErr, "destroy window");

        return true;
    }

    auto window::isOpen(const WindowHandle window, bool *outIsOpen) noexcept -> bool
    {
        RETURN_IF_NULL(window);
        return windows.contains(window, outIsOpen);
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
        glfwSetWindowTitle(WIN_CAST(window), title); ERR_CHECK(Error::BE_RuntimeErr, "set the window title");

        return true;
    }

    auto window::getTitle(const WindowHandle window, const char **outTitle) noexcept -> bool
    {
        RETURN_IF_NULL(window);
        RETURN_IF_NULL(outTitle);

        const auto title = glfwGetWindowTitle(WIN_CAST(window));
        if ( !title )
        {
            KAZE_CORE_ERRCODE(Error::BE_RuntimeErr, "Failed to get window title: {}", getGlfwErrorStr());
            return false;
        }

        *outTitle = title;
        return true;
    }

    auto window::getFramebufferSize(const WindowHandle window, int *outWidth, int *outHeight) noexcept -> bool
    {
        RETURN_IF_NULL(window);

        glfwGetFramebufferSize(WIN_CAST(window), outWidth, outHeight); ERR_CHECK(Error::BE_RuntimeErr, "get framebuffer size");

        return true;
    }

    auto window::setSize(const WindowHandle window, const int width, const int height) noexcept -> bool
    {
        RETURN_IF_NULL(window);

        glfwSetWindowSize(WIN_CAST(window), width, height);
        ERR_CHECK(Error::BE_RuntimeErr, "set logical window size");

        return true;
    }

    auto window::getSize(const WindowHandle window, int *outWidth, int *outHeight) noexcept -> bool
    {
        RETURN_IF_NULL(window);

        glfwGetWindowSize(WIN_CAST(window), outWidth, outHeight);
        ERR_CHECK(Error::BE_RuntimeErr, "get logical window size");

        return true;
    }

    static auto windowIsNativeFullscreen(const WindowHandle window, bool *outFullscreen) noexcept -> bool
    {
        RETURN_IF_NULL(window);
        RETURN_IF_NULL(outFullscreen);

#if KAZE_TARGET_MACOS
        if ( !getWindowCocoaFullscreen(WIN_CAST(window), outFullscreen) )
            return false;
#else
        const auto tempFullscreen = static_cast<bool>(glfwGetWindowMonitor(WINCAST(window)));
        ERR_CHECK(Error::BE_RuntimeErr, "get fullscreen monitor");

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
        return setWindowCocoaFullscreen(WIN_CAST(window), fullscreen);
#else
        WindowData *data;
        if ( !getWindowData(window, &data))
            return false;

        bool curFullscreen;
        if ( !backend::window::isFullscreen(window, &curFullscreen) )
            return false;

        if (fullscreen)
        {
            auto monitor = glfwGetPrimaryMonitor(); ERR_CHECK(Error::BE_RuntimeErr, "get primary monitor");
            KAZE_ASSERT(monitor);

            auto mode = glfwGetVideoMode(monitor); ERR_CHECK(Error::BE_RuntimeErr, "get video mode");


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
            const auto monitor = glfwGetPrimaryMonitor(); ERR_CHECK(Error::BE_RuntimeErr, "get primary monitor");
            const auto mode = glfwGetVideoMode(monitor);  ERR_CHECK(Error::BE_RuntimeErr, "get video mode for monitor");

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
            ERR_CHECK(Error::BE_RuntimeErr, "set window monitor to null");

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
        ERR_CHECK(Error::BE_RuntimeErr, "check if window is decorated");

        *outBordered = bordered;
        return true;
    }

    auto window::setBordered(const WindowHandle window, const bool bordered) noexcept -> bool
    {
        RETURN_IF_NULL(window);

        glfwSetWindowAttrib(WIN_CAST(window), GLFW_DECORATED, bordered);
        ERR_CHECK(Error::BE_RuntimeErr, "set window decorated attribute");

        return true;
    }

    auto window::isMinimized(const WindowHandle window, bool *outMinimized) noexcept -> bool
    {
        RETURN_IF_NULL(window);
        RETURN_IF_NULL(outMinimized);

        const auto minimized = static_cast<bool>(glfwGetWindowAttrib(WIN_CAST(window), GLFW_ICONIFIED));
        ERR_CHECK(Error::BE_RuntimeErr, "get iconified/minimized attribute");

        *outMinimized = minimized;
        return true;
    }

    auto window::minimize(const WindowHandle window) noexcept -> bool
    {
        RETURN_IF_NULL(window);

        glfwIconifyWindow(WIN_CAST(window));
        ERR_CHECK(Error::BE_RuntimeErr, "iconify/minimize window");

        return true;
    }

    auto window::isMaximized(const WindowHandle window, bool *outMaximized) noexcept -> bool
    {
        RETURN_IF_NULL(window);
        RETURN_IF_NULL(outMaximized);

        const auto maximized = static_cast<bool>(glfwGetWindowAttrib(WIN_CAST(window), GLFW_MAXIMIZED));
        ERR_CHECK(Error::BE_RuntimeErr, "check if window is maximized");

        *outMaximized = maximized;
        return true;
    }

    auto window::maximize(const WindowHandle window) noexcept -> bool
    {
        RETURN_IF_NULL(window);

        glfwMaximizeWindow(WIN_CAST(window));
        ERR_CHECK(Error::BE_RuntimeErr, "maximize window");

        return true;
    }

    auto window::restore(const WindowHandle window) noexcept -> bool
    {
        RETURN_IF_NULL(window);

        glfwRestoreWindow(WIN_CAST(window));
        ERR_CHECK(Error::BE_RuntimeErr, "restore window");

        return true;
    }

    auto window::getPosition(const WindowHandle window, int *x, int *y) noexcept -> bool
    {
        RETURN_IF_NULL(window);

        glfwGetWindowPos(WIN_CAST(window), x, y);
        ERR_CHECK(Error::BE_RuntimeErr, "get window position");

        return true;
    }

    auto window::setPosition(const WindowHandle window, const int x, const int y) noexcept -> bool
    {
        RETURN_IF_NULL(window);

        glfwSetWindowPos(WIN_CAST(window), x, y);
        ERR_CHECK(Error::BE_RuntimeErr, "set window position");

        return true;
    }

    auto window::getResizable(const WindowHandle window, bool *outResizable) noexcept -> bool
    {
        RETURN_IF_NULL(window);
        RETURN_IF_NULL(outResizable);

        const auto resizable = static_cast<bool>(glfwGetWindowAttrib(WIN_CAST(window), GLFW_RESIZABLE));
        ERR_CHECK(Error::BE_RuntimeErr, "get window resizable attribute");

        *outResizable = resizable;
        return true;
    }

    auto window::setResizable(const WindowHandle window, const bool resizable) noexcept -> bool
    {
        RETURN_IF_NULL(window);

        glfwSetWindowAttrib(WIN_CAST(window), GLFW_RESIZABLE, resizable);
        ERR_CHECK(Error::BE_RuntimeErr, "set window resizable attribute");

        return true;
    }

    auto window::isHidden(const WindowHandle window, bool *outHidden) noexcept -> bool
    {
        RETURN_IF_NULL(window);
        RETURN_IF_NULL(outHidden);

        const auto visible = static_cast<bool>(glfwGetWindowAttrib(WIN_CAST(window), GLFW_VISIBLE));
        ERR_CHECK(Error::BE_RuntimeErr, "get window visible attribute");

        *outHidden = !visible;
        return true;
    }

    auto window::setHidden(const WindowHandle window, const bool hidden) noexcept -> bool
    {
        RETURN_IF_NULL(window);

        if (hidden)
        {
            glfwHideWindow(WIN_CAST(window));
            ERR_CHECK(Error::BE_RuntimeErr, "hide window");
        }
        else
        {
            glfwShowWindow(WIN_CAST(window));
            ERR_CHECK(Error::BE_RuntimeErr, "show window");
        }

        return true;
    }

    auto window::isHovered(const WindowHandle window, bool *outHovered) noexcept -> bool
    {
        RETURN_IF_NULL(window);
        RETURN_IF_NULL(outHovered);

        const auto hovered = static_cast<bool>(glfwGetWindowAttrib(WIN_CAST(window), GLFW_HOVERED));
        ERR_CHECK(Error::BE_RuntimeErr, "get window hovered attribute");

        *outHovered = hovered;
        return true;
    }

    auto window::isFloating(const WindowHandle window, bool *outFloating) noexcept -> bool
    {
        RETURN_IF_NULL(window);
        RETURN_IF_NULL(outFloating);

        const auto floating = static_cast<bool>(glfwGetWindowAttrib(WIN_CAST(window), GLFW_FLOATING));
        ERR_CHECK(Error::BE_RuntimeErr, "get window floating attribute");

        *outFloating = floating;
        return true;
    }

    auto window::setFloating(const WindowHandle window, const bool floating) noexcept -> bool
    {
        RETURN_IF_NULL(window);

        glfwSetWindowAttrib(WIN_CAST(window), GLFW_FLOATING, floating);
        ERR_CHECK(Error::BE_RuntimeErr, "set window floating attribute");

        return true;
    }

    auto window::isTransparent(const WindowHandle window, bool *outTransparent) noexcept -> bool
    {
        RETURN_IF_NULL(window);
        RETURN_IF_NULL(outTransparent);

        const auto transparent = static_cast<bool>(glfwGetWindowAttrib(WIN_CAST(window), GLFW_TRANSPARENT_FRAMEBUFFER));
        ERR_CHECK(Error::BE_RuntimeErr, "get window transparent framebuffer attribute");

        *outTransparent = transparent;
        return true;
    }

    auto window::setTransparent(const WindowHandle window, const bool transparent) noexcept -> bool
    {
        RETURN_IF_NULL(window);

        glfwSetWindowAttrib(WIN_CAST(window), GLFW_TRANSPARENT_FRAMEBUFFER, transparent);
        ERR_CHECK(Error::BE_RuntimeErr, "set window transparent framebuffer attribute");

        return true;
    }

    auto window::isFocused(const WindowHandle window, bool *outFocused) noexcept -> bool
    {
        RETURN_IF_NULL(window);
        RETURN_IF_NULL(outFocused);

        const auto focused = static_cast<bool>(glfwGetWindowAttrib(WIN_CAST(window), GLFW_FOCUSED));
        ERR_CHECK(Error::BE_RuntimeErr, "get window focused attribute");

        *outFocused = focused;
        return true;
    }

    auto window::focus(const WindowHandle window) noexcept -> bool
    {
        RETURN_IF_NULL(window);

        glfwFocusWindow(WIN_CAST(window));
        ERR_CHECK(Error::BE_RuntimeErr, "focus window");

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
        ERR_CHECK(Error::BE_RuntimeErr, "set window size limits");

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
        ERR_CHECK(Error::BE_RuntimeErr, "set window size limits");

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

    auto window::setCursorMode(WindowHandle window, CursorMode mode) noexcept -> bool
    {
        RETURN_IF_NULL(window);

        WindowData *data;
        if ( !getWindowData(window, &data) ) return false;

        int glfwCursorMode = GLFW_CURSOR_NORMAL;
        switch(mode)
        {
        case CursorMode::Visible: glfwCursorMode = GLFW_CURSOR_NORMAL; break;
        case CursorMode::Hidden: glfwCursorMode = GLFW_CURSOR_HIDDEN; break;
        case CursorMode::Relative: glfwCursorMode = GLFW_CURSOR_HIDDEN; break;
        default:
            KAZE_CORE_ERRCODE(Error::InvalidEnum, "Unknown `CursorMode` passed to `window::setCurosrMode`");
            return false;
        }

        data->isCapture = (mode == CursorMode::Relative);

        glfwSetInputMode(WIN_CAST(window), GLFW_CURSOR, glfwCursorMode);
        ERR_CHECK(Error::BE_RuntimeErr, "set cursor mode");

        return true;
    }

    auto window::getCursorMode(WindowHandle window, CursorMode *outMode) noexcept -> bool
    {
        RETURN_IF_NULL(window);
        RETURN_IF_NULL(outMode);

        WindowData *data;
        if ( !getWindowData(window, &data) ) return false;

        if (data->isCapture)
        {
            *outMode = CursorMode::Relative;
            return true;
        }

        const auto glfwCursorMode = glfwGetInputMode(WIN_CAST(window), GLFW_CURSOR);
        switch(glfwCursorMode)
        {
        case GLFW_CURSOR_NORMAL:   *outMode = CursorMode::Visible; break;
        case GLFW_CURSOR_HIDDEN:   *outMode = CursorMode::Hidden; break;
        default:
            KAZE_CORE_ERRCODE(Error::BE_RuntimeErr,
                "Internal error: unsupported or unknown cursor mode retrieved from window: {}",
                glfwCursorMode);
            return false;
        }

        return true;
    }

    auto window::setTextInputMode(WindowHandle window, bool yes) noexcept -> bool
    {
        RETURN_IF_NULL(window);

        // does nothing right now (when IME is supported initiate it here)

        return true;
    }

    auto window::isTextInputActive(WindowHandle window, bool *outValue) noexcept -> bool
    {
        // not implemented yet, always on
        RETURN_IF_NULL(window);
        RETURN_IF_NULL(outValue);

        *outValue = true;
        return true;
    }

} // namespace backend

KAZE_NAMESPACE_END
