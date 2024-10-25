/// \file Gamepad_sdl3.h
/// Contains classes and functions related to backend SDL3 Gamepad handling
#pragma once

#include <kaze/core/lib.h>
#include <kaze/core/input/GamepadConstants.h>
#include <kaze/core/platform/backend/backend.h>

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

        auto reset() noexcept -> void;
        auto preProcessEvents() noexcept -> void;
        auto postProcessEvents() noexcept -> void;
        auto processEvent(const GamepadButtonEvent &e) noexcept -> void;
        auto processEvent(const GamepadAxisEvent &e) noexcept -> void;

        [[nodiscard]] auto isDown(GamepadBtn btn) const noexcept -> bool;
        [[nodiscard]] auto getAxis(GamepadAxis axis) const noexcept -> float;
    };

    class GamepadMgr
    {
    public:
        static const char *DataKey;
        /// Connect a joystick id that was registered as connected in a Gamepad connection event
        /// \returns the index slot that the gamepad was placed in, or -1 on failure
        auto connect(SDL_JoystickID id) noexcept -> int;

        /// Disconnect a gamepad owned by the GamepadMgr that was received in a gamepad disconnection event
        /// \param[in] id   joystick id of the controller to remove
        /// \returns  index slot of the gamepad, or -1 on error;
        ///           if `-1`, the manager probably doesn't own it, or it is an invalid id
        auto disconnect(SDL_JoystickID id) noexcept -> int;

        auto operator[](int index) noexcept -> GamepadData *;
        auto operator[](int index) const noexcept -> const GamepadData *;

        auto preProcessEvents() noexcept -> void;
        auto postProcessEvents() noexcept -> void;
        auto processEvent(const GamepadButtonEvent &e) noexcept -> void;
        auto processEvent(const GamepadAxisEvent &e) noexcept -> void;

    private:

        static auto emplaceNew(SDL_Gamepad *gamepad, int controllerIndex) noexcept -> bool;
        Array<SDL_Gamepad *, GamepadMaxSlots> m_gamepads{};
    };
}



KAZE_NAMESPACE_END
