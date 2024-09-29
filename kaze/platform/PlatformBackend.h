#pragma once

#ifndef kaze_platform_platformbackend_h_
#define kaze_platform_platformbackend_h_

#include <kaze/kaze.h>
#include "PlatformEvent.h"
#include "Window.h"
#include "Gamepad.h"

KAZE_NAMESPACE_BEGIN

namespace backend {
    struct NativePlatformData {
        void *windowHandle;
        void *displayType;
    };

    class PlatformBackend
    {
    public:
        static bool init() noexcept;

        static void shutdown() noexcept;

        /// Get time in seconds since init was first called.
        /// Most modern platforms and backends support nanosecond granularity.
        [[nodiscard]] static Double getTime() noexcept;

        static void pollEvents();

        static void setCallbacks(const PlatformCallbacks &callbacks)
        {
            events = callbacks;
        }

        // ===== Window =====

        [[nodiscard]] static void *windowCreate(const char *title, size_t width, size_t height, WindowInit::Flags initFlags = WindowInit::None);

        [[nodiscard]] static NativePlatformData windowGetNativeInfo(void *window);

        /// Close a window. The client passing the pointer hands over responsibility to this function
        /// to handle all clean up/freeing of the window internals and the memory at the pointer.
        static void windowDestroy(void *window);

        /// Check if a window is open and valid. It may be a dangling pointer,
        /// so this function should account for that possibility.
        [[nodiscard]] static bool windowIsOpen(void *window);

        /// Set a user pointer to associate with the window
        static void windowSetUserData(void *window, void *data);
        /// Get the user pointer associated with the window
        static void *windowGetUserData(void *window);

        static void windowSetTitle(void *window, const char *title);

        /// Get the window title, return null if there are any errors
        [[nodiscard]] static const char *windowGetTitle(void *window) noexcept;

        static void windowSetSize(void *window, int x, int y);
        /// Get the virtual size of the window, return {0, 0} if there ar any errors, it shouldn't throw'
        static void windowGetSize(void *window, int *x, int *y) noexcept;

        static void windowGetDisplaySize(void *window, int *x, int *y) noexcept;

        [[nodiscard]] static bool windowIsNativeFullscreen(void *window) noexcept;
        static void windowSetNativeFullscreen(void *window, bool value);

        [[nodiscard]] static bool windowIsDesktopFullscreen(void *window) noexcept;
        static void windowSetDesktopFullscreen(void *window, bool value);

        [[nodiscard]] static bool windowIsBorderless(void *window) noexcept;
        static void windowSetBorderless(void *window, bool value);

        [[nodiscard]] static bool windowIsMinimized(void *window) noexcept;
        static void windowMinimize(void *window);
        [[nodiscard]] static bool windowIsMaximized(void *window) noexcept;
        static void windowMaximize(void *window);
        static void windowRestore(void *window);

        static void windowGetPosition(void *window, int *x, int *y) noexcept;
        static void windowSetPosition(void *window, int x, int y) noexcept;

        [[nodiscard]] static bool windowGetResizable(void *window) noexcept;
        static void windowSetResizable(void *window, bool value);

        [[nodiscard]] static bool windowIsHidden(void *window) noexcept;
        static void windowSetHidden(void *window, bool value);

        [[nodiscard]] static bool windowIsHovered(void *window) noexcept;

        [[nodiscard]] static bool windowIsFloating(void *window) noexcept;
        static void windowSetFloating(void *window, bool value);

        [[nodiscard]] static bool windowIsTransparent(void *window) noexcept;
        static void windowSetTransparent(void *window, bool value);

        [[nodiscard]] static bool windowIsFocused(void *window) noexcept;
        static void windowFocus(void *window);

        static void windowSetMinimumSize(void *window, int minWidth, int minHeight);
        static void windowSetMaximumSize(void *window, int maxWidth, int maxHeight);
        static void windowGetMinimumSize(void *window, int *minWidth, int *minHeight);
        static void windowGetMaximumSize(void *window, int *maxWidth, int *maxHeight);

        // TODO: Capture mouse in window?
        // TODO: Hide/Show Cursor
        // TODO: Add mouse enter and exit window events

        // ===== Gamepad functions =====
        [[nodiscard]] static bool gamepadIsConnected(int index) noexcept;
        [[nodiscard]] static bool gamepadIsButtonDown(int index, GamepadBtn button);
        [[nodiscard]] static bool gamepadIsButtonJustDown(int index, GamepadBtn button);
        [[nodiscard]] static bool gamepadIsButtonJustUp(int index, GamepadBtn button);
        [[nodiscard]] static float gamepadGetAxis(int index, GamepadAxis axis);
        [[nodiscard]] static bool gamepadDidAxisMove(int index, GamepadAxis axis, float deadzone);
        /// Deadzone is the resultant length of axisX and axisY
        [[nodiscard]] static bool gamepadDidAxesMove(int index, GamepadAxis axisX, GamepadAxis axisY, float deadzone);

        static const char *getClipboard();
        static void setClipboard(const char *text);

        static PlatformCallbacks events;
    };
}



KAZE_NAMESPACE_END

#endif // kaze_platform_platformbackend_h_
