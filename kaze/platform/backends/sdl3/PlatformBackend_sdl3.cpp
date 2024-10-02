#include "SDL3/SDL_error.h"
#include <kaze/platform/backends/sdl3/Gamepad_sdl3.h>
#include <kaze/platform/PlatformBackend.h>
#include <kaze/platform/PlatformEvent.h>
#include <kaze/debug.h>

#include <SDL3/SDL.h>
#include <limits>

#ifdef SDL_PLATFORM_IOS
#include <SDL3/SDL_metal.h>
#endif

#include "GamepadConstants.h"
#include "GamepadConstants.inl"
#include "KeyboardConstants.inl"

KAZE_NAMESPACE_BEGIN

namespace backend {

#define WIN_CAST(window) static_cast<SDL_Window *>(window)

#define RETURN_IF_NULL(obj) do { if ( !(obj) ) { \
    KAZE_CORE_ERRCODE(Error::NullArgErr, "required argument `{}` was null", #obj); \
    return false; \
} } while(0)

    static sdl3::GamepadMgr s_gamepads{};

    struct WindowData
    {
        bool isHovered{false};
        bool cursorVisibleMode{false};
    };

    GamepadBtn sdlToGamepadButton(const Uint8 sdlButton)
    {
        return static_cast<GamepadBtn>(s_sdlToGamepadButton[sdlButton]);
    }

    SDL_GamepadButton gamepadButtonToSdl(const GamepadBtn button)
    {
        return static_cast<SDL_GamepadButton>(s_gamepadButtonToSDL[ static_cast<Uint8>(button) ]);
    }

    static bool getWindowData(const WindowHandle window, WindowData **outData)
    {
        KAZE_ASSERT(window);
        KAZE_ASSERT(outData);

        const auto props = SDL_GetWindowProperties(WIN_CAST(window));
        if (!props)
        {
            KAZE_CORE_ERRCODE(Error::BE_RuntimeErr,
                "Failed to get window properties: {}", SDL_GetError());
            return false;
        }

        const auto data = static_cast<WindowData *>(
            SDL_GetPointerProperty(props, "WindowData", nullptr));
        if ( !data )
        {
            KAZE_CORE_ERRCODE(Error::BE_LogicError,
                "Missing WindowData on SDL_Window");
            return false;
        }

        *outData = data;
        return true;
    }

    static bool sdlEventFilter(void *userdata, SDL_Event *event)
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

    NativePlatformData window::getNativeInfo(WindowHandle window) noexcept
    {
        NativePlatformData result{};
        auto props = SDL_GetWindowProperties(WIN_CAST(window));
        if (props == 0)
        {
            KAZE_CORE_ERRCODE(Error::BE_RuntimeErr, "Failed to retrieve SDL_Window properties: {}", SDL_GetError());
            return {};
        }


    #if defined(SDL_PLATFORM_MACOS)
        result.windowHandle = SDL_GetPointerProperty(props, SDL_PROP_WINDOW_COCOA_WINDOW_POINTER, nullptr);
        result.displayType = nullptr;
    #elif defined(SDL_PLATFORM_IOS)
        const auto metalView = SDL_Metal_CreateView(WIN_CAST(window));
        if (!metalView)
        {
            KAZE_CORE_ERR("Failed to create metal view": {}, SDL_GetError());
            return {};
        }

        auto metalLayer = SDL_Metal_GetLayer(metalView);
        if (!metalLayer)
        {
            KAZE_CORE_ERR("Failed to get Metal layer from SDL_MetalView: {}", SDL_GetError());
            return {};
        }
        result.windowHandle = metalLayer;
        result.displayType = nullptr;
    #elif defined(SDL_PLATFORM_WIN32)
        result.windowHandle = SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr);
        result.displayType = SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WIN32_HDC_POINTER, nullptr);
    #elif defined(SDL_PLATFORM_LINUX)
        if (SDL_strcmp(SDL_GetCurrentVideoDriver(), "x11") == 0) {            // X11
            result.windowHandle = (void *)(uintptr_t)SDL_GetNumberProperty(props, SDL_PROP_WINDOW_X11_WINDOW_NUMBER, 0);
            result.displayType = SDL_GetPointerProperty(props, SDL_PROP_WINDOW_X11_DISPLAY_POINTER, nullptr);
        } else if (SDL_strcmp(SDL_GetCurrentVideoDriver(), "wayland") == 0) { // Wayland
            result.windowHandle = SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WAYLAND_SURFACE_POINTER, nullptr);
            result.displayType = SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WAYLAND_DISPLAY_POINTER, nullptr);
        }
    #elif defined(SDL_PLATFORM_ANDROID)
        result.windowHandle = SDL_GetPointerProperty(props, SDL_PROP_WINDOW_ANDROID_WINDOW_POINTER, nullptr);
        result.displayType = result.windowHandle;
    #else
        result.windowHandle = nullptr;
        result.displayType = nullptr;
    #endif
        return result;
    }

    auto init() noexcept -> bool
    {
        if ( !SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD) )
        {
            KAZE_CORE_ERRCODE(Error::BE_RuntimeErr, "Failed to initialize SDL3: {}", SDL_GetError());
            return KAZE_FALSE;
        }

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
        s_gamepads.preProcessEvents();

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
                        .key      = static_cast<Key>(s_sdlKeyToKey[e.key.scancode]),
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
                    const auto index = s_gamepads.connect(e.gdevice.which);
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
                    const auto index = s_gamepads.disconnect(e.gdevice.which);
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
                            auto data = static_cast<sdl3::GamepadData *>(SDL_GetPointerProperty(props, sdl3::GamepadMgr::DataKey, nullptr));
                            if (data)
                            {
                                if (data->controllerIndex >= 0)
                                {
                                    const auto event = GamepadButtonEvent {
                                        .controllerIndex = data->controllerIndex,
                                        .type = e.gbutton.down ? GamepadButtonEvent::Down : GamepadButtonEvent::Up,
                                        .button = static_cast<GamepadBtn>(s_sdlToGamepadButton[e.gbutton.button]),
                                    };
                                    s_gamepads.processEvent(event);
                                    events.emit(event);
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
                            auto data = static_cast<sdl3::GamepadData *>(SDL_GetPointerProperty(props, sdl3::GamepadMgr::DataKey, nullptr));
                            if (data)
                            {
                                if (data->controllerIndex >= 0)
                                {
                                    const auto event = GamepadAxisEvent {
                                        .controllerIndex = data->controllerIndex,
                                        .axis = static_cast<GamepadAxis>(s_sdlToGamepadAxis[e.gaxis.axis]),
                                        .value = e.gaxis.value > 0 ?
                                            (float)e.gaxis.value / (float)std::numeric_limits<decltype(e.gaxis.value)>::max() :
                                            (float)e.gaxis.value / -(float)std::numeric_limits<decltype(e.gaxis.value)>::min(),
                                    };
                                    s_gamepads.processEvent(event);
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

        s_gamepads.postProcessEvents();
        return KAZE_TRUE;
    }

    static void sdlWindowDataCleanUp(void *userptr, void *value) noexcept
    {
        delete static_cast<WindowData *>(value);
    }

    auto window::open(const char *title, const int width, const int height, const WindowInit::Flags flags,
        WindowHandle *outWindow) noexcept -> bool
    {
        Uint sdl3Flags = SDL_WINDOW_HIDDEN;
    #ifdef KAZE_TARGET_APPLE
        sdl3Flags |= SDL_WINDOW_METAL;
    #endif
        if (flags & WindowInit::Resizable)
            sdl3Flags |= SDL_WINDOW_RESIZABLE;
        if (flags & WindowInit::Borderless)
            sdl3Flags |= SDL_WINDOW_BORDERLESS;
        if (flags & WindowInit::Fullscreen)
            sdl3Flags |= SDL_WINDOW_FULLSCREEN;
        if (flags & WindowInit::Floating)
            sdl3Flags |= SDL_WINDOW_ALWAYS_ON_TOP;
        if (flags & WindowInit::Maximized)
            sdl3Flags |= SDL_WINDOW_MAXIMIZED;
        if (flags & WindowInit::Transparent)
            sdl3Flags |= SDL_WINDOW_TRANSPARENT;

        const auto window = SDL_CreateWindow(title, width, height, sdl3Flags);
        if (!window)
        {
            KAZE_CORE_ERRCODE(Error::BE_RuntimeErr, "Failed to create SDL3 Window: {}", SDL_GetError());
            return KAZE_FALSE;
        }

        const auto props = SDL_GetWindowProperties(window);
        if (!props)
        {
            KAZE_CORE_ERRCODE(Error::BE_RuntimeErr, "Failed to get window properties: {}", SDL_GetError());
            SDL_DestroyWindow(window);
            return KAZE_FALSE;
        }

        const auto windowData = new WindowData();
        if (!SDL_SetPointerPropertyWithCleanup(props, "WindowData", windowData, sdlWindowDataCleanUp, nullptr))
        {
            KAZE_CORE_ERR("Failed to set WindowData to SDL_Window properties: {}", SDL_GetError());
            SDL_DestroyWindow(window);
            delete windowData;
            return KAZE_FALSE;
        }

        // Check if mouse is inside the window
        {
            float mouseX = 0, mouseY = 0;
            SDL_GetGlobalMouseState(&mouseX, &mouseY);

            int winX = 0, winY = 0;
            if ( !SDL_GetWindowPosition(window, &winX, &winY) )
            {
                KAZE_CORE_WARN("Failed to get window position: {}", SDL_GetError());
            }

            int winW = 0, winH = 0;
            if ( !SDL_GetWindowSize(window, &winW, &winH) )
            {
                KAZE_CORE_WARN("Failed to get window size: {}", SDL_GetError());
            }

            if (mouseX >= winX && mouseX < winX + winW &&
                mouseY >= winY && mouseY < winY + winH)
            {
                windowData->isHovered = true;
            }
        }

        if ( !(flags & WindowInit::Hidden) ) // keep window hidden if hidden flag was set
        {
            if (!SDL_ShowWindow(window)) // show it otherwise
            {
                KAZE_CORE_WARN("SDL_ShowWindow failed: {}", SDL_GetError());
            }
        }

        *outWindow = window;
        return KAZE_TRUE;
    }

    auto window::close(const WindowHandle window) noexcept -> bool
    {
        RETURN_IF_NULL(window);

        int windowCount;
        auto windows = SDL_GetWindows(&windowCount);
        if ( !windows )
        {

            return false;
        }

        SDL_DestroyWindow(static_cast<SDL_Window *>(window));
        return true;
    }

    auto window::isOpen(const WindowHandle window, bool *outOpen) noexcept -> bool
    {
        RETURN_IF_NULL(window);
        RETURN_IF_NULL(outOpen);

        int windowCount;
        auto windows = SDL_GetWindows(&windowCount);
        if ( !windows )
        {
            KAZE_CORE_ERRCODE(Error::BE_RuntimeErr, "Failed to get windows: {}", SDL_GetError());
            return false;
        }

        for (int i = 0; i < windowCount; ++i)
        {
            if (windows[i] == window)
            {
                *outOpen = true;
                return true;
            }
        }

        *outOpen = false;
        return true;
    }

    auto window::setUserData(const WindowHandle window, void *data) noexcept -> bool
    {
        RETURN_IF_NULL(window);

        const auto props = SDL_GetWindowProperties( WIN_CAST(window) );
        if ( !props )
        {
            KAZE_CORE_ERRCODE(Error::BE_RuntimeErr, "Failed to get window properties: {}", SDL_GetError());
            return false;
        }

        if ( !SDL_SetPointerProperty(props, "userptr", data) )
        {
            KAZE_CORE_ERRCODE(Error::BE_RuntimeErr, "Failed to set userptr pointer property: {}", SDL_GetError());
            return false;
        }

        return true;
    }

    auto window::getUserData(const WindowHandle window, void **outData) noexcept -> bool
    {
        RETURN_IF_NULL(window);
        RETURN_IF_NULL(outData);

        const auto props = SDL_GetWindowProperties( WIN_CAST(window) );
        if ( !props )
        {
            KAZE_CORE_ERRCODE(Error::BE_RuntimeErr, "Failed to get window properties: {}", SDL_GetError());
            return false;
        }

        *outData = SDL_GetPointerProperty(props, "userptr", nullptr);
        return true;
    }

    auto window::setTitle(const WindowHandle window, const char *title) noexcept -> bool
    {
        RETURN_IF_NULL(window);
        RETURN_IF_NULL(title);

        if ( !SDL_SetWindowTitle( WIN_CAST(window), title ) )
        {
            KAZE_CORE_ERRCODE(Error::BE_RuntimeErr, "Failed to set window title: {}", SDL_GetError());
            return false;
        }

        return true;
    }

    auto window::getTitle(const WindowHandle window, const char **outTitle) noexcept -> bool
    {
        RETURN_IF_NULL(window);
        RETURN_IF_NULL(outTitle);

        const auto title = SDL_GetWindowTitle(WIN_CAST(window));
        if ( !title )
        {
            KAZE_CORE_ERRCODE(Error::BE_RuntimeErr, "Failed to get window title: {}", SDL_GetError());
            return false;
        }

        *outTitle = title;
        return true;
    }

    auto window::setSize(const WindowHandle window, const int x, const int y) noexcept -> bool
    {
        RETURN_IF_NULL(window);

        if ( !SDL_SetWindowSize( WIN_CAST(window), x, y ) )
        {
            KAZE_CORE_ERRCODE(Error::BE_RuntimeErr, "failed to set logical window size: {}", SDL_GetError());
            return false;
        }

        return true;
    }

    auto window::getSize(const WindowHandle window, int *x, int *y) noexcept -> bool
    {
        RETURN_IF_NULL(window);

        if ( !SDL_GetWindowSize(WIN_CAST(window), x, y) )
        {
            KAZE_CORE_ERRCODE(Error::BE_RuntimeErr, "Failed to get window size: {}", SDL_GetError());
            return false;
        }

        return true;
    }

    auto window::getFramebufferSize(const WindowHandle window, int *x, int *y) noexcept -> bool
    {
        RETURN_IF_NULL(window);

        if ( !SDL_GetWindowSizeInPixels(WIN_CAST(window), x, y) )
        {
            KAZE_CORE_ERRCODE(Error::BE_RuntimeErr, "Failed to get window display size: {}", SDL_GetError());
            return false;
        }

        return true;
    }

    auto window::isFullscreen(const WindowHandle window, bool *outFullscreen) noexcept -> bool
    {
        RETURN_IF_NULL(window);
        RETURN_IF_NULL(outFullscreen);

        *outFullscreen = static_cast<bool>(SDL_GetWindowFlags(WIN_CAST(window)) & SDL_WINDOW_FULLSCREEN);
        return true;
    }

    auto window::setFullscreen(WindowHandle window, bool value) noexcept -> bool
    {
        RETURN_IF_NULL(window);

        if ( !SDL_SetWindowFullscreen(WIN_CAST(window), value) )
        {
            KAZE_CORE_ERRCODE(Error::BE_RuntimeErr, "Failed to set fullscreen to {}", value);
            return false;
        }

        return true;
    }

    auto window::getFullscreenMode(const WindowHandle window, FullscreenMode *mode) noexcept -> bool
    {
        RETURN_IF_NULL(window);
        RETURN_IF_NULL(mode);

        *mode = SDL_GetWindowFullscreenMode(WIN_CAST(window)) == nullptr ?
            FullscreenMode::Desktop : FullscreenMode::Native;
        return true;
    }

    static auto setNativeFullscreenMode(const WindowHandle window) noexcept -> bool
    {
        KAZE_ASSERT(window);

        const auto display = SDL_GetPrimaryDisplay();
        if (!display)
        {
            KAZE_CORE_ERR("Failed to get primary display: {}", SDL_GetError());
            return false;
        }

        SDL_Rect displayBounds;
        if (!SDL_GetDisplayBounds(display, &displayBounds))
        {
            KAZE_CORE_ERR("Failed to get display bounds: {}", SDL_GetError());
            return false;
        }

        SDL_DisplayMode displayMode;
        if (!SDL_GetClosestFullscreenDisplayMode(display, displayBounds.w, displayBounds.y, 0, true, &displayMode))
        {
            KAZE_CORE_ERR("Failed to get fullscreen display mode: {}", SDL_GetError());
            return false;
        }

        if (!SDL_SetWindowFullscreenMode(WIN_CAST(window), &displayMode))
        {
            KAZE_CORE_ERR("Failed to set fullscreen mode: {}", SDL_GetError());
            return false;
        }

        return true;
    }

    static auto setDesktopFullscreenMode(const WindowHandle window) noexcept -> bool
    {
        KAZE_ASSERT(window);

        if ( !SDL_SetWindowFullscreenMode(WIN_CAST(window), nullptr) )
        {
            KAZE_CORE_ERR("Failed to set window fullscreen mode to desktop: {}", SDL_GetError());
            return false;
        }

        return true;
    }

    auto window::setFullscreenMode(WindowHandle window, FullscreenMode mode) noexcept -> bool
    {
        if (mode == FullscreenMode::Desktop)
        {
            if ( !setDesktopFullscreenMode(window) )
                return false;
        }
        else if (mode == FullscreenMode::Native)
        {
            if ( !setNativeFullscreenMode(window) )
                return false;
        }
        else
        {
            KAZE_CORE_ERR("Unknown FullscreenMode type");
            return false;
        }

        return true;
    }

    bool windowIsDesktopFullscreen(const WindowHandle window) noexcept
    {
        return static_cast<bool>(SDL_GetWindowFlags(WIN_CAST(window)) & SDL_WINDOW_FULLSCREEN) &&
            SDL_GetWindowFullscreenMode(WIN_CAST(window)) == nullptr;
    }

    static bool setDesktopFullscreen(const WindowHandle window, const bool value)
    {
        const auto lastMode = SDL_GetWindowFullscreenMode(WIN_CAST(window));
        if ( !SDL_SetWindowFullscreenMode(WIN_CAST(window), nullptr) )
        {
            KAZE_CORE_ERR("Failed to set SDL_Window's desktop fullscreen mode: {}", SDL_GetError());
            return false;
        }

        if (!SDL_SetWindowFullscreen( WIN_CAST(window), value) )
        {
            KAZE_CORE_ERR("Failed to set SDL_Window fullscreen: {}", SDL_GetError());
            SDL_SetWindowFullscreenMode(WIN_CAST(window), lastMode); // revert to last mode
            return false;
        }

        return true;
    }

    auto window::isBordered(const WindowHandle window, bool *outBordered) noexcept -> bool
    {
        RETURN_IF_NULL(window);
        RETURN_IF_NULL(outBordered);

        *outBordered = !static_cast<bool>( SDL_GetWindowFlags(WIN_CAST(window)) & SDL_WINDOW_BORDERLESS );
        return true;
    }

    auto window::setBordered(const WindowHandle window, const bool value) noexcept -> bool
    {
        if ( !SDL_SetWindowBordered(WIN_CAST(window), value) )
        {
            KAZE_CORE_ERR("Failed to set window borderless: {}", SDL_GetError());
            return false;
        }

        return true;
    }

    auto window::isMinimized(const WindowHandle window, bool *outMinimized) noexcept -> bool
    {
        RETURN_IF_NULL(window);
        RETURN_IF_NULL(outMinimized);

        *outMinimized = static_cast<bool>(
            SDL_GetWindowFlags(WIN_CAST(window)) & SDL_WINDOW_MINIMIZED);
        return true;
    }

    auto window::minimize(const WindowHandle window) noexcept -> bool
    {
        RETURN_IF_NULL(window);

        if ( !SDL_MinimizeWindow( WIN_CAST(window) ) )
        {
            KAZE_CORE_ERR("Failed to minimize window: {}", SDL_GetError());
            return false;
        }

        return true;
    }

    auto window::isMaximized(const WindowHandle window, bool *outMaximized) noexcept -> bool
    {
        RETURN_IF_NULL(window);
        RETURN_IF_NULL(outMaximized);

        *outMaximized = static_cast<bool>(
            SDL_GetWindowFlags(WIN_CAST(window)) & SDL_WINDOW_MAXIMIZED);
        return true;
    }

    auto window::maximize(const WindowHandle window) noexcept -> bool
    {
        RETURN_IF_NULL(window);

        if ( !SDL_MaximizeWindow( WIN_CAST(window) ) )
        {
            KAZE_CORE_ERR("Failed to maximize window: {}", SDL_GetError());
            return false;
        }

        return true;
    }

    auto window::restore(const WindowHandle window) noexcept -> bool
    {
        RETURN_IF_NULL(window);

        if ( !SDL_RestoreWindow( WIN_CAST(window) ) )
        {
            KAZE_CORE_ERR("Failed to restore window: {}", SDL_GetError());
            return false;
        }

        return true;
    }

    auto window::getPosition(const WindowHandle window, int *x, int *y) noexcept -> bool
    {
        RETURN_IF_NULL(window);

        if ( !SDL_GetWindowPosition( WIN_CAST(window), x, y ) )
        {
            KAZE_CORE_ERR("Failed to get window position: {}", SDL_GetError());
            return false;
        }

        return true;
    }

    auto window::setPosition(const WindowHandle window, const int x, const int y) noexcept -> bool
    {
        RETURN_IF_NULL(window);

        if ( !SDL_SetWindowPosition( WIN_CAST(window), x, y ) )
        {
            KAZE_CORE_ERR("Failed to set window position to {{{}, {}}}: {}", x, y, SDL_GetError());
            return false;
        }

        return true;
    }

    auto window::getResizable(const WindowHandle window, bool *outResizable) noexcept -> bool
    {
        RETURN_IF_NULL(window);
        RETURN_IF_NULL(outResizable);

        *outResizable = static_cast<bool>(
            SDL_GetWindowFlags(WIN_CAST(window)) & SDL_WINDOW_RESIZABLE);
        return true;
    }

    auto window::setResizable(const WindowHandle window, const bool value) noexcept -> bool
    {
        if ( !SDL_SetWindowResizable(WIN_CAST(window), value) )
        {
            KAZE_CORE_ERR("Failed to set window resizable attribute to {}: {}",
                value, SDL_GetError());
            return false;
        }

        return true;
    }

    auto window::isHidden(const WindowHandle window, bool *outHidden) noexcept -> bool
    {
        RETURN_IF_NULL(window);
        RETURN_IF_NULL(outHidden);

        *outHidden = static_cast<bool>(
            SDL_GetWindowFlags(WIN_CAST(window)) & SDL_WINDOW_HIDDEN);
        return true;
    }

    auto window::setHidden(const WindowHandle window, const bool value) noexcept -> bool
    {
        RETURN_IF_NULL(window);

        bool result;
        if (value)
        {
            result = SDL_HideWindow( WIN_CAST(window) );
        }
        else
        {
            result = SDL_ShowWindow( WIN_CAST(window) );
        }

        if ( !result )
        {
            KAZE_CORE_ERRCODE(Error::BE_RuntimeErr, "Failed to set window to {}: {}",
                value ? "hidden" : "shown", SDL_GetError());
            return false;
        }

        return true;
    }

    auto window::isHovered(const WindowHandle window, bool *outHovered) noexcept -> bool
    {
        RETURN_IF_NULL(window);
        RETURN_IF_NULL(outHovered);

        WindowData *data;
        if ( !getWindowData(window, &data) )
            return false;

        *outHovered = data->isHovered;
        return true;
    }

    auto window::isFloating(const WindowHandle window, bool *outFloating) noexcept -> bool
    {
        RETURN_IF_NULL(window);
        RETURN_IF_NULL(outFloating);

        *outFloating = static_cast<bool>(
            SDL_GetWindowFlags( WIN_CAST(window)) & SDL_WINDOW_ALWAYS_ON_TOP );
        return true;
    }

    auto window::setFloating(const WindowHandle window, const bool value) noexcept -> bool
    {
        if ( !SDL_SetWindowAlwaysOnTop( WIN_CAST(window), value ) )
        {
            KAZE_CORE_ERR("Failed to set window floating attribute to {}: {}", value, SDL_GetError());
            return false;
        }

        return true;
    }

    auto window::isTransparent(const WindowHandle window, bool *outTransparent) noexcept -> bool
    {
        RETURN_IF_NULL(window);
        RETURN_IF_NULL(outTransparent);

        *outTransparent = static_cast<bool>(
            SDL_GetWindowFlags(WIN_CAST(window)) & SDL_WINDOW_TRANSPARENT);
        return true;
    }

    auto window::setTransparent(const WindowHandle window, const bool value) noexcept -> bool
    {
        RETURN_IF_NULL(window);

        if ( !SDL_SetWindowOpacity( WIN_CAST(window), (value ? 1.0f : 0) ) )
        {
            KAZE_CORE_ERR("Failed to set window transparency to {}: {}", value, SDL_GetError());
            return false;
        }

        return true;
    }

    auto window::isFocused(const WindowHandle window, bool *outFocused) noexcept -> bool
    {
        RETURN_IF_NULL(window);
        RETURN_IF_NULL(outFocused);

        *outFocused = static_cast<bool>(
            SDL_GetWindowFlags(WIN_CAST(window)) & SDL_WINDOW_INPUT_FOCUS);
        return true;
    }

    auto window::focus(const WindowHandle window) noexcept -> bool
    {
        bool curHidden;
        if ( !window::isHidden(window, &curHidden) )
            return false;

        if ( !curHidden )
        {
            if ( !window::setHidden(window, false) )
                return false;
        }

        if ( !SDL_RaiseWindow( WIN_CAST(window) ) )
        {
            KAZE_CORE_WARN("Failed to raise window: {}", SDL_GetError());
            return false;
        }

        if ( !SDL_SetWindowKeyboardGrab(WIN_CAST(window), true) )
        {
            KAZE_CORE_WARN("Failed to set window keyboard grab: {}", SDL_GetError());
            return false;
        }

        return true;
    }

    auto window::setMinSize(const WindowHandle window, const int minWidth, const int minHeight) noexcept -> bool
    {
        RETURN_IF_NULL(window);

        if ( !SDL_SetWindowMinimumSize( WIN_CAST(window), minWidth, minHeight ) )
        {
            KAZE_CORE_ERR("Failed to set window minimum size to {{{}, {}}}: {}",
                minWidth, minHeight, SDL_GetError());
            return false;
        }

        return true;
    }

    auto window::setMaxSize(const WindowHandle window, const int maxWidth, const int maxHeight) noexcept -> bool
    {
        RETURN_IF_NULL(window);

        if ( !SDL_SetWindowMaximumSize(WIN_CAST(window), maxWidth, maxHeight) )
        {
            KAZE_CORE_ERR("Failed to set window maximum size to {{{}, {}}}: {}",
                maxWidth, maxHeight, SDL_GetError());
            return false;
        }

        return true;
    }

    auto window::getMinSize(const WindowHandle window, int *minWidth, int *minHeight) noexcept -> bool
    {
        RETURN_IF_NULL(window);

        if ( !SDL_GetWindowMinimumSize( WIN_CAST(window), minWidth, minHeight ) )
        {
            KAZE_CORE_ERR("Failed to get window minimum size: {}", SDL_GetError());
            return false;
        }

        return true;
    }

    auto window::getMaxSize(const WindowHandle window, int *maxWidth, int *maxHeight) noexcept -> bool
    {
        if ( !SDL_GetWindowMaximumSize( WIN_CAST(window), maxWidth, maxHeight) )
        {
            KAZE_CORE_ERRCODE(Error::BE_RuntimeErr, "Failed to get window maximum size: {}", SDL_GetError());
            return false;
        }

        return true;
    }

    auto window::setShowCursorMode(const WindowHandle window, bool value) noexcept -> bool
    {
        RETURN_IF_NULL(window);

        WindowData *data;
        if ( !getWindowData(window, &data) )
            return false;

        data->cursorVisibleMode = value;
        return true;
    }

    auto window::getShowCursorMode(const WindowHandle window, bool *outMode) noexcept -> bool
    {
        RETURN_IF_NULL(window);

        WindowData *data;
        if ( !getWindowData(window, &data) )
            return false;

        *outMode = data->cursorVisibleMode;
        return true;
    }

    auto window::setCaptureCursorMode(const WindowHandle window, bool value) noexcept -> bool
    {
        RETURN_IF_NULL(window);

        if ( !SDL_SetWindowRelativeMouseMode( WIN_CAST(window), value ) )
        {
            KAZE_CORE_ERR("Failed to set window relative mouse mode: {}", SDL_GetError());
            return false;
        }

        return true;
    }

    auto window::getCaptureCursorMode(const WindowHandle window, bool *outValue) noexcept -> bool
    {
        RETURN_IF_NULL(window);
        RETURN_IF_NULL(outValue);

        if ( !SDL_GetWindowRelativeMouseMode( WIN_CAST(window) ) )
        {
            KAZE_CORE_ERR("Failed to get window relative mouse mode: {}", SDL_GetError());
            return false;
        }

        return true;
    }

    auto keyboard::isDown(Key key, bool *outDown) noexcept -> bool
    {
        RETURN_IF_NULL(outDown);

        const auto keys = SDL_GetKeyboardState(nullptr);
        if ( !keys )
        {
            KAZE_CORE_ERRCODE(Error::BE_RuntimeErr, "Failed to get keyboard state: {}", SDL_GetError());
            return false;
        }

        const auto sdlKey =  s_keyToSdlKey[ static_cast<int>(key) ];

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

#define GP_IN_RANGE(index) \
    do { if ( !((index) >= 0 && (index) < MaxGamepadSlots) )  { \
        KAZE_CORE_ERRCODE(Error::OutOfRange, "gamepad index {} is out of range", (index)); \
        return false; \
    } } while(0)

    /// TODO: Implement Gamepad functions!
    auto gamepad::isConnected(int index, bool *outConnected) noexcept -> bool
    {
        GP_IN_RANGE(index);
        return static_cast<bool>(s_gamepads[index]);
    }

    auto gamepad::isDown(int index, GamepadBtn button, bool *outDown) noexcept -> bool
    {
        GP_IN_RANGE(index);
        RETURN_IF_NULL(outDown);

        const auto data = s_gamepads[index];
        if (!data)
        {
            *outDown = false;
            return true;
        }

        *outDown = data->buttons[ static_cast<Uint8>(button) ].isDown[ data->currentIndex ];
        return true;
    }

    auto gamepad::isJustDown(int index, GamepadBtn button, bool *outJustDown) noexcept -> bool
    {
        GP_IN_RANGE(index);
        RETURN_IF_NULL(outJustDown);

        const auto data = s_gamepads[index];
        if (!data)
        {
            *outJustDown = false;
            return true;
        }
        const auto &buttonData = data->buttons[ static_cast<Uint8>(button) ];

        return buttonData.isDown[ data->currentIndex ] && !buttonData.isDown[ !data->currentIndex ];
    }

    auto gamepad::isJustUp(int index, GamepadBtn button, bool *outJustUp) noexcept -> bool
    {
        GP_IN_RANGE(index);
        RETURN_IF_NULL(outJustUp);

        const auto data = s_gamepads[index];
        if (!data)
        {
            *outJustUp = false;
            return true;
        }

        const auto &buttonData = data->buttons[ static_cast<Uint8>(button) ];

        *outJustUp = !buttonData.isDown[ data->currentIndex ] && buttonData.isDown[ !data->currentIndex ];
        return true;
    }

    auto gamepad::getAxis(int index, GamepadAxis axis, float *outValue) noexcept -> bool
    {
        GP_IN_RANGE(index);
        RETURN_IF_NULL(outValue);

        const auto data = s_gamepads[index];
        if (!data)
        {
            *outValue = 0;
            return true;
        }

        const auto &axisData = data->axes[ static_cast<Uint8>(axis) ];
        return axisData.value[ data->currentIndex ];
        return true;
    }

    auto gamepad::getAxisMoved(int index, GamepadAxis axis, float deadzone, bool *outMoved) noexcept -> bool
    {
        GP_IN_RANGE(index);
        RETURN_IF_NULL(outMoved);

        const auto data = s_gamepads[index];
        if (!data)
        {
            *outMoved = false;
            return true;
        }

        const auto &axisData = data->axes[ static_cast<Uint8>(axis) ];
        auto curValue = axisData.value[ data->currentIndex ];
        auto lastValue = axisData.value[ !data->currentIndex ];

        if (curValue <= deadzone)
            curValue = 0;
        if (lastValue <= deadzone)
            lastValue = 0;
        *outMoved = curValue != lastValue;
        return true;
    }

    auto gamepad::getAxesMoved(int index, GamepadAxis axisX, GamepadAxis axisY, float deadzone, bool *outMoved) noexcept -> bool
    {
        GP_IN_RANGE(index);
        RETURN_IF_NULL(outMoved);

        const auto data = s_gamepads[index];
        if (!data)
        {
            *outMoved = false;
            return true;
        }

        const auto &axisDataX = data->axes[ static_cast<Uint8>(axisX) ];
        const auto &axisDataY = data->axes[ static_cast<Uint8>(axisY) ];

        auto curValueX = axisDataX.value[ data->currentIndex ];
        auto lastValueX = axisDataX.value[ !data->currentIndex ];
        auto curValueY = axisDataY.value[ data->currentIndex ];
        auto lastValueY = axisDataY.value[ !data->currentIndex ];

        if (mathf::distance(0.f, 0.f, curValueX, curValueY) <= deadzone)
        {
            curValueX = 0;
            curValueY = 0;
        }

        if (mathf::distance(0.f, 0.f, lastValueX, lastValueY) <= deadzone)
        {
            lastValueX = 0;
            lastValueY = 0;
        }

        *outMoved = curValueX != lastValueX || curValueY != lastValueY;
        return true;
    }
}

KAZE_NAMESPACE_END
