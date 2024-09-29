#include <kaze/platform/backends/sdl3/Gamepad_sdl3.h>
#include <kaze/platform/PlatformBackend.h>
#include <kaze/platform/PlatformEvent.h>
#include <kaze/debug.h>

#include <SDL3/SDL.h>
#include <limits>

#ifdef SDL_PLATFORM_IOS
#include <SDL3/SDL_metal.h>
#endif

#include "GamepadConstants.inl"
#include "KeyboardConstants.inl"

KAZE_NAMESPACE_BEGIN

namespace backend {

    #define WIN_CAST(window) static_cast<SDL_Window *>(window)

    static sdl3::GamepadMgr s_gamepads{};

    struct WindowData
    {
        bool isHovered{false};
    };

    static WindowData *getWindowData(void *window)
    {
        const auto props = SDL_GetWindowProperties(WIN_CAST(window));
        if (!props)
        {
            KAZE_CORE_ERR("Failed to get window properties: {}", SDL_GetError());
            return nullptr;
        }

        return static_cast<WindowData *>(
            SDL_GetPointerProperty(props, "WindowData", nullptr));
    }

    static bool sdlEventFilter(void *userdata, SDL_Event *event)
    {
        auto &e = *event;
        switch (e.type)
        {
        case SDL_EVENT_WINDOW_RESIZED:
            {
                PlatformBackend::events.emit(WindowEvent {
                    .type = WindowEvent::Resized,
                    .data0 = event->window.data1,
                    .data1 = event->window.data2,
                    .window = SDL_GetWindowFromID(e.window.windowID),
                });
            } return false;

        case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
            {
                PlatformBackend::events.emit(WindowEvent {
                    .type = WindowEvent::ResizedFramebuffer,
                    .data0 = event->window.data1,
                    .data1 = event->window.data2,
                    .window = SDL_GetWindowFromID(e.window.windowID),
                });
            } return false;
        case SDL_EVENT_WINDOW_MOVED:
            {
                const auto window = SDL_GetWindowFromID(e.window.windowID);
                PlatformBackend::events.emit(WindowEvent {
                    .type = WindowEvent::Moved,
                    .data0 = e.window.data1,
                    .data1 = e.window.data2,
                    .window = window,
                });
            } break;
        default:
            return true;
        }
    }

