/// @file PlatformBackend_globals.cpp
/// Implementation for constant conversions and global variables
#include "common_sdl3.h"
#include "window_sdl3.h"

#include <kaze/platform/backend.h>

#include "private/GamepadConstants.inl"
#include "private/KeyboardConstants.inl"

KAZE_NAMESPACE_BEGIN

namespace backend {
    /// singleton gamepads manager
    GamepadMgr gamepads{};

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

        /// Event filter to handle resizing and move callbacks, which would otherwise block on the main thread
    /// This means that callbacks should be relatively lightweight and also thread-safe
    static auto sdlEventFilter(void *userdata, SDL_Event *event) -> bool
    {
        auto &e = *event;
        switch (e.type)
        {
        case SDL_EVENT_WINDOW_RESIZED:
            {
                events.emit(WindowEvent {
                    .type = WindowEvent::Resized,
                    .data0 = event->window.data1,
                    .data1 = event->window.data2,
                    .window = SDL_GetWindowFromID(e.window.windowID),
                });
            } return false;

        case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
            {
                events.emit(WindowEvent {
                    .type = WindowEvent::ResizedFramebuffer,
                    .data0 = event->window.data1,
                    .data1 = event->window.data2,
                    .window = SDL_GetWindowFromID(e.window.windowID),
                });
            } return false;
        case SDL_EVENT_WINDOW_MOVED:
            {
                const auto window = SDL_GetWindowFromID(e.window.windowID);
                events.emit(WindowEvent {
                    .type = WindowEvent::Moved,
                    .data0 = e.window.data1,
                    .data1 = e.window.data2,
                    .window = window,
                });
            } return false;
        default:
            return true;
        }
    }

    auto init() noexcept -> bool
    {
        if ( !SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD) )
        {
            KAZE_CORE_ERRCODE(Error::BE_RuntimeErr, "Failed to initialize SDL3: {}", SDL_GetError());
            return KAZE_FALSE;
        }

        backend::initGlobals();

        SDL_SetEventFilter(sdlEventFilter, nullptr);

