#pragma once
#include <kaze/core/ConditionalAction.h>
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

    ConditionalAction<const GamepadAxisEvent &, Double, App *> gpadAxisFilter{};
    ConditionalAction<const GamepadButtonEvent &, Double, App *> gpadButtonFilter{};
    ConditionalAction<const GamepadConnectEvent &, Double, App *> gpadConnectFilter{};
    ConditionalAction<const KeyboardEvent &, Double, App *> keyFilter{};
    ConditionalAction<const MouseButtonEvent &, Double, App *> mbuttonFilter{};
    ConditionalAction<const MouseMotionEvent &, Double, App *> mmotionFilter{};
    ConditionalAction<const MouseScrollEvent &, Double, App *> mscrollFilter{};
    ConditionalAction<const TextInputEvent &, Double, App *> textInputFilter{};
    ConditionalAction<const WindowEvent &, Double, App *> windowFilter{};

    Action<const GamepadAxisEvent &, Double, App *> gpadAxisEvent{};
    Action<const GamepadButtonEvent &, Double, App *> gpadButtonEvent{};
    Action<const GamepadConnectEvent &, Double, App *> gpadConnectEvent{};
    Action<const KeyboardEvent &, Double, App *> keyEvent{};
    Action<const MouseButtonEvent &, Double, App *> mbuttonEvent{};
    Action<const MouseMotionEvent &, Double, App *> mmotionEvent{};
    Action<const MouseScrollEvent &, Double, App *> mscrollEvent{};
    Action<const TextInputEvent &, Double, App *> textInputEvent{};
    Action<const WindowEvent &, Double, App *> windowEvent{};
private:
    auto removePluginCallbacks(const AppPlugin &plugin) -> void;
    auto addPluginCallbacks(const AppPlugin &plugin) -> void;
    Map<String, AppPlugin> m_plugins{};
};


KAZE_TK_NAMESPACE_END
