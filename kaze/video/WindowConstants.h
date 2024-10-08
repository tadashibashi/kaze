#pragma once
#ifndef kaze_video_windowconstants_h_
#define kaze_video_windowconstants_h_

#include <kaze/kaze.h>

KAZE_NAMESPACE_BEGIN

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

KAZE_NAMESPACE_END

#endif // kaze_video_windowconstants_h_
