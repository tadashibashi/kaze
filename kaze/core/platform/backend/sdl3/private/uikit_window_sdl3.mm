#include "uikit_window_sdl3.h"

#include <kaze/core/debug.h>

#import <UIKit/UIKit.h>
#import <Metal/Metal.h>
#import <SDL3/SDL_metal.h>


KAZE_NS_BEGIN

auto backend::initMetalUIKitWindow(SDL_Window *window) -> void *
{
    const auto props = SDL_GetWindowProperties(window);
    if ( !props )
    {
        KAZE_PUSH_ERR(Error::BE_RuntimeErr, "failed to get SDL_Window props");
        return nullptr;
    }

    const auto uiWindow = (UIWindow *)SDL_GetPointerProperty(props,
                                                             SDL_PROP_WINDOW_UIKIT_WINDOW_POINTER,
                                                             nullptr);
    if ( !uiWindow )
    {
        KAZE_PUSH_ERR(Error::BE_RuntimeErr, "failed to get UIWindow");
        return nullptr;
    }

    UIView* uiView = uiWindow.rootViewController.view;
    CAMetalLayer* metalLayer = [CAMetalLayer layer];
    metalLayer.device = MTLCreateSystemDefaultDevice();
    metalLayer.pixelFormat = MTLPixelFormatBGRA8Unorm;
    metalLayer.framebufferOnly = YES;
    metalLayer.frame = uiView.bounds;

    [uiView.layer addSublayer:metalLayer];

    return metalLayer;
}

KAZE_NS_END
