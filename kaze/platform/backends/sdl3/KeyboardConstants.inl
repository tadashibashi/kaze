#pragma once
#ifndef kaze_platform_backends_sdl3_keyboardconstants_inl_
#define kaze_platform_backends_sdl3_keyboardconstants_inl_
#include <kaze/kaze.h>
#include <kaze/platform/Key.h>

#include <SDL3/SDL_keyboard.h>

KAZE_NAMESPACE_BEGIN

static constexpr FixedArray<Uint16, static_cast<Size>(Key::Count)> s_keyToSdlKey = {
    SDL_SCANCODE_ESCAPE,
    SDL_SCANCODE_RETURN,
    SDL_SCANCODE_TAB,
    SDL_SCANCODE_SPACE,
    SDL_SCANCODE_BACKSPACE,
    SDL_SCANCODE_UP,
    SDL_SCANCODE_DOWN,
    SDL_SCANCODE_LEFT,
    SDL_SCANCODE_RIGHT,
    SDL_SCANCODE_INSERT,
    SDL_SCANCODE_DELETE,
    SDL_SCANCODE_HOME,
    SDL_SCANCODE_END,
    SDL_SCANCODE_PAGEUP,
    SDL_SCANCODE_PAGEDOWN,
    SDL_SCANCODE_PRINTSCREEN,
    SDL_SCANCODE_EQUALS,
    SDL_SCANCODE_MINUS,
    SDL_SCANCODE_LEFTBRACKET,
    SDL_SCANCODE_RIGHTBRACKET,
    SDL_SCANCODE_SEMICOLON,
    SDL_SCANCODE_APOSTROPHE,
    SDL_SCANCODE_COMMA,
    SDL_SCANCODE_PERIOD,
    SDL_SCANCODE_SLASH,
    SDL_SCANCODE_BACKSLASH,
    SDL_SCANCODE_GRAVE,
    SDL_SCANCODE_CAPSLOCK,
    SDL_SCANCODE_NUMLOCKCLEAR,

    SDL_SCANCODE_F1,
    SDL_SCANCODE_F2,
    SDL_SCANCODE_F3,
    SDL_SCANCODE_F4,
    SDL_SCANCODE_F5,
    SDL_SCANCODE_F6,
    SDL_SCANCODE_F7,
    SDL_SCANCODE_F8,
    SDL_SCANCODE_F9,
    SDL_SCANCODE_F10,
    SDL_SCANCODE_F11,
    SDL_SCANCODE_F12,

    SDL_SCANCODE_LSHIFT,
    SDL_SCANCODE_RSHIFT,
    SDL_SCANCODE_LALT,
    SDL_SCANCODE_RALT,
    SDL_SCANCODE_LCTRL,
    SDL_SCANCODE_RCTRL,
    SDL_SCANCODE_LGUI,
    SDL_SCANCODE_RGUI,

    SDL_SCANCODE_KP_0,
    SDL_SCANCODE_KP_1,
    SDL_SCANCODE_KP_2,
    SDL_SCANCODE_KP_3,
    SDL_SCANCODE_KP_4,
    SDL_SCANCODE_KP_5,
    SDL_SCANCODE_KP_6,
    SDL_SCANCODE_KP_7,
    SDL_SCANCODE_KP_8,
    SDL_SCANCODE_KP_9,

    SDL_SCANCODE_KP_PLUS,
    SDL_SCANCODE_KP_MINUS,
    SDL_SCANCODE_KP_MULTIPLY,
    SDL_SCANCODE_KP_DIVIDE,
    SDL_SCANCODE_KP_DECIMAL,
    SDL_SCANCODE_KP_ENTER,

    SDL_SCANCODE_0,
    SDL_SCANCODE_1,
    SDL_SCANCODE_2,
    SDL_SCANCODE_3,
    SDL_SCANCODE_4,
    SDL_SCANCODE_5,
    SDL_SCANCODE_6,
    SDL_SCANCODE_7,
    SDL_SCANCODE_8,
    SDL_SCANCODE_9,

    SDL_SCANCODE_A,
    SDL_SCANCODE_B,
    SDL_SCANCODE_C,
    SDL_SCANCODE_D,
    SDL_SCANCODE_E,
    SDL_SCANCODE_F,
    SDL_SCANCODE_G,
    SDL_SCANCODE_H,
    SDL_SCANCODE_I,
    SDL_SCANCODE_J,
    SDL_SCANCODE_K,
    SDL_SCANCODE_L,
    SDL_SCANCODE_M,
    SDL_SCANCODE_N,
    SDL_SCANCODE_O,
    SDL_SCANCODE_P,
    SDL_SCANCODE_Q,
    SDL_SCANCODE_R,
    SDL_SCANCODE_S,
    SDL_SCANCODE_T,
    SDL_SCANCODE_U,
    SDL_SCANCODE_V,
    SDL_SCANCODE_W,
    SDL_SCANCODE_X,
    SDL_SCANCODE_Y,
    SDL_SCANCODE_Z,
};

static FixedArray<Uint16, SDL_SCANCODE_COUNT> s_sdlKeyToKey = {0};

KAZE_NAMESPACE_END

#endif // kaze_platform_backends_sdl3_keyboardconstants_inl_
