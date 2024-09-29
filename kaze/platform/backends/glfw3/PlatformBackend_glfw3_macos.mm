#include "PlatformBackend_glfw3.h"

#import <AppKit/AppKit.h>

#include <GLFW/glfw3.h>

#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3native.h>

#include <kaze/kaze.h>

KAZE_NAMESPACE_BEGIN

bool getWindowCocoaFullScreen(GLFWwindow *window)
{
    NSWindow *nswindow = static_cast<NSWindow *>(glfwGetCocoaWindow(window));
    return static_cast<bool>([nswindow styleMask] & NSWindowStyleMaskFullScreen);
}

void setWindowCocoaFullScreen(GLFWwindow *window, bool value)
{
    if (getWindowCocoaFullScreen(window) == value) return;

    NSWindow *nswindow = static_cast<NSWindow *>(glfwGetCocoaWindow(window));
    [nswindow toggleFullScreen:nil];
}

KAZE_NAMESPACE_END
