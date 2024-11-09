#pragma once
#include <kaze/core/lib.h>
#include <SDL3/SDL.h>

KAZE_NS_BEGIN

namespace backend {
    /// Init Window for Apple Devices, and returns a metal layer
    /// \param[in]  window  Window to initialize Metal in
    /// \returns whether function succeeded
    auto uiKitWindowInitMetal(SDL_Window *window) -> bool;

    /// (Currently no-op until I can get it working)
    /// \param[in]  window     Window to set the text input on
    /// \param[in]  textInput  `true` turns text input mode on; `false` turns it off
    /// \returns whether function succeeded
    auto uiKitWindowSetTextInput(SDL_Window *window, bool textInput) -> bool;

    /// Get the metal layer from a successfully initialized `uiKitWindowInitMetal`.
    /// \param[in]  window         Window to get metal layer from
    /// \param[out] outMetalLayer  variable to receive the pointer to the CAMetalLayer
    /// \returns whether function succeeded
    auto uiKitWindowGetMetalLayer(SDL_Window *window, void **outMetalLayer) -> bool;
}

KAZE_NS_END
