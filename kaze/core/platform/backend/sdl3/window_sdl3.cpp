/// \file Window_sdl3.h
/// SDL3 implementation for functions in the backend::window namespace
#include "window_sdl3.h"
#include "common_sdl3.h"

#include <kaze/core/errors.h>
#include <kaze/core/debug.h>

#include <SDL3/SDL.h>

#ifdef SDL_PLATFORM_IOS
#include <SDL3/SDL_metal.h>
#endif

KAZE_NS_BEGIN

namespace backend {

    auto getWindowData(const WindowHandle window,
                                         WindowData **outData) -> bool
    {
        KAZE_ASSERT(window);
        KAZE_ASSERT(outData);

        const auto props = SDL_GetWindowProperties(WIN_CAST(window));
        if (!props) {
        KAZE_PUSH_ERR(Error::BE_RuntimeErr,
                          "Failed to get window properties: {}", SDL_GetError());
        return false;
        }

        const auto data = static_cast<WindowData *>(
            SDL_GetPointerProperty(props, "WindowData", nullptr));
        if (!data) {
            KAZE_PUSH_ERR(Error::BE_LogicError, "Missing WindowData on SDL_Window");
            return false;
        }

        *outData = data;
        return true;
    }

    auto window::getNativeInfo(WindowHandle window) noexcept -> NativePlatformData
    {
        NativePlatformData result{};
        auto props = SDL_GetWindowProperties( WIN_CAST(window) );
        if (props == 0)
        {
            KAZE_PUSH_ERR(Error::BE_RuntimeErr, "Failed to retrieve SDL_Window properties: {}", SDL_GetError());
            return {};
        }

    #if defined(SDL_PLATFORM_MACOS)

        result.windowHandle = SDL_GetPointerProperty(props, SDL_PROP_WINDOW_COCOA_WINDOW_POINTER, nullptr);
        result.displayType = nullptr;

    #elif defined(SDL_PLATFORM_IOS)

        const auto metalView = SDL_Metal_CreateView(WIN_CAST(window));
        if (!metalView)
        {
            KAZE_CORE_ERR("Failed to create metal view": {}, SDL_GetError());
            return {};
        }

        auto metalLayer = SDL_Metal_GetLayer(metalView);
        if (!metalLayer)
        {
            KAZE_CORE_ERR("Failed to get Metal layer from SDL_MetalView: {}", SDL_GetError());
            return {};
        }
        result.windowHandle = metalLayer;
        result.displayType = nullptr;

    #elif defined(SDL_PLATFORM_WIN32)

        result.windowHandle = SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr);
        result.displayType = SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WIN32_HDC_POINTER, nullptr);

    #elif defined(SDL_PLATFORM_LINUX)

        if (SDL_strcmp(SDL_GetCurrentVideoDriver(), "x11") == 0)
        {   // X11
            result.windowHandle = (void *)(uintptr_t)SDL_GetNumberProperty(props, SDL_PROP_WINDOW_X11_WINDOW_NUMBER, 0);
            result.displayType = SDL_GetPointerProperty(props, SDL_PROP_WINDOW_X11_DISPLAY_POINTER, nullptr);
        }
        else if (SDL_strcmp(SDL_GetCurrentVideoDriver(), "wayland") == 0)
        {   // Wayland
            result.windowHandle = SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WAYLAND_SURFACE_POINTER, nullptr);
            result.displayType = SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WAYLAND_DISPLAY_POINTER, nullptr);
        }

    #elif defined(SDL_PLATFORM_ANDROID)
        result.windowHandle = SDL_GetPointerProperty(props, SDL_PROP_WINDOW_ANDROID_WINDOW_POINTER, nullptr);
        result.displayType = result.windowHandle;

    #elif defined(SDL_PLATFORM_EMSCRIPTEN)
        result.windowHandle = (void *)"#canvas";
        result.displayType = nullptr;

    #else
        result.windowHandle = nullptr;
        result.displayType = nullptr;

    #endif
        return result;
    }

    auto window::open(const char *title, const int width, const int height, const WindowInit::Flags flags,
        WindowHandle *outWindow) noexcept -> bool
    {
        auto sdl3Flags = SDL_WINDOW_HIDDEN | SDL_WINDOW_HIGH_PIXEL_DENSITY;
    #if KAZE_PLATFORM_APPLE
        sdl3Flags |= SDL_WINDOW_METAL;
    #endif
        if (flags & WindowInit::Resizable)
            sdl3Flags |= SDL_WINDOW_RESIZABLE;
        if (flags & WindowInit::Borderless)
            sdl3Flags |= SDL_WINDOW_BORDERLESS;
        if (flags & WindowInit::Fullscreen)
            sdl3Flags |= SDL_WINDOW_FULLSCREEN;
        if (flags & WindowInit::Floating)
            sdl3Flags |= SDL_WINDOW_ALWAYS_ON_TOP;
        if (flags & WindowInit::Maximized)
            sdl3Flags |= SDL_WINDOW_MAXIMIZED;
        if (flags & WindowInit::Transparent)
            sdl3Flags |= SDL_WINDOW_TRANSPARENT;

        const auto window = SDL_CreateWindow(title, width, height, sdl3Flags);
        if ( !window )
        {
            KAZE_PUSH_ERR(Error::BE_RuntimeErr, "Failed to create SDL3 Window: {}", SDL_GetError());
            return KAZE_FALSE;
        }
        window::setSize(window, width, height);

        const auto props = SDL_GetWindowProperties(window);
        if ( !props )
        {
            KAZE_PUSH_ERR(Error::BE_RuntimeErr, "Failed to get window properties: {}", SDL_GetError());
            SDL_DestroyWindow(window);
            return KAZE_FALSE;
        }

        // Set up associated window metadata
        const auto windowData = new WindowData();
        if ( !SDL_SetPointerPropertyWithCleanup(props, "WindowData", windowData,
            [](void *userptr, void *value) noexcept {
                delete static_cast<WindowData *>(value);
            }, nullptr) )
        {
            KAZE_CORE_ERR("Failed to set WindowData to SDL_Window properties: {}", SDL_GetError());
            SDL_DestroyWindow(window);
            delete windowData; // data ptr failed to hand over to prop API, clean up manually
            return KAZE_FALSE;
        }

        // Check if mouse is inside the window to set initial hover state
        {
            float mouseX = 0, mouseY = 0;
            SDL_GetGlobalMouseState(&mouseX, &mouseY);

            int winX = 0, winY = 0;
            if ( !SDL_GetWindowPosition(window, &winX, &winY) )
            {
                KAZE_CORE_WARN("Failed to get window position: {}", SDL_GetError());
            }

            int winW = 0, winH = 0;
            if ( !SDL_GetWindowSize(window, &winW, &winH) )
            {
                KAZE_CORE_WARN("Failed to get window size: {}", SDL_GetError());
            }

            if (mouseX >= winX && mouseX < winX + winW &&
                mouseY >= winY && mouseY < winY + winH)
            {
                windowData->isHovered = true;
            }
        }

        if ( !(flags & WindowInit::Hidden) ) // keep window hidden if hidden flag was set
        {
            if (!SDL_ShowWindow(window)) // show it otherwise
            {
                KAZE_CORE_WARN("SDL_ShowWindow failed: {}", SDL_GetError());
            }
        }

        *outWindow = window;
        return KAZE_TRUE;
    }

    auto window::close(const WindowHandle window) noexcept -> bool
    {
        RETURN_IF_NULL(window);

        int windowCount;
        auto windows = SDL_GetWindows(&windowCount);
        if ( !windows )
        {

            return false;
        }

        SDL_DestroyWindow(static_cast<SDL_Window *>(window));
        return true;
    }

    auto window::isOpen(const WindowHandle window, bool *outOpen) noexcept -> bool
    {
        RETURN_IF_NULL(window);
        RETURN_IF_NULL(outOpen);

        int windowCount;
        auto windows = SDL_GetWindows(&windowCount);
        if ( !windows )
        {
            KAZE_PUSH_ERR(Error::BE_RuntimeErr, "Failed to get windows: {}", SDL_GetError());
            return false;
        }

        for (int i = 0; i < windowCount; ++i)
        {
            if (windows[i] == window)
            {
                *outOpen = true;
                return true;
            }
        }

        *outOpen = false;
        return true;
    }

    auto window::setUserData(const WindowHandle window, void *data) noexcept -> bool
    {
        RETURN_IF_NULL(window);

        const auto props = SDL_GetWindowProperties( WIN_CAST(window) );
        if ( !props )
        {
            KAZE_PUSH_ERR(Error::BE_RuntimeErr, "Failed to get window properties: {}", SDL_GetError());
            return false;
        }

        if ( !SDL_SetPointerProperty(props, "userptr", data) )
        {
            KAZE_PUSH_ERR(Error::BE_RuntimeErr, "Failed to set userptr pointer property: {}", SDL_GetError());
            return false;
        }

        return true;
    }

    auto window::getUserData(const WindowHandle window, void **outData) noexcept -> bool
    {
        RETURN_IF_NULL(window);
        RETURN_IF_NULL(outData);

        const auto props = SDL_GetWindowProperties( WIN_CAST(window) );
        if ( !props )
        {
            KAZE_PUSH_ERR(Error::BE_RuntimeErr, "Failed to get window properties: {}", SDL_GetError());
            return false;
        }

        *outData = SDL_GetPointerProperty(props, "userptr", nullptr);
        return true;
    }

    auto window::setTitle(const WindowHandle window, const char *title) noexcept -> bool
    {
        RETURN_IF_NULL(window);
        RETURN_IF_NULL(title);

        if ( !SDL_SetWindowTitle( WIN_CAST(window), title ) )
        {
            KAZE_PUSH_ERR(Error::BE_RuntimeErr, "Failed to set window title: {}", SDL_GetError());
            return false;
        }

        return true;
    }

    auto window::getTitle(const WindowHandle window, const char **outTitle) noexcept -> bool
    {
        RETURN_IF_NULL(window);
        RETURN_IF_NULL(outTitle);

        const auto title = SDL_GetWindowTitle(WIN_CAST(window));
        if ( !title )
        {
            KAZE_PUSH_ERR(Error::BE_RuntimeErr, "Failed to get window title: {}", SDL_GetError());
            return false;
        }

        *outTitle = title;
        return true;
    }

    auto window::setSize(const WindowHandle window, const int x, const int y) noexcept -> bool
    {
        RETURN_IF_NULL(window);

        const auto scale = SDL_GetDisplayContentScale(SDL_GetDisplayForWindow(WIN_CAST(window)));

        if ( !SDL_SetWindowSize( WIN_CAST(window), x * scale, y * scale) )
        {
            KAZE_PUSH_ERR(Error::BE_RuntimeErr, "failed to set logical window size: {}", SDL_GetError());
            return false;
        }

        return true;
    }

    auto window::getSize(const WindowHandle window, int *x, int *y) noexcept -> bool
    {
        RETURN_IF_NULL(window);

        if ( !SDL_GetWindowSize(WIN_CAST(window), x, y) )
        {
            KAZE_PUSH_ERR(Error::BE_RuntimeErr, "Failed to get window size: {}", SDL_GetError());
            return false;
        }

        const auto scale = SDL_GetDisplayContentScale(SDL_GetDisplayForWindow(WIN_CAST(window)));
        if (scale != 0)
        {
            *x /= scale;
            *y /= scale;
        }

        return true;
    }

    auto window::getFramebufferSize(const WindowHandle window, int *x, int *y) noexcept -> bool
    {
        RETURN_IF_NULL(window);

        if ( !SDL_GetWindowSizeInPixels(WIN_CAST(window), x, y) )
        {
            KAZE_PUSH_ERR(Error::BE_RuntimeErr, "Failed to get window display size: {}", SDL_GetError());
            return false;
        }

        return true;
    }

    auto window::isFullscreen(const WindowHandle window, bool *outFullscreen) noexcept -> bool
    {
        RETURN_IF_NULL(window);
        RETURN_IF_NULL(outFullscreen);

        *outFullscreen = static_cast<bool>(SDL_GetWindowFlags(WIN_CAST(window)) & SDL_WINDOW_FULLSCREEN);
        return true;
    }

    auto window::setFullscreen(WindowHandle window, bool value) noexcept -> bool
    {
        RETURN_IF_NULL(window);

        if ( !SDL_SetWindowFullscreen(WIN_CAST(window), value) )
        {
            KAZE_PUSH_ERR(Error::BE_RuntimeErr, "Failed to set fullscreen to {}", value);
            return false;
        }

        return true;
    }

    auto window::getFullscreenMode(const WindowHandle window, FullscreenMode *mode) noexcept -> bool
    {
        RETURN_IF_NULL(window);
        RETURN_IF_NULL(mode);

        *mode = SDL_GetWindowFullscreenMode(WIN_CAST(window)) == nullptr ?
            FullscreenMode::Desktop : FullscreenMode::Native;
        return true;
    }

    static auto setNativeFullscreenMode(const WindowHandle window) noexcept -> bool
    {
        KAZE_ASSERT(window);

        const auto display = SDL_GetPrimaryDisplay();
        if (!display)
        {
            KAZE_CORE_ERR("Failed to get primary display: {}", SDL_GetError());
            return false;
        }

        SDL_Rect displayBounds;
        if (!SDL_GetDisplayBounds(display, &displayBounds))
        {
            KAZE_CORE_ERR("Failed to get display bounds: {}", SDL_GetError());
            return false;
        }

        SDL_DisplayMode displayMode;
        if (!SDL_GetClosestFullscreenDisplayMode(display, displayBounds.w, displayBounds.y, 0, true, &displayMode))
        {
            KAZE_CORE_ERR("Failed to get fullscreen display mode: {}", SDL_GetError());
            return false;
        }

        if (!SDL_SetWindowFullscreenMode(WIN_CAST(window), &displayMode))
        {
            KAZE_CORE_ERR("Failed to set fullscreen mode: {}", SDL_GetError());
            return false;
        }

        return true;
    }

    static auto setDesktopFullscreenMode(const WindowHandle window) noexcept -> bool
    {
        KAZE_ASSERT(window);

        if ( !SDL_SetWindowFullscreenMode(WIN_CAST(window), nullptr) )
        {
            KAZE_CORE_ERR("Failed to set window fullscreen mode to desktop: {}", SDL_GetError());
            return false;
        }

        return true;
    }

    auto window::setFullscreenMode(WindowHandle window, FullscreenMode mode) noexcept -> bool
    {
        if (mode == FullscreenMode::Desktop)
        {
            if ( !setDesktopFullscreenMode(window) )
                return false;
        }
        else if (mode == FullscreenMode::Native)
        {
            if ( !setNativeFullscreenMode(window) )
                return false;
        }
        else
        {
            KAZE_CORE_ERR("Unknown FullscreenMode type");
            return false;
        }

        return true;
    }

    bool windowIsDesktopFullscreen(const WindowHandle window) noexcept
    {
        return static_cast<bool>(SDL_GetWindowFlags(WIN_CAST(window)) & SDL_WINDOW_FULLSCREEN) &&
            SDL_GetWindowFullscreenMode(WIN_CAST(window)) == nullptr;
    }

    static bool setDesktopFullscreen(const WindowHandle window, const bool value)
    {
        const auto lastMode = SDL_GetWindowFullscreenMode(WIN_CAST(window));
        if ( !SDL_SetWindowFullscreenMode(WIN_CAST(window), nullptr) )
        {
            KAZE_CORE_ERR("Failed to set SDL_Window's desktop fullscreen mode: {}", SDL_GetError());
            return false;
        }

        if (!SDL_SetWindowFullscreen( WIN_CAST(window), value) )
        {
            KAZE_CORE_ERR("Failed to set SDL_Window fullscreen: {}", SDL_GetError());
            SDL_SetWindowFullscreenMode(WIN_CAST(window), lastMode); // revert to last mode
            return false;
        }

        return true;
    }

    auto window::isBordered(const WindowHandle window, bool *outBordered) noexcept -> bool
    {
        RETURN_IF_NULL(window);
        RETURN_IF_NULL(outBordered);

        *outBordered = !static_cast<bool>( SDL_GetWindowFlags(WIN_CAST(window)) & SDL_WINDOW_BORDERLESS );
        return true;
    }

    auto window::setBordered(const WindowHandle window, const bool value) noexcept -> bool
    {
        if ( !SDL_SetWindowBordered(WIN_CAST(window), value) )
        {
            KAZE_CORE_ERR("Failed to set window borderless: {}", SDL_GetError());
            return false;
        }

        return true;
    }

    auto window::isMinimized(const WindowHandle window, bool *outMinimized) noexcept -> bool
    {
        RETURN_IF_NULL(window);
        RETURN_IF_NULL(outMinimized);

        *outMinimized = static_cast<bool>(
            SDL_GetWindowFlags(WIN_CAST(window)) & SDL_WINDOW_MINIMIZED);
        return true;
    }

    auto window::minimize(const WindowHandle window) noexcept -> bool
    {
        RETURN_IF_NULL(window);

        if ( !SDL_MinimizeWindow( WIN_CAST(window) ) )
        {
            KAZE_CORE_ERR("Failed to minimize window: {}", SDL_GetError());
            return false;
        }

        return true;
    }

    auto window::isMaximized(const WindowHandle window, bool *outMaximized) noexcept -> bool
    {
        RETURN_IF_NULL(window);
        RETURN_IF_NULL(outMaximized);

        *outMaximized = static_cast<bool>(
            SDL_GetWindowFlags(WIN_CAST(window)) & SDL_WINDOW_MAXIMIZED);
        return true;
    }

    auto window::maximize(const WindowHandle window) noexcept -> bool
    {
        RETURN_IF_NULL(window);

        if ( !SDL_MaximizeWindow( WIN_CAST(window) ) )
        {
            KAZE_CORE_ERR("Failed to maximize window: {}", SDL_GetError());
            return false;
        }

        return true;
    }

    auto window::restore(const WindowHandle window) noexcept -> bool
    {
        RETURN_IF_NULL(window);

        if ( !SDL_RestoreWindow( WIN_CAST(window) ) )
        {
            KAZE_CORE_ERR("Failed to restore window: {}", SDL_GetError());
            return false;
        }

        return true;
    }

    auto window::getPosition(const WindowHandle window, int *x, int *y) noexcept -> bool
    {
        RETURN_IF_NULL(window);

        if ( !SDL_GetWindowPosition( WIN_CAST(window), x, y ) )
        {
            KAZE_CORE_ERR("Failed to get window position: {}", SDL_GetError());
            return false;
        }

        return true;
    }

    auto window::setPosition(const WindowHandle window, const int x, const int y) noexcept -> bool
    {
        RETURN_IF_NULL(window);

        if ( !SDL_SetWindowPosition( WIN_CAST(window), x, y ) )
        {
            KAZE_CORE_ERR("Failed to set window position to {{{}, {}}}: {}", x, y, SDL_GetError());
            return false;
        }

        return true;
    }

    auto window::getResizable(const WindowHandle window, bool *outResizable) noexcept -> bool
    {
        RETURN_IF_NULL(window);
        RETURN_IF_NULL(outResizable);

        *outResizable = static_cast<bool>(
            SDL_GetWindowFlags(WIN_CAST(window)) & SDL_WINDOW_RESIZABLE);
        return true;
    }

    auto window::setResizable(const WindowHandle window, const bool value) noexcept -> bool
    {
        if ( !SDL_SetWindowResizable(WIN_CAST(window), value) )
        {
            KAZE_CORE_ERR("Failed to set window resizable attribute to {}: {}",
                value, SDL_GetError());
            return false;
        }

        return true;
    }

    auto window::isHidden(const WindowHandle window, bool *outHidden) noexcept -> bool
    {
        RETURN_IF_NULL(window);
        RETURN_IF_NULL(outHidden);

        *outHidden = static_cast<bool>(
            SDL_GetWindowFlags(WIN_CAST(window)) & SDL_WINDOW_HIDDEN);
        return true;
    }

    auto window::setHidden(const WindowHandle window, const bool value) noexcept -> bool
    {
        RETURN_IF_NULL(window);

        bool result;
        if (value)
        {
            result = SDL_HideWindow( WIN_CAST(window) );
        }
        else
        {
            result = SDL_ShowWindow( WIN_CAST(window) );
        }

        if ( !result )
        {
            KAZE_PUSH_ERR(Error::BE_RuntimeErr, "Failed to set window to {}: {}",
                value ? "hidden" : "shown", SDL_GetError());
            return false;
        }

        return true;
    }

    auto window::isHovered(const WindowHandle window, bool *outHovered) noexcept -> bool
    {
        RETURN_IF_NULL(window);
        RETURN_IF_NULL(outHovered);

        WindowData *data;
        if ( !getWindowData(window, &data) )
            return false;

        *outHovered = data->isHovered;
        return true;
    }

    auto window::isFloating(const WindowHandle window, bool *outFloating) noexcept -> bool
    {
        RETURN_IF_NULL(window);
        RETURN_IF_NULL(outFloating);

        *outFloating = static_cast<bool>(
            SDL_GetWindowFlags( WIN_CAST(window)) & SDL_WINDOW_ALWAYS_ON_TOP );
        return true;
    }

    auto window::setFloating(const WindowHandle window, const bool value) noexcept -> bool
    {
        if ( !SDL_SetWindowAlwaysOnTop( WIN_CAST(window), value ) )
        {
            KAZE_CORE_ERR("Failed to set window floating attribute to {}: {}", value, SDL_GetError());
            return false;
        }

        return true;
    }

    auto window::isTransparent(const WindowHandle window, bool *outTransparent) noexcept -> bool
    {
        RETURN_IF_NULL(window);
        RETURN_IF_NULL(outTransparent);

        *outTransparent = static_cast<bool>(
            SDL_GetWindowFlags(WIN_CAST(window)) & SDL_WINDOW_TRANSPARENT);
        return true;
    }

    auto window::setTransparent(const WindowHandle window, const bool value) noexcept -> bool
    {
        RETURN_IF_NULL(window);

        if ( !SDL_SetWindowOpacity( WIN_CAST(window), (value ? 1.0f : 0) ) )
        {
            KAZE_CORE_ERR("Failed to set window transparency to {}: {}", value, SDL_GetError());
            return false;
        }

        return true;
    }

    auto window::isFocused(const WindowHandle window, bool *outFocused) noexcept -> bool
    {
        RETURN_IF_NULL(window);
        RETURN_IF_NULL(outFocused);

        *outFocused = static_cast<bool>(
            SDL_GetWindowFlags(WIN_CAST(window)) & SDL_WINDOW_INPUT_FOCUS);
        return true;
    }

    auto window::focus(const WindowHandle window) noexcept -> bool
    {
        bool curHidden;
        if ( !window::isHidden(window, &curHidden) )
            return false;

        if ( !curHidden )
        {
            if ( !window::setHidden(window, false) )
                return false;
        }

        if ( !SDL_RaiseWindow( WIN_CAST(window) ) )
        {
            KAZE_CORE_WARN("Failed to raise window: {}", SDL_GetError());
            return false;
        }

        if ( !SDL_SetWindowKeyboardGrab(WIN_CAST(window), true) )
        {
            KAZE_CORE_WARN("Failed to set window keyboard grab: {}", SDL_GetError());
            return false;
        }

        return true;
    }

    auto window::setMinSize(const WindowHandle window, const int minWidth, const int minHeight) noexcept -> bool
    {
        RETURN_IF_NULL(window);

        if ( !SDL_SetWindowMinimumSize( WIN_CAST(window), minWidth, minHeight ) )
        {
            KAZE_CORE_ERR("Failed to set window minimum size to {{{}, {}}}: {}",
                minWidth, minHeight, SDL_GetError());
            return false;
        }

        return true;
    }

    auto window::setMaxSize(const WindowHandle window, const int maxWidth, const int maxHeight) noexcept -> bool
    {
        RETURN_IF_NULL(window);

        if ( !SDL_SetWindowMaximumSize(WIN_CAST(window), maxWidth, maxHeight) )
        {
            KAZE_CORE_ERR("Failed to set window maximum size to {{{}, {}}}: {}",
                maxWidth, maxHeight, SDL_GetError());
            return false;
        }

        return true;
    }

    auto window::getMinSize(const WindowHandle window, int *minWidth, int *minHeight) noexcept -> bool
    {
        RETURN_IF_NULL(window);

        if ( !SDL_GetWindowMinimumSize( WIN_CAST(window), minWidth, minHeight ) )
        {
            KAZE_CORE_ERR("Failed to get window minimum size: {}", SDL_GetError());
            return false;
        }

        return true;
    }

    auto window::getMaxSize(const WindowHandle window, int *maxWidth, int *maxHeight) noexcept -> bool
    {
        if ( !SDL_GetWindowMaximumSize( WIN_CAST(window), maxWidth, maxHeight) )
        {
            KAZE_PUSH_ERR(Error::BE_RuntimeErr, "Failed to get window maximum size: {}", SDL_GetError());
            return false;
        }

        return true;
    }

    auto window::setCursorMode(const WindowHandle window, const CursorMode mode) noexcept -> bool
    {
        RETURN_IF_NULL(window);

        WindowData *data;
        if ( !getWindowData(window, &data) )
            return false;

        switch(mode)
        {
        case CursorMode::Visible:
            data->cursorVisibleMode = true;
            return SDL_SetWindowRelativeMouseMode(WIN_CAST(window), false);
        case CursorMode::Hidden:
            data->cursorVisibleMode = false;
            return SDL_SetWindowRelativeMouseMode(WIN_CAST(window), false);
        case CursorMode::Relative:
            data->cursorVisibleMode = false;
            return SDL_SetWindowRelativeMouseMode(WIN_CAST(window), true);
        default:
            KAZE_PUSH_ERR(Error::InvalidEnum, "Invalid CursorMode passed to window::setCursorMode");
            return false;
        }
    }

    auto window::getCursorMode(const WindowHandle window, CursorMode *outMode) noexcept -> bool
    {
        RETURN_IF_NULL(window);
        RETURN_IF_NULL(outMode);

        WindowData *data;
        if ( !getWindowData(window, &data) )
            return false;

        if (data->cursorVisibleMode)
        {
            *outMode = CursorMode::Visible;
        }
        else if (SDL_GetWindowRelativeMouseMode(WIN_CAST(window)))
        {
            *outMode = CursorMode::Relative;
        }
        else
        {
            *outMode = CursorMode::Hidden;
        }

        return true;
    }

    auto window::setTextInputMode(WindowHandle window, bool yes) noexcept -> bool
    {
        RETURN_IF_NULL(window);

        return (yes) ?
            SDL_StartTextInput(WIN_CAST(window)) :
            SDL_StopTextInput(WIN_CAST(window));
    }

    auto window::isTextInputActive(WindowHandle window, bool *outValue) noexcept -> bool
    {
        RETURN_IF_NULL(window);
        RETURN_IF_NULL(outValue);
        *outValue = SDL_TextInputActive(WIN_CAST(window));
        return true;
    }

} // namespace backend

KAZE_NS_END

