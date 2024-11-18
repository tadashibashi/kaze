/// \file PlatformBackend_globals.cpp
/// Implementation for constant conversions and global variables
#include "common_sdl3.h"
#include "SDL3/SDL_events.h"
#include "window_sdl3.h"
#include "private/GamepadConstants.inl"
#include "private/KeyboardConstants.inl"
#include "private/MouseConstants.inl"

#include <kaze/core/memory.h>
#include <kaze/core/platform/backend/backend.h>

#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_video.h>

KAZE_NS_BEGIN

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

    auto toMouseBtn(Uint8 button) noexcept -> MouseBtn
    {
        return s_toMouseButton[ button ];
    }

    auto toSDLMouseBtn(MouseBtn button) noexcept -> Uint8
    {
        return s_toSDLMouseButton[ static_cast<int>(button) ];
    }

    auto initGlobals() noexcept -> void
    {
        // Initialize SDL key to kaze::Key array
        for (int i = 0; i < s_sdlKeyToKey.size(); ++i)
            s_sdlKeyToKey[i] = -1;

        for (Uint16 i = 0; const auto key : s_keyToSdlKey)
        {
            s_sdlKeyToKey[key] = i++;
        }

        // Initialize SDL gamepad buttons to kaze::GamepadBtn array
        for (int i = 0; i < s_sdlToGamepadButton.size(); ++i)
            s_sdlToGamepadButton[i] = -1;
        for (Uint8 i = 0; const auto btn : s_gamepadButtonToSDL)
        {
            s_sdlToGamepadButton[btn] = i++;
        }

        // Initialize SDL gamepad axis to kaze::GamepadAxis array
        for (int i = 0; i < s_sdlToGamepadAxis.size(); ++i)
            s_sdlToGamepadAxis[i] = -1;
        for (Uint8 i = 0; const auto axis : s_gamepadAxisToSDL)
        {
            s_sdlToGamepadAxis[axis] = i++;
        }

        // Initialize SDL mouse buttons
        for (int i = 0; i < s_toMouseButton.size(); ++i)
            s_toMouseButton[i] = (MouseBtn)-1;
        for (Uint8 i = 0; const auto button : s_toSDLMouseButton)
        {
            s_toMouseButton[button] = (MouseBtn)i++;
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
            KAZE_PUSH_ERR(Error::BE_RuntimeErr, "Failed to initialize SDL3: {}", SDL_GetError());
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
                KAZE_PUSH_ERR(Error::BE_RuntimeErr, "Failed to get clipboard text: {}", SDL_GetError());
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

    static auto utf8ToUint32(const char *bytes) -> Uint
    {
        uint32_t codepoint = 0;

        if ((bytes[0] & 0x80) == 0) { // 1-byte sequence (ASCII)
            codepoint = bytes[0];
        } else if ((bytes[0] & 0xE0) == 0xC0) { // 2-byte sequence
            codepoint = ((bytes[0] & 0x1F) << 6) |
                        (bytes[1] & 0x3F);
        } else if ((bytes[0] & 0xF0) == 0xE0) { // 3-byte sequence
            codepoint = ((bytes[0] & 0x0F) << 12) |
                        ((bytes[1] & 0x3F) << 6) |
                        (bytes[2] & 0x3F);
        } else if ((bytes[0] & 0xF8) == 0xF0) { // 4-byte sequence
            codepoint = ((bytes[0] & 0x07) << 18) |
                        ((bytes[1] & 0x3F) << 12) |
                        ((bytes[2] & 0x3F) << 6) |
                        (bytes[3] & 0x3F);
        } else {
            throw std::runtime_error("Invalid UTF-8 encoding");
        }

        return codepoint;
    }

    static auto getContentScale(WindowHandle window) noexcept -> float
    {
        return SDL_GetDisplayContentScale(SDL_GetDisplayForWindow(WIN_CAST(window)));
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
                    const auto key = backend::toKey(e.key.scancode);
                    if (key != Key::Unknown)
                    {
                        const auto window = SDL_GetWindowFromID(e.wheel.windowID);
                        events.emit(KeyboardEvent {
                            .key      = key,
                            .isDown   = e.key.down,
                            .isRepeat = e.key.repeat,
                            .window = window,
                        });
                    }

                } break;

            case SDL_EVENT_MOUSE_WHEEL:
                {
                    const auto window = SDL_GetWindowFromID(e.wheel.windowID);
#if KAZE_PLATFORM_EMSCRIPTEN
                    auto offset = (e.wheel.direction == SDL_MOUSEWHEEL_FLIPPED) ?
                        Vec2f{ e.wheel.x * 0.01f,  e.wheel.y } :
                        Vec2f{ e.wheel.x * -0.01f, -e.wheel.y };
#else
                    auto offset = (e.wheel.direction == SDL_MOUSEWHEEL_FLIPPED) ?
                        Vec2f{ -e.wheel.x, -e.wheel.y } :
                        Vec2f{ e.wheel.x, e.wheel.y };
#endif
                    events.emit(MouseScrollEvent {
                        .offset = offset,
                        .window = window,
                    });
                } break;

            case SDL_EVENT_MOUSE_BUTTON_UP:
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                {
                    const auto button = backend::toMouseBtn(e.button.button);
                    if (button != MouseBtn::Unknown)
                    {
                        const auto window = SDL_GetWindowFromID(e.button.windowID);
                        events.emit(MouseButtonEvent {
                            .button = button,
                            .isDown = e.button.down,
                            .window = window
                        });
                    }
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

                    SDL_SetCursor(static_cast<SDL_Cursor *>(data->cursor.handle));
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
            case SDL_EVENT_TEXT_INPUT:
                {
                    const auto window = SDL_GetWindowFromID(e.text.windowID);
                    events.emit(TextInputEvent {
                        .codepoint = utf8ToUint32(e.text.text),
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
                    const auto button = backend::toGamepadBtn(static_cast<SDL_GamepadButton>(e.gbutton.button));
                    if (button == GamepadBtn::Unknown) break;

                    const auto gamepad = SDL_GetGamepadFromID(e.gbutton.which);
                    if ( !gamepad ) break;

                    auto props = SDL_GetGamepadProperties(gamepad);
                    if ( !props ) break;

                    auto data = static_cast<GamepadData *>(SDL_GetPointerProperty(props, GamepadMgr::DataKey, nullptr));
                    if ( !data ) break;

                    if (data->controllerIndex >= 0)
                    {
                        const auto event = GamepadButtonEvent {
                            .controllerIndex = data->controllerIndex,
                            .type = e.gbutton.down ? GamepadButtonEvent::Down : GamepadButtonEvent::Up,
                            .button = button,
                        };
                        backend::gamepads.processEvent(event);
                        backend::events.emit(event);
                    }
                } break;

            case SDL_EVENT_GAMEPAD_AXIS_MOTION:
                {
                    const auto axis = backend::toGamepadAxis( static_cast<SDL_GamepadAxis>(e.gaxis.axis) );
                    if (axis == GamepadAxis::Unknown) break;

                    auto gamepad = SDL_GetGamepadFromID(e.gaxis.which);
                    if ( !gamepad ) break;

                    auto props = SDL_GetGamepadProperties(gamepad);
                    if ( !props ) break;

                    auto data = static_cast<GamepadData *>(SDL_GetPointerProperty(props, GamepadMgr::DataKey, nullptr));
                    if ( !data ) break;

                    if (data->controllerIndex >= 0)
                    {
                        const auto event = GamepadAxisEvent {
                            .controllerIndex = data->controllerIndex,
                            .axis = axis,
                            .value = e.gaxis.value > 0 ?
                                (float)e.gaxis.value / (float)std::numeric_limits<decltype(e.gaxis.value)>::max() :
                                (float)e.gaxis.value / -(float)std::numeric_limits<decltype(e.gaxis.value)>::min(),
                        };
                        gamepads.processEvent(event);
                        events.emit(event);
                    }
                } break;

            case SDL_EVENT_MOUSE_MOTION:
                {
                    const auto window = SDL_GetWindowFromID(e.motion.windowID);
                    const auto scale = getContentScale(window);
                    events.emit(MouseMotionEvent {
                        .position = {
                            e.motion.x / scale,
                            e.motion.y / scale,
                        },
                        .relative = {
                            e.motion.xrel / scale,
                            e.motion.yrel / scale,
                        },
                        .window = window,
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

    static Array<SDL_SystemCursor, static_cast<Uint>(CursorType::Count)> s_toSDLSysCursor = {
        SDL_SYSTEM_CURSOR_DEFAULT,
        SDL_SYSTEM_CURSOR_TEXT,
        SDL_SYSTEM_CURSOR_CROSSHAIR,
        SDL_SYSTEM_CURSOR_MOVE,
        SDL_SYSTEM_CURSOR_NS_RESIZE,
        SDL_SYSTEM_CURSOR_EW_RESIZE,
        SDL_SYSTEM_CURSOR_NWSE_RESIZE,
        SDL_SYSTEM_CURSOR_NESW_RESIZE,
        SDL_SYSTEM_CURSOR_POINTER,
        SDL_SYSTEM_CURSOR_NOT_ALLOWED
    };

    auto cursor::createStandard(CursorType type, CursorHandle *outCursor) noexcept -> bool
    {
        if ( !outCursor)
        {
            KAZE_PUSH_ERR(Error::NullArgErr, "Required argument `outCursor` was null");
            return false;
        }

        if (type >= CursorType::Count || type < CursorType::Arrow)
        {
            KAZE_PUSH_ERR(Error::InvalidEnum, "CursorType was out of range");
            return false;
        }

        const auto sdlCursorType = s_toSDLSysCursor[static_cast<Uint>(type)];
        const auto cursor = SDL_CreateSystemCursor(sdlCursorType);
        if ( !cursor )
        {
            KAZE_PUSH_ERR(Error::BE_RuntimeErr, "Failed to create SDL System Cursor: {}", SDL_GetError());
            return false;
        }

        *outCursor = {.handle = cursor };
        return true;
    }

    auto cursor::createCustom(const ImageContainer &image, int anchorX, int anchorY, CursorHandle *outCursor) noexcept -> bool
    {
        if ( !outCursor )
        {
            KAZE_PUSH_ERR(Error::NullArgErr, "Required arg `outCursor` was null");
            return false;
        }

        const auto sdlSurf = SDL_CreateSurface(image.width, image.height, SDL_PIXELFORMAT_RGBA8888);
        if ( !sdlSurf )
        {
            KAZE_PUSH_ERR(Error::BE_RuntimeErr, "Failed to create SDL_Surface: {}", SDL_GetError());
            return false;
        }

        if ( !SDL_LockSurface(sdlSurf) )
        {
            KAZE_PUSH_ERR(Error::BE_RuntimeErr, "Failed to lock SDL_Surface: {}", SDL_GetError());
            SDL_DestroySurface(sdlSurf);
            return false;
        }

        memory::copy(sdlSurf->pixels, image.data, image.size);
        SDL_UnlockSurface(sdlSurf);
        SDL_DestroySurface(sdlSurf);

        if (const auto cursor = SDL_CreateColorCursor(sdlSurf, anchorX, anchorY))
        {
            *outCursor = { .handle = cursor };
            return true;
        }

        return false;
    }

    auto cursor::getCursor(WindowHandle window, CursorHandle *outCursor) noexcept -> bool
    {
        RETURN_IF_NULL(outCursor);

        WindowData *data;
        if ( !getWindowData(window, &data) )
            return false;

        *outCursor = data->cursor;
        return true;
    }

    auto cursor::setCursor(WindowHandle window, CursorHandle cursor) noexcept -> bool
    {
        RETURN_IF_NULL(window);

        WindowData *data;
        if ( !getWindowData(window, &data) )
            return false;

        data->cursor = cursor;
        if (data->isHovered)
        {
            if ( !SDL_SetCursor(static_cast<SDL_Cursor *>(cursor.handle)) )
            {
                KAZE_PUSH_ERR(Error::BE_RuntimeErr, "Failed to set cursor: {}", SDL_GetError());
                return false;
            }
        }

        return true;
    }

    auto cursor::destroy(CursorHandle cursor) noexcept -> bool
    {
        SDL_DestroyCursor(static_cast<SDL_Cursor *>(cursor.handle));
        return true;
    }

    /// Mouse and keyboard functions are here since there's not many of them to justify their own separate file...

    auto keyboard::isDown(Key key, bool *outDown) noexcept -> bool
    {
        RETURN_IF_NULL(outDown);

        const auto keys = SDL_GetKeyboardState(nullptr);
        if ( !keys )
        {
            KAZE_PUSH_ERR(Error::BE_RuntimeErr, "Failed to get keyboard state: {}", SDL_GetError());
            return false;
        }

        const auto sdlKey = backend::toSDLKey(key);

        *outDown = keys[sdlKey];
        return true;
    }

    auto cursor::getRelativePosition(const WindowHandle window, float *x, float *y) noexcept -> bool
    {
        RETURN_IF_NULL(window);

        float tempX, tempY;
        SDL_GetGlobalMouseState(&tempX, &tempY);

        const auto scale = getContentScale(window);
        tempX /= scale;
        tempY /= scale;

        int winX, winY;
        if ( !SDL_GetWindowPosition( WIN_CAST(window), &winX, &winY ) )
        {
            KAZE_PUSH_ERR(Error::BE_RuntimeErr, "Failed to get window position: {}", SDL_GetError());
            return false;
        }

        if (x)
            *x = tempX - winX;
        if (y)
            *y = tempY - winY;

        return true;
    }

    auto cursor::getGlobalPosition(float *x, float *y) noexcept -> bool
    {
        float tempX, tempY;
        SDL_GetGlobalMouseState(&tempX, &tempY);

        SDL_Point point(tempX, tempY);
        if (auto display = SDL_GetDisplayForPoint(&point))
        {
            const auto scale = SDL_GetDisplayContentScale(0);
            tempX /= scale;
            tempY /= scale;
        }

        *x = tempX;
        *y = tempY;

        return true;
    }

    auto cursor::isDown(const WindowHandle window, kaze::MouseBtn button, bool *outDown) noexcept -> bool
    {
        RETURN_IF_NULL(window);
        RETURN_IF_NULL(outDown);

        *outDown = static_cast<bool>( SDL_GetMouseState(nullptr, nullptr) & static_cast<uint32_t>(button) );
        return true;
    }
}

KAZE_NS_END
