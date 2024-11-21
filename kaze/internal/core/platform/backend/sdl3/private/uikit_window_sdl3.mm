#import "uikit_window_sdl3.h"
#import "uikit_InputHandler.h"
#import <kaze/core/debug.h>

#import <UIKit/UIKit.h>
#import <Metal/Metal.h>
#import <SDL3/SDL_metal.h>


KAZE_NS_BEGIN

/// Ensures a parameter is not null, passes a NullArgErr and returns false if so
/// Used in most backend functions, since they all return success and fail state via boolean.
#define RETURN_IF_NULL(obj) do { if ( !(obj) ) { \
    KAZE_PUSH_ERR(Error::NullArgErr, "required argument `{}` was null", #obj); \
    return false; \
} } while(0)

static const auto KAZE_PROP_UIWINDOW_METAL_LAYER = "kaze.window.uikit.metalLayer";
static const auto KAZE_PROP_UIWINDOW_INPUTHANDLER = "kaze.window.uikit.inputHandler";

auto backend::uiKitWindowInitMetal(SDL_Window *window) -> bool
{
    RETURN_IF_NULL(window);

    const auto props = SDL_GetWindowProperties(window);
    if ( !props )
    {
        KAZE_PUSH_ERR(Error::BE_RuntimeErr, "failed to get SDL_Window props: {}",
                      SDL_GetError());
        return false;
    }

    const auto uiWindow = (UIWindow *)SDL_GetPointerProperty(props,
                                                             SDL_PROP_WINDOW_UIKIT_WINDOW_POINTER,
                                                             nullptr);
    if ( !uiWindow )
    {
        KAZE_PUSH_ERR(Error::BE_RuntimeErr,
                      "failed to get UIWindow");
        return false;
    }

    // Create custom input manager for uikit
//    KZInputHandler *inputHandler = [[KZInputHandler alloc] init];
//    if ( !inputHandler )
//    {
//        KAZE_PUSH_ERR(Error::BE_RuntimeErr,
//                      "failed to create input handler for UIKit");
//        return false;
//    }

    // Setup the metal view
    UIView* uiView = uiWindow.rootViewController.view;
    CAMetalLayer* metalLayer = [CAMetalLayer layer];
    metalLayer.device = MTLCreateSystemDefaultDevice();
    metalLayer.pixelFormat = MTLPixelFormatBGRA8Unorm;
    metalLayer.framebufferOnly = YES;
    metalLayer.frame = uiView.bounds;

    [uiView.layer addSublayer: metalLayer];

    // Done, commit the items
    if ( !SDL_SetPointerProperty(props, KAZE_PROP_UIWINDOW_METAL_LAYER, metalLayer) )
    {
        KAZE_PUSH_ERR(Error::BE_RuntimeErr,
                      "failed toset uiwindow metal layer into properties");
        [metalLayer removeFromSuperlayer];
        return false;
    }

//    if ( !SDL_SetPointerProperty(props, KAZE_PROP_UIWINDOW_INPUTHANDLER,
//                                 inputHandler) )
//    {
//        KAZE_PUSH_ERR(Error::BE_RuntimeErr,
//                      "failed toset uiwindow input handler into properties");
//        [metalLayer removeFromSuperlayer];
//        return false;
//    }

    return true;

}

auto backend::uiKitWindowSetTextInput(SDL_Window *window, bool textInput) -> bool
{
    RETURN_IF_NULL(window);

//    const auto props = SDL_GetWindowProperties(window);
//    if ( !props )
//    {
//        KAZE_PUSH_ERR(Error::BE_RuntimeErr,
//                      "failed to get SDL_Window props: {}",
//                      SDL_GetError());
//        return false;
//    }
//
//    const auto inputHandler = (KZInputHandler *)SDL_GetPointerProperty(props, KAZE_PROP_UIWINDOW_INPUTHANDLER, nullptr);
//    if ( !inputHandler )
//    {
//        KAZE_PUSH_ERR(Error::BE_RuntimeErr,
//                      "failed to get KZInputHandler from uiwindow");
//        return false;
//    }
//
//    if (textInput)
//        [inputHandler startListening];
//    else
//        [inputHandler stopListening];

    return true;
}

auto backend::uiKitWindowGetMetalLayer(SDL_Window *window, void **outMetalLayer) -> bool
{
    RETURN_IF_NULL(window);
    RETURN_IF_NULL(outMetalLayer);

    const auto props = SDL_GetWindowProperties(window);
    if ( !props )
    {
        KAZE_PUSH_ERR(Error::BE_RuntimeErr, "failed to get window props: {}", SDL_GetError());
        return false;
    }

    const auto layer = SDL_GetPointerProperty(props, KAZE_PROP_UIWINDOW_METAL_LAYER, nullptr);
    if ( !layer )
    {
        KAZE_PUSH_ERR(Error::BE_LogicError, "backend-owned metal layer does not exist on window");
        return false;
    }

    *outMetalLayer = layer;
    return true;
}

KAZE_NS_END
