#pragma once
#include <kaze/internal/core/input/InputEvents.h>
#include <kaze/internal/core/traits.h>

#include <kaze/internal/tk/lib.h>

KTK_NS_BEGIN

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
        // Filtered input events (ending in "Filter") enable the ability to prevent the rest of the app from
        // forwarding the event if `False` is returned. Input state has not yet been processed by the App, so checking
        // App->input() will not yet be updated to reflect the events received in these callbacks.
        funcptr_t<Bool (const GamepadAxisEvent &e, Double, App *app, void *userptr)>    gpadAxisFilter{};
        funcptr_t<Bool (const GamepadButtonEvent &e, Double, App *app, void *userptr)>  gpadButtonFilter{};
        funcptr_t<Bool (const GamepadConnectEvent &e, Double, App *app, void *userptr)> gpadConnectFilter{};
        funcptr_t<Bool (const KeyboardEvent &e, Double, App *app, void *userptr)>       keyFilter{};
        funcptr_t<Bool (const MouseButtonEvent &e, Double, App *app, void *userptr)>    mbuttonFilter{};
        funcptr_t<Bool (const MouseMotionEvent &e, Double, App *app, void *userptr)>    mmotionFilter{};
        funcptr_t<Bool (const MouseScrollEvent &e, Double, App *app, void *userptr)>    mscrollFilter{};
        funcptr_t<Bool (const TextInputEvent &e, Double, App *app, void *userptr)>      textInputFilter{};
        funcptr_t<Bool (const WindowEvent &e, Double, App *app, void *userptr)>         windowFilter{};

        // Regular input events (ending in "Event") occur after the filter stage, and after the application has
        // processed these events.
        funcptr_t<void (const GamepadAxisEvent &e, Double, App *app, void *userptr)>    gpadAxisEvent{};
        funcptr_t<void (const GamepadButtonEvent &e, Double, App *app, void *userptr)>  gpadButtonEvent{};
        funcptr_t<void (const GamepadConnectEvent &e, Double, App *app, void *userptr)> gpadConnectEvent{};
        funcptr_t<void (const KeyboardEvent &e, Double, App *app, void *userptr)>       keyEvent{};
        funcptr_t<void (const MouseButtonEvent &e, Double, App *app, void *userptr)>    mbuttonEvent{};
        funcptr_t<void (const MouseMotionEvent &e, Double, App *app, void *userptr)>    mmotionEvent{};
        funcptr_t<void (const MouseScrollEvent &e, Double, App *app, void *userptr)>    mscrollEvent{};
        funcptr_t<void (const TextInputEvent &e, Double, App *app, void *userptr)>      textInputEvent{};
        funcptr_t<void (const WindowEvent &e, Double, App *app, void *userptr)>         windowEvent{};
    };

    AppPlugin(StringView name, const Callbacks &callbacks) noexcept :
        m_name(name), m_callbacks(callbacks) { }

    const auto &callbacks() const noexcept { return m_callbacks; }
    const auto &name() const noexcept { return m_name; }
private:
    Callbacks m_callbacks;
    String m_name;
};

KTK_NS_END
