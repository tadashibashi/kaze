/// \file Window_glfw3.h
/// Contains GLFW3 window-handling functionality
#pragma once
#include <kaze/core/lib.h>
#include <kaze/core/WindowConstants.h>
#include <kaze/core/input/CursorConstants.h>
#include <kaze/core/math/Rect.h>
#include <kaze/core/platform/backend/backend.h>
#include <kaze/core/platform/defines.h>

#include <GLFW/glfw3.h>

KAZE_NS_BEGIN

namespace backend {

#if KAZE_PLATFORM_MACOS // Platform-specific fullscreen workaround for Mac (bugged out in glfw3)
    auto setWindowCocoaFullscreen(GLFWwindow *window, bool fullscreen) noexcept -> bool;
    auto getWindowCocoaFullscreen(GLFWwindow *window, bool *outFullscreen) noexcept -> bool;
#endif
    /// Data associated with a glfw window
    struct WindowData {
        void *userdata{};
        FullscreenMode fullscreenMode{FullscreenMode::Native};

        /// Tracks last window state before going into desktop fullscreen mode
        struct LastWindowState {
            bool decorated{};        ///< whether window was bordered
            Recti rect{};            ///< window position and size
            Vec2i minSize{-1, -1};   ///< min window size limit
            Vec2i maxSize{-1, -1};   ///< max window size limit
        } last;

        bool isDesktopFullscreen{};  ///< whether the window is in fullscreen with desktop mode on
        CursorHandle cursor{};
        Vec2d lastCursorPos{};
        Vec2d relCursorPos{};
        bool isCapture{};
    };

    /// Get data from a glfw window
    /// \param[in]  window    window to get data from
    /// \param[out] outData   retreives the data pointer
    /// \returns whether retrieval was successful;
    ///          on true, `outData` is non-null and should contain associated data pointer
    auto getWindowData(const WindowHandle window, WindowData **outData) noexcept -> bool;

} // namespace backend

KAZE_NS_END