        return KAZE_TRUE;
    }

    auto shutdown() noexcept -> void
    {
        SDL_Quit();
    }

    auto getTime(double *outTime) noexcept -> bool
    {
        RETURN_IF_NULL(outTime);

        *outTime = static_cast<double>(SDL_GetTicksNS()) * 1e-9;
        return true;
    }

    auto getClipboard(const char **outText) noexcept -> bool
    {
        RETURN_IF_NULL(outText);

        static String text;
        if (SDL_HasClipboardText())
        {
            const auto cstr = SDL_GetClipboardText();
            if ( !cstr || *cstr == '\0') // failed to get text
            {
                SDL_free(cstr);
                KAZE_CORE_ERRCODE(Error::BE_RuntimeErr, "Failed to get clipboard text: {}", SDL_GetError());
                return false;
            }

            text.assign(cstr);
            *outText = text.c_str();
            return true;
        }

        *outText = "";
        return true;
    }

    auto setClipboard(const char *text) noexcept -> bool
    {
        if ( !SDL_SetClipboardText(text) )
        {
            KAZE_CORE_ERR("Failed to set clipboard text to \"{}\": {}",
                (text ? text : ""), SDL_GetError());
            return false;
        }

        return true;
    }

    auto pollEvents() noexcept -> bool
    {
        gamepads.preProcessEvents();

        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            switch (e.type)
            {
            case SDL_EVENT_KEY_UP:
            case SDL_EVENT_KEY_DOWN:
                {
                    events.emit(KeyboardEvent {
                        .type     = e.key.down ? KeyboardEvent::Down : KeyboardEvent::Up,
                        .key      = backend::toKey(e.key.scancode),
                        .isRepeat = e.key.repeat,
                    });
                } break;

            case SDL_EVENT_MOUSE_WHEEL:
                {
                    const auto window = SDL_GetWindowFromID(e.wheel.windowID);
                    const auto offset = (e.wheel.direction == SDL_MOUSEWHEEL_FLIPPED) ?
                        Vec2f{ -e.wheel.x, -e.wheel.y } :
                        Vec2f{ e.wheel.x, e.wheel.y };
                    events.emit(MouseScrollEvent {
                        .offset = offset,
                        .window = window,
                    });
                } break;

            case SDL_EVENT_MOUSE_BUTTON_UP:
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                {
                    const auto window = SDL_GetWindowFromID(e.button.windowID);
                    events.emit(MouseButtonEvent {
                        .type = e.button.down ? MouseButtonEvent::Down : MouseButtonEvent::Up,
                        .button = static_cast<MouseBtn>(e.button.button - 1),
                        .window = window
                    });
                } break;

            case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                {
                    const auto window = SDL_GetWindowFromID(e.window.windowID);
                    events.emit(WindowEvent {
                        .type = WindowEvent::Closed,
                        .window = window,
                    });
                } break;

            case SDL_EVENT_WINDOW_FOCUS_GAINED:
                {
                    const auto window = SDL_GetWindowFromID(e.window.windowID);
                    events.emit(WindowEvent {
                        .type = WindowEvent::FocusGained,
                        .window = window,
                    });
                } break;

            case SDL_EVENT_WINDOW_FOCUS_LOST:
                {
                    const auto window = SDL_GetWindowFromID(e.window.windowID);
                    events.emit(WindowEvent {
                        .type = WindowEvent::FocusLost,
                        .window = window,
                    });
                } break;

            case SDL_EVENT_WINDOW_MOUSE_ENTER:
                {
                    const auto window = SDL_GetWindowFromID(e.window.windowID);
                    events.emit(WindowEvent {
                        .type = WindowEvent::MouseEntered,
                        .window = window
                    });

                    WindowData *data;
                    if ( !getWindowData(window, &data) )
                        break;
                    data->isHovered = true;

                    if (data->cursorVisibleMode)
                    {
                        SDL_ShowCursor();
                    }
                    else
                    {
                        SDL_HideCursor();
                    }
                } break;

            case SDL_EVENT_WINDOW_MOUSE_LEAVE:
                {
                    const auto window = SDL_GetWindowFromID(e.window.windowID);
                    events.emit(WindowEvent {
                        .type = WindowEvent::MouseExited,
                        .window = window
                    });

                    WindowData *data;
                    if ( !getWindowData(window, &data) )
                        break;
                    data->isHovered = false;
                } break;

            case SDL_EVENT_WINDOW_MINIMIZED:
                {
                    const auto window = SDL_GetWindowFromID(e.window.windowID);
                    events.emit(WindowEvent {
                        .type = WindowEvent::Minimized,
                        .window = window,
                    });
                } break;

            case SDL_EVENT_WINDOW_MAXIMIZED:
                {
                    const auto window = SDL_GetWindowFromID(e.window.windowID);
                    events.emit(WindowEvent {
                        .type = WindowEvent::Maximized,
                        .window = window,
                    });
                } break;

            case SDL_EVENT_WINDOW_RESTORED:
                {
                    const auto window = SDL_GetWindowFromID(e.window.windowID);
                    events.emit(WindowEvent {
                        .type = WindowEvent::Restored,
                        .window = window,
                    });
                } break;

            case SDL_EVENT_GAMEPAD_ADDED:
                {
                    const auto index = gamepads.connect(e.gdevice.which);
                    if (index >= 0)
                    {
                        events.emit(GamepadConnectEvent {
                            .type = GamepadConnectEvent::Connected,
                            .id = index,
                        });
                    }
                } break;

            case SDL_EVENT_GAMEPAD_REMOVED:
                {
                    const auto index = gamepads.disconnect(e.gdevice.which);
                    if (index >= 0)
                    {
                        events.emit(GamepadConnectEvent {
                            .type = GamepadConnectEvent::Disconnected,
                            .id = index,
                        });
                    }
                } break;

            case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
            case SDL_EVENT_GAMEPAD_BUTTON_UP:
                {
                    auto gamepad = SDL_GetGamepadFromID(e.gbutton.which);
                    if (gamepad)
                    {
                        auto props = SDL_GetGamepadProperties(gamepad);
                        if (props)
                        {
                            auto data = static_cast<GamepadData *>(SDL_GetPointerProperty(props, GamepadMgr::DataKey, nullptr));
                            if (data)
                            {
                                if (data->controllerIndex >= 0)
                                {
                                    const auto event = GamepadButtonEvent {
                                        .controllerIndex = data->controllerIndex,
                                        .type = e.gbutton.down ? GamepadButtonEvent::Down : GamepadButtonEvent::Up,
                                        .button = backend::toGamepadBtn(static_cast<SDL_GamepadButton>(e.gbutton.button)),
                                    };
                                    backend::gamepads.processEvent(event);
                                    backend::events.emit(event);
                                }
                            }
                        }
                    }
                } break;

            case SDL_EVENT_GAMEPAD_AXIS_MOTION:
                {
                    auto gamepad = SDL_GetGamepadFromID(e.gaxis.which);
                    if (gamepad)
                    {
                        auto props = SDL_GetGamepadProperties(gamepad);
                        if (props)
                        {
                            auto data = static_cast<GamepadData *>(SDL_GetPointerProperty(props, GamepadMgr::DataKey, nullptr));
                            if (data)
                            {
                                if (data->controllerIndex >= 0)
                                {
                                    const auto event = GamepadAxisEvent {
                                        .controllerIndex = data->controllerIndex,
                                        .axis = backend::toGamepadAxis( static_cast<SDL_GamepadAxis>(e.gaxis.axis) ),
                                        .value = e.gaxis.value > 0 ?
                                            (float)e.gaxis.value / (float)std::numeric_limits<decltype(e.gaxis.value)>::max() :
                                            (float)e.gaxis.value / -(float)std::numeric_limits<decltype(e.gaxis.value)>::min(),
                                    };
                                    gamepads.processEvent(event);
                                    events.emit(event);
                                }
                            }
                        }
                    }
                } break;

            case SDL_EVENT_MOUSE_MOTION:
                {
                    events.emit(MouseMotionEvent {
                        .position = {
                            e.motion.x,
                            e.motion.y,
                        },
                        .window = SDL_GetWindowFromID(e.motion.windowID)
                    });
                } break;

            case SDL_EVENT_DROP_FILE:
                {
                    const auto window = SDL_GetWindowFromID(e.drop.windowID);
                    events.emit(FileDropEvent {
                        .path = e.drop.data,
                        .window = window,
                        .position = { e.drop.x, e.drop.y },
                    });
                } break;
            default:
                break;
            }
        }

        gamepads.postProcessEvents();
        return KAZE_TRUE;
    }

    /// Mouse and keyboard functions are here since there's not many of them to justify their own separate file...

    auto keyboard::isDown(Key key, bool *outDown) noexcept -> bool
    {
        RETURN_IF_NULL(outDown);

        const auto keys = SDL_GetKeyboardState(nullptr);
        if ( !keys )
        {
            KAZE_CORE_ERRCODE(Error::BE_RuntimeErr, "Failed to get keyboard state: {}", SDL_GetError());
            return false;
        }

        const auto sdlKey = backend::toSDLKey(key);

        *outDown = keys[sdlKey];
        return true;
    }

    auto mouse::getRelativePosition(const WindowHandle window, float *x, float *y) noexcept -> bool
    {
        RETURN_IF_NULL(window);

        float tempX, tempY;
        SDL_GetGlobalMouseState(&tempX, &tempY);

        int winX, winY;
        if ( !SDL_GetWindowPosition( WIN_CAST(window), &winX, &winY ) )
        {
            KAZE_CORE_ERRCODE(Error::BE_RuntimeErr, "Failed to get window position: {}", SDL_GetError());
            return false;
        }

        if (x)
            *x = tempX - winX;
        if (y)
            *y = tempY - winY;

        return true;
    }

    auto mouse::getGlobalPosition(float *x, float *y) noexcept -> bool
    {
        SDL_GetGlobalMouseState(x, y);
        return true;
    }

    auto mouse::isDown(const WindowHandle window, kaze::MouseBtn button, bool *outDown) noexcept -> bool
    {
        RETURN_IF_NULL(window);
        RETURN_IF_NULL(outDown);

        *outDown = static_cast<bool>( SDL_GetMouseState(nullptr, nullptr) & static_cast<uint32_t>(button) );
        return true;
    }
}

KAZE_NAMESPACE_END
