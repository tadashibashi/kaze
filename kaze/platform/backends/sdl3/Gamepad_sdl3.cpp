#include "Gamepad_sdl3.h"

#include <kaze/debug.h>

#include "GamepadConstants.h"

KAZE_NAMESPACE_BEGIN
    namespace backend::sdl3 {

    int GamepadMgr::connect(const SDL_JoystickID id)
    {
        constexpr int CONNECT_FAILED = -1;

        // find next free index
        int index = 0;
        while (index < m_gamepads.size())
        {
            if ( !m_gamepads[index] )
                break;
            ++index;
        }

        // check that a slot is available
        if (index >= m_gamepads.size())
        {
            KAZE_CORE_ERR("Failed to connect controller with jid {}: too many controllers are open",
                          static_cast<int>(id));
            return CONNECT_FAILED;
        }

        // create the gamepad
        const auto gamepad = SDL_OpenGamepad(id);
        if ( !gamepad )
        {
            KAZE_CORE_ERR("Failed to open gamepad with id {}: {}",
                          static_cast<int>(id), SDL_GetError());
            return CONNECT_FAILED;
        }

        // emplace user data
        if ( !GamepadMgr::emplaceNew(gamepad, index) )
        {
            SDL_CloseGamepad(gamepad);

            // Don't log error, let the emplaceNew error remain
            return CONNECT_FAILED;
        }

        // emplace in the gamepads array
        m_gamepads[index] = gamepad;
        return index;
    }

    int GamepadMgr::disconnect(const SDL_JoystickID id)
    {
        constexpr int DISCONNECT_FAILED = -1;

        const auto gamepad = SDL_GetGamepadFromID(id);
        if ( !gamepad )
        {
            KAZE_CORE_WARN("Attempted to delete a gamepad with id, but no gamepad was available: {}",
                           SDL_GetError());
            return DISCONNECT_FAILED;
        }

        const auto props = SDL_GetGamepadProperties(gamepad);
        if ( !props )
        {
            KAZE_CORE_WARN("Failed to get properties from gamepad: {}", SDL_GetError());
            return DISCONNECT_FAILED;
        }

        const auto data = static_cast<GamepadData *>(SDL_GetPointerProperty(props, GamepadMgr::DataKey, nullptr));
        if ( !data )
        {
            KAZE_CORE_WARN("Failed to get GamepadData pointer from Gamepad properties");
            return DISCONNECT_FAILED;
        }

        const auto gamepadIndex = data->controllerIndex;

        if (gamepadIndex < 0 || gamepadIndex >= 16) // 16 is the max number of gamepad slots
        {
            KAZE_CORE_WARN("Gamepad has an invalid controllerIndex");
            return DISCONNECT_FAILED;
        }

        m_gamepads[gamepadIndex] = nullptr;
        SDL_CloseGamepad(gamepad);
        return gamepadIndex;
    }

    void GamepadMgr::preProcessEvents()
    {
        for (int i = 0; i < m_gamepads.size(); ++i)
        {
            auto data = operator[](i);
            if (data)
            {
                data->preProcessEvents();
            }
        }
    }

    void GamepadMgr::postProcessEvents()
    {
        for (int i = 0; i < m_gamepads.size(); ++i)
        {
            auto data = operator[](i);
            if (data)
            {
                data->postProcessEvents();
            }
        }
    }

    void GamepadMgr::processEvent(const GamepadButtonEvent &e)
    {
        auto data = operator[](e.controllerIndex);
        if (data)
        {
            data->processEvent(e);
        }
    }

    void GamepadMgr::processEvent(const GamepadAxisEvent &e)
    {
        auto data = operator[](e.controllerIndex);
        if (data)
        {
            data->processEvent(e);
        }
    }

    GamepadData *sdl3::GamepadMgr::operator[](const int index)
    {
        const auto gamepad = m_gamepads[index];
        if ( !gamepad )
            return nullptr;

        const auto props = SDL_GetGamepadProperties(gamepad);
        if ( !props )
        {
            KAZE_CORE_ERR("Internal error: failed to get properties from gamepad: {}",
                SDL_GetError());
            return nullptr;
        }

        return static_cast<GamepadData *>(SDL_GetPointerProperty(props, GamepadMgr::DataKey, nullptr));
    }

    const sdl3::GamepadData * sdl3::GamepadMgr::operator[](const int index) const
    {
        const auto gamepad = m_gamepads[index];
        if ( !gamepad )
            return nullptr;

        const auto props = SDL_GetGamepadProperties(gamepad);
        if ( !props )
        {
            KAZE_CORE_ERR("Internal error: failed to get properties from gamepad: {}",
                SDL_GetError());
            return nullptr;
        }

        return static_cast<GamepadData *>(SDL_GetPointerProperty(props, GamepadMgr::DataKey, nullptr));
    }

    const char *GamepadMgr::DataKey = "GamepadData";

    bool GamepadMgr::emplaceNew(SDL_Gamepad *gamepad, const int controllerIndex)
    {
        /// Ensure this is a valid gamepad
        const auto id = SDL_GetGamepadID(gamepad);
        if (!id)
        {
            KAZE_CORE_ERR("Failed to get gamepad id: {}", SDL_GetError());
            return false;
        }

        // Emplace data via properties API
        const auto props = SDL_GetGamepadProperties(gamepad);
        if (!props)
        {
            KAZE_CORE_ERR("Failed to get gamepad properties for joystick id {}: {}",
                          static_cast<int>(id), SDL_GetError());
            return false;
        }

        const auto data = new GamepadData();
        data->controllerIndex = controllerIndex;
        data->joystickID = id;
        data->gamepad = gamepad;

        const auto result = SDL_SetPointerPropertyWithCleanup(props, GamepadMgr::DataKey, data,
            [](void *userdata, void *value) {
                delete static_cast<GamepadData *>(value);
        }, nullptr);

        if ( !result )
        {
            KAZE_CORE_ERR("Failed to set data property on the SDL_Gamepad: {}", SDL_GetError());
            delete data; // clean up b/c ownership has not passed to the SDL_Gamepad successfully
            return false;
        }

        return true;
    }

    void GamepadData::reset()
    {
        std::memset(buttons.data(), 0, sizeof(buttons));
        for (auto &axis : axes)
        {
            axis.value[0] = 0;
            axis.value[1] = 0;
            axis.interaction = AxisData::NullInteraction;
        }
        currentIndex = 0;
    }

    void GamepadData::preProcessEvents()
    {
        currentIndex = !currentIndex;
    }

    void GamepadData::postProcessEvents()
    {
        // Process buttons
        for (int i = 0; i < buttons.size(); ++i)
        {
            auto &button = buttons[i];
            if (button.interactions > 0)
            {
                --button.interactions;
                if (button.interactions == 0)
                {
                    button.isDown[currentIndex] = SDL_GetGamepadButton(gamepad,
                        gamepadButtonToSdl(static_cast<GamepadBtn>(i)));
                }
                else
                {
                    button.isDown[currentIndex] = !button.isDown[!currentIndex];
                }

            }
            else
            {
                button.isDown[currentIndex] = button.isDown[!currentIndex];
            }
        }

        // Process axes
        for (int i = 0; i < axes.size(); ++i)
        {
            auto &axis = axes[i];
            if (axis.interaction != AxisData::NullInteraction) // sentry value
            {
                // Set value to interaction captured this frame
                axis.value[currentIndex] = axis.interaction;
                axis.interaction = AxisData::NullInteraction;
            }
            else
            {
                // just copy last value, no change was made
                axis.value[currentIndex] = axis.value[!currentIndex];
            }
        }
    }

    void GamepadData::processEvent(const GamepadButtonEvent &e)
    {
        KAZE_ASSERT(static_cast<Uint8>(e.button) < static_cast<Uint8>(GamepadBtn::Count) &&
            static_cast<Uint8>(e.button) >= 0);
        ++buttons[static_cast<Uint8>(e.button)].interactions;
    }

    void GamepadData::processEvent(const GamepadAxisEvent &e)
    {
        KAZE_ASSERT(static_cast<Uint8>(e.axis) < static_cast<Uint8>(GamepadAxis::Count) &&
            static_cast<Uint8>(e.axis) >= 0);
        axes[static_cast<Uint8>(e.axis)].interaction = e.value;
    }
}



KAZE_NAMESPACE_END
