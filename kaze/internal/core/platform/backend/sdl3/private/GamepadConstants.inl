/// \file GamepadConstants.inl
/// Contains gamepad constants, which map SDL constants with kaze constants
#pragma once

#include <kaze/internal/core/lib.h>
#include <kaze/internal/core/input/GamepadConstants.h>

#include <SDL3/SDL_gamepad.h>

KAZE_NS_BEGIN

static Array<int, static_cast<Size>(GamepadBtn::Count)> s_gamepadButtonToSDL = {
    SDL_GAMEPAD_BUTTON_SOUTH,
    SDL_GAMEPAD_BUTTON_EAST,
    SDL_GAMEPAD_BUTTON_WEST,
    SDL_GAMEPAD_BUTTON_NORTH,
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

static Array<int, static_cast<Size>(GamepadAxis::Count)> s_gamepadAxisToSDL = {
    SDL_GAMEPAD_AXIS_LEFTX,
    SDL_GAMEPAD_AXIS_LEFTY,
    SDL_GAMEPAD_AXIS_RIGHTX,
    SDL_GAMEPAD_AXIS_RIGHTY,
    SDL_GAMEPAD_AXIS_LEFT_TRIGGER,
    SDL_GAMEPAD_AXIS_RIGHT_TRIGGER,
};

static Array<int, SDL_GAMEPAD_BUTTON_COUNT> s_sdlToGamepadButton = {};
static Array<int, SDL_GAMEPAD_AXIS_COUNT> s_sdlToGamepadAxis = {};

KAZE_NS_END
