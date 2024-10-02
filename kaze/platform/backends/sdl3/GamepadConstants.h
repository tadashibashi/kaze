#pragma once
#ifndef kaze_platform_backends_sdl3_gamepadconstants_h_
#define kaze_platform_backends_sdl3_gamepadconstants_h_

#include <kaze/kaze.h>
#include <kaze/platform/Gamepad.h>
#include <SDL3/SDL_gamepad.h>

KAZE_NAMESPACE_BEGIN
namespace backend {
    /// Convert an SDL button to a GamepadBtn
    /// Make sure to call PlatformBackend::init before calling this function
    GamepadBtn sdlToGamepadButton(Uint8 sdlButton);
    SDL_GamepadButton gamepadButtonToSdl(GamepadBtn button);
}
KAZE_NAMESPACE_END

#endif // kaze_platform_backends_sdl3_gamepadconstants_h_
