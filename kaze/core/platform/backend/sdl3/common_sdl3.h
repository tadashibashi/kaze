#pragma once

#include <kaze/core/lib.h>
#include <kaze/core/debug.h>

#include <kaze/core/input/InputEvents.h>
#include <kaze/core/input/GamepadConstants.h>
#include <kaze/core/input/KeyboardConstants.h>
#include <kaze/core/input/MouseConstants.h>

#include "private/GamepadMgr.h"

#include <SDL3/SDL.h>

/// Ensures a parameter is not null, passes a NullArgErr and returns false if so
/// Used in most backend functions, since they all return success and fail state via boolean.
#define RETURN_IF_NULL(obj) do { if ( !(obj) ) { \
    KAZE_CORE_ERRCODE(Error::NullArgErr, "required argument `{}` was null", #obj); \
    return false; \
} } while(0)

/// Convenience macro to cast void * => SDL_Window *
#define WIN_CAST(window) static_cast<SDL_Window *>(window)

KAZE_NAMESPACE_BEGIN

namespace backend {
    /// Convert an SDL button to a GamepadBtn
    /// \note make sure to call backend::initGlobals before calling this function
    /// \param[in] sdlButton   SDL_GamepadButton to convert
    /// \returns the equivalent GamepadBtn, assuming that the `sdlButton` is valid, otherwise the result is undefined.
    [[nodiscard]] auto toGamepadBtn(SDL_GamepadButton sdlButton) noexcept -> GamepadBtn;

    /// Convert a kaze::GamepadBtn to an SDL_GamepadButton
    /// \note make sure to call backend::initGlobals before calling this function
    /// \param[in] button   GamepadBtn to convert
    [[nodiscard]] auto toSDLGamepadButton(GamepadBtn button) noexcept -> SDL_GamepadButton;

    /// Convert an SDL_GamepadAxis to a kaze::GamepadAxis
    /// \note make sure to call backend::initGlobals before calling this function
    /// \param[in] sdlAxis   the SDL_GamepadAxis to convert
    /// \returns the equivalent GamepadAxis, granted the `sdlAxis` is valid, otherwise the result is undefined.
    [[nodiscard]] auto toGamepadAxis(SDL_GamepadAxis sdlAxis) noexcept -> GamepadAxis;

    /// Convert a kaze::GamepadAxis to an SDL_GamepadAxis
    /// \note make sure to call backend::initGlobals before calling this function
    /// \param[in] axis   the kaze::GamepadAxis to convert
    /// \returns the equivalent SDL_GamepadAxis, granted the `axis` is valid, otherwise the result is undefined.
    [[nodiscard]] auto toSDLAxis(GamepadAxis axis) noexcept -> SDL_GamepadAxis;

    /// Convert SDL_Scancode to kaze::Key
    /// \note make sure to call backend::initGlobals before calling this function
    /// \param[in] scancode   scancode to convert
    /// \returns the equivalent kaze::Key, granted the `scancode` is valid, otherwise the result is undefined.
    [[nodiscard]] auto toKey(SDL_Scancode scancode) noexcept -> Key;

    /// Convert kaze::Key to an SDL_Scancode
    /// \note make sure to call backend::initGlobals before calling this function
    /// \param[in] key   key to convert
    /// \returns the equivalen SDL_Scancode, granted the `key` is valid, otherwise the result is undefined.
    [[nodiscard]] auto toSDLKey(Key key) noexcept -> SDL_Scancode;

    /// Initialize the backend global variable internals. Must be called before calling any constant conversion func.
    auto initGlobals() noexcept -> void;

    extern GamepadMgr gamepads;
}

KAZE_NAMESPACE_END
