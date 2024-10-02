#include "PlatformBackend_glfw3.h"

#import <AppKit/AppKit.h>

#include <GLFW/glfw3.h>

#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3native.h>

#include <kaze/kaze.h>
#include <kaze/debug.h>

KAZE_NAMESPACE_BEGIN

auto getWindowCocoaFullScreen(GLFWwindow *window, bool *outFullscreen) -> bool
{
    if (outFullscreen == nullptr)
    {
        KAZE_CORE_ERRCODE(Error::NullArgErr, "Required argument outFullscreen was null");
        return false;
    }

    if (window == nullptr)
    {
        KAZE_CORE_ERRCODE(Error::NullArgErr, "Required argument window was null");
        return false;
    }

    NSWindow *nswindow = static_cast<NSWindow *>(glfwGetCocoaWindow(window));
    if ( !nswindow )
    {
        const char *errMessage;
        glfwGetError(&errMessage);
        KAZE_CORE_ERRCODE(Error::BE_RuntimeError, "Failed to get Cocoa window: {}", errMessage);
        return false;
    }

     *outFullscreen = static_cast<bool>([nswindow styleMask] & NSWindowStyleMaskFullScreen);
     return true;
}

auto setWindowCocoaFullScreen(GLFWwindow *window, bool value) -> bool
{
    bool curFullscreen;
    if ( !getWindowCocoaFullScreen(window, &curFullscreen) || curFullscreen == value)
      return false;

    NSWindow *nswindow = static_cast<NSWindow *>(glfwGetCocoaWindow(window));
    if ( !nswindow )
    {
        const char *errMessage;
        glfwGetError(&errMessage);
        KAZE_CORE_ERRCODE(Error::BE_RuntimeError, "Failed to get Cocoa window: {}", errMessage);

        return false;
    }

    [nswindow toggleFullScreen:nil];
    return true;
}

KAZE_NAMESPACE_END
