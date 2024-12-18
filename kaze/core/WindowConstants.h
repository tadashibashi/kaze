#pragma once

#include <kaze/core/lib.h>

KAZE_NS_BEGIN

using WindowHandle = void *;
using ConstWindowHandle = const void *;

struct WindowInit {
    enum Flags : Uint
    {
        None =        0,
        Resizable =   1u << 0,  ///< user can drag corners to adjust the window size
        Borderless =  1u << 1,  ///< has no border, title bar, or window control buttons
        Fullscreen =  1u << 2,  ///< starts off native fullscreen
        Floating =    1u << 3,  ///< exhibits always on-top behavior
        Transparent = 1u << 4,  ///< framebuffer is see-through
        Hidden =      1u << 5,  ///< starts off hidden
        Maximized =   1u << 6,  ///< starts off maximized
    };
};

enum struct FullscreenMode
{
    Native,  ///< uses the platform's native windowing API for fullscreen
    Desktop, ///< on fullscreen, window becomes borderless, positioned at the top-left, with full screen size
    Unknown,
};

constexpr WindowInit::Flags operator |(const WindowInit::Flags a, const WindowInit::Flags b)
{
    return static_cast<WindowInit::Flags>(static_cast<Uint>(a) | static_cast<Uint>(b));
}

constexpr WindowInit::Flags &operator |=(WindowInit::Flags &a, const WindowInit::Flags b)
{
    a = static_cast<WindowInit::Flags>(static_cast<Uint>(a) | static_cast<Uint>(b));
    return a;
}

KAZE_NS_END
