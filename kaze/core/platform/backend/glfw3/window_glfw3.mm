/// \file Window_glfw3.mm
/// Native MacOS window-related functions for the GLFW3 backend
#include "window_glfw3.h"
#include "common_glfw3.h"

#include <kaze/core/lib.h>
#include <kaze/core/debug.h>
#include <kaze/core/errors.h>

#import <AppKit/AppKit.h>

#include <GLFW/glfw3.h>

#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3native.h>


KAZE_NS_BEGIN

namespace backend {
    auto getWindowCocoaFullscreen(GLFWwindow *window, bool *outFullscreen) noexcept -> bool
    {
        if (outFullscreen == nullptr)
        {
            KAZE_PUSH_ERR(Error::NullArgErr, "Required argument outFullscreen was null");
            return false;
        }

        if (window == nullptr)
        {
            KAZE_PUSH_ERR(Error::NullArgErr, "Required argument window was null");
            return false;
        }

        NSWindow *nswindow = static_cast<NSWindow *>(glfwGetCocoaWindow(window));
        if ( !nswindow )
        {
            const char *errMessage;
            glfwGetError(&errMessage);
            KAZE_PUSH_ERR(Error::BE_RuntimeErr, "Failed to get Cocoa window: {}", errMessage);
            return false;
        }

         *outFullscreen = static_cast<bool>([nswindow styleMask] & NSWindowStyleMaskFullScreen);
         return true;
    }

    auto setWindowCocoaFullscreen(GLFWwindow *window, bool value) noexcept -> bool
    {
        bool curFullscreen;
        if ( !getWindowCocoaFullscreen(window, &curFullscreen) || curFullscreen == value)
          return false;

        NSWindow *nswindow = static_cast<NSWindow *>(glfwGetCocoaWindow(window));
        if ( !nswindow )
        {
            const char *errMessage;
            glfwGetError(&errMessage);
            KAZE_PUSH_ERR(Error::BE_RuntimeErr, "Failed to get Cocoa window: {}", errMessage);

            return false;
        }

        [nswindow toggleFullScreen:nil];
        return true;
    }
}
KAZE_NS_END
