/// @file Window_sdl3.h
/// @description
/// Contains window data types and functions
#pragma once
#ifndef kaze_platform_backends_sdl3_window_sdl3_h_
#define kaze_platform_backends_sdl3_window_sdl3_h_
#include <kaze/kaze.h>
#include <kaze/platform/PlatformBackend.h>

KAZE_NAMESPACE_BEGIN
namespace backend {

    /// Data associated with each window
    struct WindowData
    {
        bool isHovered{false};
        bool cursorVisibleMode{true};
    };

    bool getWindowData(const WindowHandle window, WindowData **outData);
}

KAZE_NAMESPACE_END

#endif