    NativePlatformData PlatformBackend::windowGetNativeInfo(void *window)
    {
        NativePlatformData result{};
        auto props = SDL_GetWindowProperties(WIN_CAST(window));
        if (props == 0)
        {
            KAZE_CORE_ERR("Failed to retrieve SDL_Window properties: {}", SDL_GetError());
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

    bool PlatformBackend::init() noexcept
    {
        if ( !SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD) )
        {
            KAZE_CORE_ERR("Failed to initialize SDL3: {}", SDL_GetError());
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

    void PlatformBackend::shutdown() noexcept
    {
        SDL_Quit();
    }

    double PlatformBackend::getTime() noexcept
    {
        return static_cast<Double>(SDL_GetTicksNS()) * 1e-9;
    }

    void PlatformBackend::pollEvents()
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

                    const auto data = getWindowData(window);
                    KAZE_ASSERT(data);
                    data->isHovered = true;
                } break;

            case SDL_EVENT_WINDOW_MOUSE_LEAVE:
                {
                    const auto window = SDL_GetWindowFromID(e.window.windowID);
                    events.emit(WindowEvent {
                        .type = WindowEvent::MouseExited,
                        .window = window
                    });

                    const auto data = getWindowData(window);
                    KAZE_ASSERT(data);
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
                            .id = index,
                            .type = GamepadConnectEvent::Connected,
                        });
                    }
                } break;

            case SDL_EVENT_GAMEPAD_REMOVED:
                {
                    const auto index = s_gamepads.disconnect(e.gdevice.which);
                    if (index >= 0)
                    {
                        events.emit(GamepadConnectEvent {
                            .id = index,
                            .type = GamepadConnectEvent::Disconnected,
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
                        .position = { e.drop.x, e.drop.y },
                        .window = window,
                    });
                } break;
            default:
                break;
            }
        }

        s_gamepads.postProcessEvents();
    }

    static void sdlWindowDataCleanUp(void *userptr, void *value)
    {
        delete static_cast<WindowData *>(value);
    }

    void *PlatformBackend::windowCreate(const Cstring title, const Size width, const Size height, const WindowInit::Flags initFlags)
    {
        Uint sdlFlags = SDL_WINDOW_HIDDEN;
    #ifdef KAZE_TARGET_APPLE
        sdlFlags |= SDL_WINDOW_METAL;
    #endif
        if (initFlags & WindowInit::Resizable)
            sdlFlags |= SDL_WINDOW_RESIZABLE;
        if (initFlags & WindowInit::Borderless)
            sdlFlags |= SDL_WINDOW_BORDERLESS;
        if (initFlags & WindowInit::Fullscreen)
            sdlFlags |= SDL_WINDOW_FULLSCREEN;
        if (initFlags & WindowInit::Floating)
            sdlFlags |= SDL_WINDOW_ALWAYS_ON_TOP;
        if (initFlags & WindowInit::Maximized)
            sdlFlags |= SDL_WINDOW_MAXIMIZED;
        if (initFlags & WindowInit::Transparent)
            sdlFlags |= SDL_WINDOW_TRANSPARENT;

        const auto window = SDL_CreateWindow(title, static_cast<Int>(width), static_cast<Int>(height), sdlFlags);
        if (!window)
        {
            KAZE_CORE_ERR("Failed to create SDL3 Window: {}", SDL_GetError());
            return nullptr;
        }

        const auto props = SDL_GetWindowProperties(window);
        if (!props)
        {
            KAZE_CORE_ERR("Failed to get window properties: {}", SDL_GetError());
            SDL_DestroyWindow(window);
            return nullptr;
        }

        auto windowData = new WindowData();
        if (!SDL_SetPointerPropertyWithCleanup(props, "WindowData", windowData, sdlWindowDataCleanUp, nullptr))
        {
            KAZE_CORE_ERR("Failed to set WindowData to SDL_Window properties: {}", SDL_GetError());
            SDL_DestroyWindow(window);
            delete windowData;
            return nullptr;
        }

        // Check if mouse is inside the window
        {
            float mouseX = 0, mouseY = 0;
            SDL_GetGlobalMouseState(&mouseX, &mouseY);

            int winX = 0, winY = 0;
            if (!SDL_GetWindowPosition(window, &winX, &winY))
            {
                KAZE_CORE_WARN("Failed to get window position: {}", SDL_GetError());
            }

            int winW = 0, winH = 0;
            if (!SDL_GetWindowSize(window, &winW, &winH))
            {
                KAZE_CORE_WARN("Failed to get window size: {}", SDL_GetError());
            }

            if (mouseX >= winX && mouseX < winX + winW &&
                mouseY >= winY && mouseY < winY + winH)
            {
                windowData->isHovered = true;
            }
        }

        if ( !(initFlags & WindowInit::Hidden) ) // keep window hidden if hidden flag was set
        {
            if (!SDL_ShowWindow(window)) // show it otherwise
            {
                KAZE_CORE_WARN("SDL_ShowWindow failed: {}", SDL_GetError());
            }
        }

        return window;
    }

    void PlatformBackend::windowDestroy(void *window)
    {
        SDL_DestroyWindow(static_cast<SDL_Window *>(window));
    }

    bool PlatformBackend::windowIsOpen(void *window)
    {
        return SDL_GetWindowID(static_cast<SDL_Window *>(window)) != 0;
    }

    void PlatformBackend::windowSetUserData(void *window, void *data)
    {
        const auto props = SDL_GetWindowProperties(static_cast<SDL_Window *>(window));
        SDL_SetPointerProperty(props, "userptr", data);
    }

    void *PlatformBackend::windowGetUserData(void *window)
    {
        const auto props = SDL_GetWindowProperties(static_cast<SDL_Window *>(window));
        return SDL_GetPointerProperty(props, "userptr", nullptr);
    }

    void PlatformBackend::windowSetTitle(void *window, const char *title)
    {
        SDL_SetWindowTitle(WIN_CAST(window), title);
    }

    const char *PlatformBackend::windowGetTitle(void *window) noexcept
    {
        return SDL_GetWindowTitle(WIN_CAST(window));
    }

    void PlatformBackend::windowSetSize(void *window, const int x, const int y)
    {
        SDL_SetWindowSize(WIN_CAST(window), x, y);
    }

    void PlatformBackend::windowGetSize(void *window, int *x, int *y) noexcept
    {
        SDL_GetWindowSize(WIN_CAST(window), x, y);
    }

    void PlatformBackend::windowGetDisplaySize(void *window, int *x, int *y) noexcept
    {
        SDL_GetWindowSizeInPixels(WIN_CAST(window), x, y);
    }

    bool PlatformBackend::windowIsNativeFullscreen(void *window) noexcept
    {
        return static_cast<bool>(SDL_GetWindowFlags(WIN_CAST(window)) & SDL_WINDOW_FULLSCREEN) &&
            SDL_GetWindowFullscreenMode(WIN_CAST(window)) != nullptr;
    }

    void PlatformBackend::windowSetNativeFullscreen(void *window, bool value)
    {
        const auto display = SDL_GetPrimaryDisplay();
        if (!display)
        {
            KAZE_CORE_ERR("Failed to get primary display: {}", SDL_GetError());
            return;
        }

        SDL_Rect displayBounds;
        if (!SDL_GetDisplayBounds(display, &displayBounds))
        {
            KAZE_CORE_ERR("Failed to get display bounds: {}", SDL_GetError());
            return;
        }

        SDL_DisplayMode displayMode;
        if (!SDL_GetClosestFullscreenDisplayMode(display, displayBounds.w, displayBounds.y, 0, true, &displayMode))
        {
            KAZE_CORE_ERR("Failed to get fullscreen display mode: {}", SDL_GetError());
            return;
        }

        if (!SDL_SetWindowFullscreenMode(WIN_CAST(window), &displayMode))
        {
            KAZE_CORE_ERR("Failed to set fullscreen mode: {}", SDL_GetError());
            return;
        }

        if (!SDL_SetWindowFullscreen(WIN_CAST(window), value))
        {
            KAZE_CORE_ERR("Failed to set fullscreen mode to {}: ", value, SDL_GetError());
        }
    }

    bool PlatformBackend::windowIsDesktopFullscreen(void *window) noexcept
    {
        return static_cast<bool>(SDL_GetWindowFlags(WIN_CAST(window)) & SDL_WINDOW_FULLSCREEN) &&
            SDL_GetWindowFullscreenMode(WIN_CAST(window)) == nullptr;
    }

    void PlatformBackend::windowSetDesktopFullscreen(void *window, const bool value)
    {
        if (!SDL_SetWindowFullscreenMode(WIN_CAST(window), nullptr))
        {
            KAZE_CORE_ERR("Failed to set SDL_Window's desktop fullscreen mode: {}", SDL_GetError());
            return;
        }

        if (!SDL_SetWindowFullscreen(WIN_CAST(window), value))
        {
            KAZE_CORE_ERR("Failed to set SDL_Window fullscreen: {}", SDL_GetError());
        }
    }

    bool PlatformBackend::windowIsBorderless(void *window) noexcept
    {
        return static_cast<bool>(SDL_GetWindowFlags(WIN_CAST(window)) & SDL_WINDOW_BORDERLESS);
    }

    void PlatformBackend::windowSetBorderless(void *window, const bool value)
    {
        if (!SDL_SetWindowBordered(WIN_CAST(window), !value))
        {
            KAZE_CORE_ERR("Failed to set window borderless: {}", SDL_GetError());
        }
    }

    bool PlatformBackend::windowIsMinimized(void *window) noexcept
    {
        return static_cast<bool>(
            SDL_GetWindowFlags(WIN_CAST(window)) & SDL_WINDOW_MINIMIZED);
    }

    void PlatformBackend::windowMinimize(void *window)
    {
        if (!SDL_MinimizeWindow(WIN_CAST(window)))
        {
            KAZE_CORE_ERR("Failed to minimize window: {}", SDL_GetError());
        }
    }

    bool PlatformBackend::windowIsMaximized(void *window) noexcept
    {
        return static_cast<bool>(
            SDL_GetWindowFlags(WIN_CAST(window)) & SDL_WINDOW_MAXIMIZED);
    }

    void PlatformBackend::windowMaximize(void *window)
    {
        if (!SDL_MaximizeWindow(WIN_CAST(window)))
        {
            KAZE_CORE_ERR("Failed to maximize window: {}", SDL_GetError());
        }
    }

    void PlatformBackend::windowRestore(void *window)
    {
        if (!SDL_RestoreWindow(WIN_CAST(window)))
        {
            KAZE_CORE_ERR("Failed to restore window: {}", SDL_GetError());
        }
    }

    void PlatformBackend::windowGetPosition(void *window, int *x, int *y) noexcept
    {
        if (!SDL_GetWindowPosition(WIN_CAST(window), x, y))
        {
            KAZE_CORE_ERR("Failed to get window position: {}", SDL_GetError());
        }
    }

    void PlatformBackend::windowSetPosition(void *window, const int x, const int y) noexcept
    {
        if (!SDL_SetWindowPosition(WIN_CAST(window), x, y))
        {
            KAZE_CORE_ERR("Failed to set window position to {{{}, {}}}: {}", x, y, SDL_GetError());
        }
    }

    bool PlatformBackend::windowGetResizable(void *window) noexcept
    {
        return static_cast<bool>(
            SDL_GetWindowFlags(WIN_CAST(window)) & SDL_WINDOW_RESIZABLE);
    }

    void PlatformBackend::windowSetResizable(void *window, const bool value)
    {
        if (!SDL_SetWindowResizable(WIN_CAST(window), value))
        {
            KAZE_CORE_ERR("Failed to set window resizable attribute to {}: {}",
                value, SDL_GetError());
        }
    }

    bool PlatformBackend::windowIsHidden(void *window) noexcept
    {
        return static_cast<bool>(
            SDL_GetWindowFlags(WIN_CAST(window)) & SDL_WINDOW_HIDDEN);
    }

    void PlatformBackend::windowSetHidden(void *window, const bool value)
    {
        bool result;
        if (value)
        {
            result = SDL_HideWindow(WIN_CAST(window));
        }
        else
        {
            result = SDL_ShowWindow(WIN_CAST(window));
        }

        if (!result)
        {
            KAZE_CORE_ERR("Failed to set window to {}: {}",
                value ? "hidden" : "shown", SDL_GetError());
        }
    }

    bool PlatformBackend::windowIsHovered(void *window) noexcept
    {
        const auto data = getWindowData(window);
        KAZE_ASSERT(data);

        return data->isHovered;
    }

    bool PlatformBackend::windowIsFloating(void *window) noexcept
    {
        return static_cast<bool>(
            SDL_GetWindowFlags(WIN_CAST(window)) & SDL_WINDOW_ALWAYS_ON_TOP);
    }

    void PlatformBackend::windowSetFloating(void *window, const bool value)
    {
        if (!SDL_SetWindowAlwaysOnTop(WIN_CAST(window), value))
        {
            KAZE_CORE_ERR("Failed to set window floating attribute to {}: {}", value, SDL_GetError());
        }
    }

    bool PlatformBackend::windowIsTransparent(void *window) noexcept
    {
        return static_cast<bool>(
            SDL_GetWindowFlags(WIN_CAST(window)) & SDL_WINDOW_TRANSPARENT);
    }

    void PlatformBackend::windowSetTransparent(void *window, const bool value)
    {
        if (!SDL_SetWindowOpacity(WIN_CAST(window), value ? 1.0f : 0))
        {
            KAZE_CORE_ERR("Failed to set window transparency to {}: {}", value, SDL_GetError());
        }
    }

    bool PlatformBackend::windowIsFocused(void *window) noexcept
    {
        return static_cast<bool>(
            SDL_GetWindowFlags(WIN_CAST(window)) & SDL_WINDOW_INPUT_FOCUS);
    }

    void PlatformBackend::windowFocus(void *window)
    {
        if (windowIsHidden(window))
        {
            windowSetHidden(window, false);
        }

        if (!SDL_RaiseWindow(WIN_CAST(window)))
        {
            KAZE_CORE_WARN("Failed to raise window: {}", SDL_GetError());
        }

        if (!SDL_SetWindowKeyboardGrab(WIN_CAST(window), true))
        {
            KAZE_CORE_WARN("Failed to set window keyboard grab: {}", SDL_GetError());
        }
    }

    void PlatformBackend::windowSetMinimumSize(void *window, const int minWidth, const int minHeight)
    {
        if (!SDL_SetWindowMinimumSize(WIN_CAST(window), minWidth, minHeight))
        {
            KAZE_CORE_ERR("Failed to set window minimum size to {{{}, {}}}: {}",
                minWidth, minHeight, SDL_GetError());
        }
    }

    void PlatformBackend::windowSetMaximumSize(void *window, const int maxWidth, const int maxHeight)
    {
        if (!SDL_SetWindowMaximumSize(WIN_CAST(window), maxWidth, maxHeight))
        {
            KAZE_CORE_ERR("Failed to set window maximum size to {{{}, {}}}: {}",
                maxWidth, maxHeight, SDL_GetError());
        }
    }

    void PlatformBackend::windowGetMinimumSize(void *window, int *minWidth, int *minHeight)
    {
        if (!SDL_GetWindowMinimumSize(WIN_CAST(window), minWidth, minHeight))
        {
            KAZE_CORE_ERR("Failed to get window minimum size: {}", SDL_GetError());
        }
    }

    void PlatformBackend::windowGetMaximumSize(void *window, int *maxWidth, int *maxHeight)
    {
        if (!SDL_GetWindowMaximumSize(WIN_CAST(window), maxWidth, maxHeight))
        {
            KAZE_CORE_ERR("Failed to get window maximum size: {}", SDL_GetError());
        }
    }

    /// TODO: Implement Gamepad functions!
    bool PlatformBackend::gamepadIsConnected(int index) noexcept
    {
        return static_cast<bool>(s_gamepads[index]);
    }

    bool PlatformBackend::gamepadIsButtonDown(int index, GamepadBtn button)
    {
        const auto data = s_gamepads[index];
        if (!data)
            return false;
        return data->buttons[ static_cast<Uint8>(button) ].isDown[ data->currentIndex ];
    }

    bool PlatformBackend::gamepadIsButtonJustDown(int index, GamepadBtn button)
    {
        const auto data = s_gamepads[index];
        if (!data)
            return false;
        const auto &buttonData = data->buttons[ static_cast<Uint8>(button) ];

        return buttonData.isDown[ data->currentIndex ] && !buttonData.isDown[ !data->currentIndex ];
    }

    bool PlatformBackend::gamepadIsButtonJustUp(int index, GamepadBtn button)
    {
        const auto data = s_gamepads[index];
        if (!data)
            return false;
        const auto &buttonData = data->buttons[ static_cast<Uint8>(button) ];

        return !buttonData.isDown[ data->currentIndex ] && buttonData.isDown[ !data->currentIndex ];
    }

    float PlatformBackend::gamepadGetAxis(int index, GamepadAxis axis)
    {
        const auto data = s_gamepads[index];
        if (!data)
            return 0;
        const auto &axisData = data->axes[ static_cast<Uint8>(axis) ];

        return axisData.value[ data->currentIndex ];
    }

    bool PlatformBackend::gamepadDidAxisMove(int index, GamepadAxis axis, float deadzone)
    {
        const auto data = s_gamepads[index];
        if (!data)
            return 0;
        const auto &axisData = data->axes[ static_cast<Uint8>(axis) ];

        auto curValue = axisData.value[ data->currentIndex ];
        auto lastValue = axisData.value[ !data->currentIndex ];

        if (curValue <= deadzone)
            curValue = 0;
        if (lastValue <= deadzone)
            lastValue = 0;
        return curValue != lastValue;
    }

    bool PlatformBackend::gamepadDidAxesMove(int index, GamepadAxis axisX, GamepadAxis axisY, float deadzone)
    {
        const auto data = s_gamepads[index];
        if (!data)
            return 0;
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

        return curValueX != lastValueX || curValueY != lastValueY;
    }

    const char * PlatformBackend::getClipboard()
    {
        static String text;

        const auto cstr = SDL_GetClipboardText();
        text.assign(cstr);

        SDL_free(cstr);

        return text.c_str();
    }

    void PlatformBackend::setClipboard(const char *text)
    {
        if (!SDL_SetClipboardText(text))
        {
            KAZE_CORE_ERR("Failed to set clipboard text to \"{}\": {}",
                (text ? text : ""), SDL_GetError());
        }
    }
}

KAZE_NAMESPACE_END