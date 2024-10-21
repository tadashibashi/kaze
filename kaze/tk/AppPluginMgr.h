#pragma once
#ifndef kaze_tk_apppluginmgr_h_
#define kaze_tk_apppluginmgr_h_

#include <kaze/tk/lib.h>
#include <kaze/tk/AppPlugin.h>
#include <kaze/core/Action.h>

KAZE_TK_NAMESPACE_BEGIN
class App;

class AppPluginMgr {
public:
    AppPluginMgr() = default;
    ~AppPluginMgr();

    auto addPlugin(const AppPlugin &plugin) -> AppPluginMgr &;
    auto removePlugin(StringView name) -> Bool;

    auto clear() -> void;
    [[nodiscard]]
    auto empty() const noexcept -> Bool;
    [[nodiscard]]
    auto size() const noexcept -> Size;

    [[nodiscard]]
    auto contains(StringView name) const noexcept -> Bool;

    /// Attempt to get plugin data from the manager
    /// \param[in]  name        name of the plugin to find
    /// \param[out] outPlugin   retrieves pointer to the found plugin if this function returns `true`
    /// \returns `true` - if plugin with `name` was found and `outPlugin` was populated;
    ///          `false` - plugin was not found and `outPlugin` is not mutated.
    auto tryGet(StringView name, const AppPlugin **outPlugin) const -> Bool;

    Action<App *> init{};
    Action<App *> preFrame{};
    Action<App *> preUpdate{};
    Action<App *> postUpdate{};
    Action<App *> preRender{};
    Action<App *> postRender{};
    Action<App *> preRenderUI{};
    Action<App *> postRenderUI{};
    Action<App *> postFrame{};
    Action<App *> close{};

    Action<const WindowEvent &, App *> windowEvent{};
    Action<const GamepadAxisEvent &, App *> gamepadAxisEvent{};
    Action<const GamepadButtonEvent &, App *> gamepadButtonEvent{};
    Action<const GamepadConnectEvent &, App *> gamepadConnectEvent{};
    Action<const KeyboardEvent &, App *> keyboardEvent{};
    Action<const MouseButtonEvent &, App *> mouseButtonEvent {};
    Action<const MouseMotionEvent &, App *> mouseMotionEvent {};
    Action<const MouseScrollEvent &, App *> mouseScrollEvent {};
private:
    auto removePluginCallbacks(const AppPlugin &plugin) -> void;
    auto addPluginCallbacks(const AppPlugin &plugin) -> void;
    Map<String, AppPlugin> m_plugins{};
};


KAZE_TK_NAMESPACE_END

#endif // kaze_tk_apppluginmgr_h_
