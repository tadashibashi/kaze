/// @file PlatformBackend_globals.cpp
/// @description
/// Implementation for constant conversions and global variables
#include "PlatformBackend_globals.h"

#include "GamepadConstants.inl"
#include "KeyboardConstants.inl"

KAZE_NAMESPACE_BEGIN

namespace backend {
    /// singleton gamepads manager
    sdl3::GamepadMgr gamepads{};

    auto toGamepadBtn(const SDL_GamepadButton sdlButton) noexcept -> GamepadBtn
    {
        return static_cast<GamepadBtn>(s_sdlToGamepadButton[sdlButton]);
    }

    auto toSDLGamepadButton(const GamepadBtn button) noexcept -> SDL_GamepadButton
    {
        return static_cast<SDL_GamepadButton>(s_gamepadButtonToSDL[ static_cast<int>(button) ]);
    }

    auto toSDLAxis(GamepadAxis axis) noexcept -> SDL_GamepadAxis
    {
        return static_cast<SDL_GamepadAxis>(s_gamepadAxisToSDL[ static_cast<int>(axis) ]);
    }

    auto toGamepadAxis(SDL_GamepadAxis sdlAxis) noexcept -> GamepadAxis
    {
        return static_cast<GamepadAxis>(s_sdlToGamepadAxis[ static_cast<int>(sdlAxis) ]);
    }

    auto toKey(SDL_Scancode scancode) noexcept -> Key
    {
        return static_cast<Key>(s_sdlKeyToKey[ static_cast<int>(scancode) ]);
    }

    auto toSDLKey(Key key) noexcept -> SDL_Scancode
    {
        return static_cast<SDL_Scancode>(s_keyToSdlKey[ static_cast<int>(key) ]);
    }

    auto initGlobals() noexcept -> void
    {
        // Initialize SDL key to kaze::Key array
        if (s_sdlKeyToKey[SDL_SCANCODE_Z] == 0)
        {
            for (Uint16 i = 0; const auto key : s_keyToSdlKey)
            {
                s_sdlKeyToKey[key] = i++;
            }
        }

        // Initialize SDL gamepad buttons to kaze::GamepadBtn array
        if (s_sdlToGamepadButton[SDL_GAMEPAD_BUTTON_COUNT-1] == 0)
        {
            for (Uint8 i = 0; const auto btn : s_gamepadButtonToSDL)
            {
                s_sdlToGamepadButton[btn] = i++;
            }
        }

        // Initialize SDL gamepad axis to kaze::GamepadAxis array
        if (s_sdlToGamepadAxis[SDL_GAMEPAD_AXIS_COUNT-1] == 0)
        {
            for (Uint8 i = 0; const auto axis : s_gamepadAxisToSDL)
            {
                s_sdlToGamepadAxis[axis] = i++;
            }
        }
    }
}

KAZE_NAMESPACE_END
