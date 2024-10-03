/// @file PlatformBackend_globals.h
/// @description
/// Contains private implementation helper functions that should be available to different backend implementation files
#pragma once
#ifndef kaze_platform_backends_sdl3_gamepadconstants_h_
#define kaze_platform_backends_sdl3_gamepadconstants_h_

#include "Gamepad_sdl3.h"

#include <kaze/kaze.h>
#include <kaze/platform/Gamepad.h>
#include <kaze/platform/Key.h>

#include <SDL3/SDL_gamepad.h>
#include <SDL3/SDL_keyboard.h>

KAZE_NAMESPACE_BEGIN

namespace backend {
    /// Convert an SDL button to a GamepadBtn
    /// @note make sure to call backend::initGlobals before calling this function
    /// @param sdlButton SDL_GamepadButton to convert
    /// @return the equivalent GamepadBtn, assuming that the `sdlButton` is valid, otherwise the result is undefined.
    [[nodiscard]] auto toGamepadBtn(SDL_GamepadButton sdlButton) noexcept -> GamepadBtn;

    /// Convert a kaze::GamepadBtn to an SDL_GamepadButton
    /// @note make sure to call backend::initGlobals before calling this function
    /// @param button GamepadBtn to convert
    [[nodiscard]] auto toSDLGamepadButton(GamepadBtn button) noexcept -> SDL_GamepadButton;

    /// Convert an SDL_GamepadAxis to a kaze::GamepadAxis
    /// @note make sure to call backend::initGlobals before calling this function
    /// @param sdlAxis the SDL_GamepadAxis to convert
    /// @returns the equivalent GamepadAxis, granted the `sdlAxis` is valid, otherwise the result is undefined.
    [[nodiscard]] auto toGamepadAxis(SDL_GamepadAxis sdlAxis) noexcept -> GamepadAxis;

    /// Convert a kaze::GamepadAxis to an SDL_GamepadAxis
    /// @note make sure to call backend::initGlobals before calling this function
    /// @param axis the kaze::GamepadAxis to convert
    /// @returns the equivalent SDL_GamepadAxis, granted the `axis` is valid, otherwise the result is undefined.
    [[nodiscard]] auto toSDLAxis(GamepadAxis axis) noexcept -> SDL_GamepadAxis;

    /// Convert SDL_Scancode to kaze::Key
    /// @note make sure to call backend::initGlobals before calling this function
    /// @param scancode scancode to convert
    /// @returns the equivalent kaze::Key, granted the `scancode` is valid, otherwise the result is undefined.
    [[nodiscard]] auto toKey(SDL_Scancode scancode) noexcept -> Key;

    /// Convert kaze::Key to an SDL_Scancode
    /// @note make sure to call backend::initGlobals before calling this function
    /// @param key key to convert
    /// @returns the equivalen SDL_Scancode, granted the `key` is valid, otherwise the result is undefined.
    [[nodiscard]] auto toSDLKey(Key key) noexcept -> SDL_Scancode;

    /// Initialize the backend global variable internals. Must be called before calling any constant conversion func.
    auto initGlobals() noexcept -> void;

    extern sdl3::GamepadMgr gamepads;
}
KAZE_NAMESPACE_END

#endif // kaze_platform_backends_sdl3_gamepadconstants_h_
