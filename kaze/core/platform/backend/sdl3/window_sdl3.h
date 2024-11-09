/// \file Window_sdl3.h
/// Contains SDL3 implementation window-specific data types and functions
#pragma once

#include <kaze/core/lib.h>
#include <kaze/core/platform/backend/backend.h>
#include <kaze/core/platform/defines.h>

KAZE_NS_BEGIN
namespace backend {

    /// Data associated with each window
    struct WindowData
    {
        bool isHovered{false};
        bool cursorVisibleMode{true};
        CursorHandle cursor{};
        Uint textInputPropsId{};
    };

    /// Get window data associated with the window handle, if any
    /// \param[in]  window    window handle to get data from
    /// \param[out] outData   retrieves data pointer
    /// \returns whether retrieval was successful, also returns `false` if data was `nullptr`;
    ///          this behavior is default since lack of associated data is considered an
    ///          unexpected error.
    bool getWindowData(const WindowHandle window, WindowData **outData);
}

KAZE_NS_END
