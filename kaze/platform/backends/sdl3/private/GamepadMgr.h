/// @file Gamepad_sdl3.h
/// Contains classes and functions related to backend SDL3 Gamepad handling
#pragma once
#ifndef kaze_platform_backends_sdl3_gamepad_sdl3_h_
#define kaze_platform_backends_sdl3_gamepad_sdl3_h_

#include <kaze/kaze.h>
#include <kaze/input/GamepadConstants.h>
#include <kaze/platform/PlatformCallbacks.h>

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_gamepad.h>

KAZE_NAMESPACE_BEGIN

namespace backend
{
    struct ButtonData
    {
        bool isDown[2]{};        ///< value/last value, dependent on currentIndex of GamepadData
        uint8_t interactions{0}; ///< number of on/offs lined up to process
    };

    struct AxisData {
        static constexpr float NullInteraction = -123.456f; ///< sentry value for null interaction

        float value[2]{};    ///< value/last value, dependent on currentIndex of GamepadData
        float interaction{NullInteraction}; ///< stored value for this frame
    };

    struct GamepadData
    {
        Array<ButtonData, SDL_GAMEPAD_BUTTON_COUNT> buttons{};
        Array<AxisData, SDL_GAMEPAD_AXIS_COUNT> axes{};

        int currentIndex{0};     ///< current data set current, nullified is last
        int controllerIndex{-1}; ///< controller slot connected to (0-15)

        SDL_JoystickID joystickID {};
        SDL_Gamepad *gamepad{};

        void reset();

        void preProcessEvents();
        void postProcessEvents();
        void processEvent(const GamepadButtonEvent &e);
        void processEvent(const GamepadAxisEvent &e);

        [[nodiscard]] bool isDown(GamepadBtn btn) const noexcept;
        [[nodiscard]] float getAxis(GamepadAxis axis) const noexcept;
    };

    class GamepadMgr
    {
    public:
        static const char *DataKey;
        /// Connect a joystick id that was registered as connected in a Gamepad connection event
        /// @returns the index slot that the gamepad was placed in, or -1 on failure
        int connect(SDL_JoystickID id);

        /// Disconnect a gamepad owned by the GamepadMgr that was received in a gamepad disconnection event
        /// @param id joystick id of the controller to remove
        /// @returns  index slot of the gamepad, or -1 on error;
        ///           if `-1`, the manager probably doesn't own it, or it is an invalid id
        int disconnect(SDL_JoystickID id);

        GamepadData *operator[](int index);
        const GamepadData *operator[](int index) const;

        void preProcessEvents();
        void postProcessEvents();

        void processEvent(const GamepadButtonEvent &e);
        void processEvent(const GamepadAxisEvent &e);

        static void initGamepadConstants();

    private:

        static bool emplaceNew(SDL_Gamepad *gamepad, int controllerIndex);
        Array<SDL_Gamepad *, 16> m_gamepads{};
    };
}



KAZE_NAMESPACE_END

#endif // kaze_platform_backends_sdl3_gamepad_sdl3_h_
