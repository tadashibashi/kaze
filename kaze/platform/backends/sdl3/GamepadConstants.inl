#pragma once
#ifndef kaze_platform_backends_sdl3_gamepadconstants_inl_
#define kaze_platform_backends_sdl3_gamepadconstants_inl_
#include <kaze/kaze.h>
#include <kaze/platform/Gamepad.h>

#include <SDL3/SDL_gamepad.h>

KAZE_NAMESPACE_BEGIN

static FixedArray<Uint8, static_cast<Size>(GamepadBtn::Count)> s_gamepadButtonToSDL = {
    SDL_GAMEPAD_BUTTON_LABEL_A,
    SDL_GAMEPAD_BUTTON_LABEL_B,
    SDL_GAMEPAD_BUTTON_LABEL_X,
    SDL_GAMEPAD_BUTTON_LABEL_Y,
    SDL_GAMEPAD_BUTTON_LEFT_SHOULDER,
    SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER,
    SDL_GAMEPAD_BUTTON_LEFT_STICK,
    SDL_GAMEPAD_BUTTON_RIGHT_STICK,
    SDL_GAMEPAD_BUTTON_BACK,
    SDL_GAMEPAD_BUTTON_START,
    SDL_GAMEPAD_BUTTON_GUIDE,
    SDL_GAMEPAD_BUTTON_DPAD_LEFT,
    SDL_GAMEPAD_BUTTON_DPAD_RIGHT,
    SDL_GAMEPAD_BUTTON_DPAD_UP,
    SDL_GAMEPAD_BUTTON_DPAD_DOWN,
};

static FixedArray<Uint8, static_cast<Size>(GamepadAxis::Count)> s_gamepadAxisToSDL = {
    SDL_GAMEPAD_AXIS_LEFTX,
    SDL_GAMEPAD_AXIS_LEFTY,
    SDL_GAMEPAD_AXIS_RIGHTX,
    SDL_GAMEPAD_AXIS_RIGHTY,
    SDL_GAMEPAD_AXIS_LEFT_TRIGGER,
    SDL_GAMEPAD_AXIS_RIGHT_TRIGGER,
};

static FixedArray<Uint8, SDL_GAMEPAD_BUTTON_COUNT> s_sdlToGamepadButton = {};
static FixedArray<Uint8, SDL_GAMEPAD_AXIS_COUNT> s_sdlToGamepadAxis = {};

KAZE_NAMESPACE_END

#endif // kaze_platform_backends_sdl3_gamepadconstants_inl_