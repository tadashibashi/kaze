#pragma once
#ifndef kaze_platform_backends_glfw3_h_
#define kaze_platform_backends_glfw3_h_

#include <kaze/kaze.h>
#include <kaze/math/Rect.h>
#include <kaze/platform/Window.h>

#include <GLFW/glfw3.h>

#define WIN_CAST(window) static_cast<GLFWwindow*>(window)
#define WINDATA_CAST(window) static_cast<WindowData *>(glfwGetWindowUserPointer(WIN_CAST(window)))

KAZE_NAMESPACE_BEGIN

#if KAZE_TARGET_MACOS
auto setWindowCocoaFullScreen(GLFWwindow *window, bool fullscreen) -> bool;
auto getWindowCocoaFullScreen(GLFWwindow *window, bool *outFullscreen) -> bool;
#endif

struct WindowData {
    void *userdata{};
    FullscreenMode fullscreenMode{FullscreenMode::Native};
    struct LastWindowState {
        bool decorated{};
        Recti rect{};
        Vec2i minSize{-1, -1};
        Vec2i maxSize{-1, -1};
    } last;

    bool isDesktopFullscreen{};
};


KAZE_NAMESPACE_END

#endif
