#pragma once
#include <kaze/core/input/InputEvents.h>
#include <kaze/core/traits.h>

#include <kaze/tk/lib.h>

KAZE_TK_NAMESPACE_BEGIN

class App;

class AppPlugin
{
public:
    struct Callbacks
    {
        void *userptr{};                                            ///< associated user data context passed into each callback

        // ----- App events -----
        funcptr_t<void (App *app, void *userptr)> init{};           ///< occurs after the app has successfully initialized; called immediately if already initialized: initialize resources here
        funcptr_t<void (App *app, void *userptr)> preFrame{};       ///< occurs at the start of the app frame
        funcptr_t<void (App *app, void *userptr)> preUpdate{};      ///< occurs after preFrame, and before the app updates
        funcptr_t<void (App *app, void *userptr)> postUpdate{};     ///< occurs after app update
        funcptr_t<void (App *app, void *userptr)> preRender{};      ///< occurs after postUpdate, and before the app renders graphics
        funcptr_t<void (App *app, void *userptr)> postRender{};     ///< occurs after the app renders graphics
        funcptr_t<void (App *app, void *userptr)> preRenderUI{};    ///< occurs after postRender, and before the app renders UI
        funcptr_t<void (App *app, void *userptr)> postRenderUI{};   ///< occurs after the app renders UI
        funcptr_t<void (App *app, void *userptr)> postFrame{};      ///< occurs at the end of the app frame
        funcptr_t<void (App *app, void *userptr)> close{};          ///< occurs right before the app closes or disposes of this plugin: clean up resources here, e.g. userptr.

        // ----- Input and System events -----
        funcptr_t<void (const GamepadAxisEvent &e, App *app, void *userptr)>    gamepadAxisEvent{};
        funcptr_t<void (const GamepadButtonEvent &e, App *app, void *userptr)>  gamepadButtonEvent{};
        funcptr_t<void (const GamepadConnectEvent &e, App *app, void *userptr)> gamepadConnectEvent{};
        funcptr_t<void (const KeyboardEvent &e, App *app, void *userptr)>       keyboardEvent{};
        funcptr_t<void (const MouseButtonEvent &e, App *app, void *userptr)>    mouseButtonEvent{};
        funcptr_t<void (const MouseMotionEvent &e, App *app, void *userptr)>    mouseMotionEvent{};
        funcptr_t<void (const MouseScrollEvent &e, App *app, void *userptr)>    mouseScrollEvent{};
        funcptr_t<void (const TextInputEvent &e, App *app, void *userptr)>      textInputEvent{};
        funcptr_t<void (const WindowEvent &e, App *app, void *userptr)>         windowEvent{};
    };

    AppPlugin(StringView name, const Callbacks &callbacks) noexcept :
        m_name(name), m_callbacks(callbacks) { }

    const auto &callbacks() const noexcept { return m_callbacks; }
    const auto &name() const noexcept { return m_name; }
private:
    Callbacks m_callbacks;
    String m_name;
};


KAZE_TK_NAMESPACE_END
