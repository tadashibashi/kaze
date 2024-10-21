#include "AppPluginMgr.h"

KAZE_TK_NAMESPACE_BEGIN
AppPluginMgr::~AppPluginMgr()
{
    clear();
}

auto AppPluginMgr::addPlugin(const AppPlugin &plugin) -> AppPluginMgr &
{
    const auto &name = plugin.name();
    if (m_plugins.contains(name))
    {
        removePlugin(name);
    }

    addPluginCallbacks(plugin);

    m_plugins.emplace(name, plugin);
    return *this;
}

auto AppPluginMgr::removePlugin(StringView name) -> Bool
{
    auto it = m_plugins.find(name.data());
    if (it != m_plugins.end())
    {
        removePluginCallbacks(it->second);
        m_plugins.erase(it);
        return KAZE_TRUE;
    }

    return KAZE_FALSE;
}

auto AppPluginMgr::clear() -> void
{
    for (auto &[name, plugin] : m_plugins)
    {
        removePluginCallbacks(plugin);
    }

    m_plugins.clear();
}

auto AppPluginMgr::empty() const noexcept -> Bool
{
    return m_plugins.empty();
}

auto AppPluginMgr::size() const noexcept -> Size
{
    return m_plugins.size();
}

auto AppPluginMgr::contains(const StringView name) const noexcept -> Bool
{
    return m_plugins.contains(name.data());
}

auto AppPluginMgr::tryGet(const StringView name, const AppPlugin **outPlugin) const -> Bool
{
    auto it = m_plugins.find(name.data());
    if (it != m_plugins.end())
    {
        if (outPlugin)
            *outPlugin = &it->second;
        return KAZE_TRUE;
    }

    return KAZE_FALSE;
}

auto AppPluginMgr::removePluginCallbacks(const AppPlugin &plugin) -> void
{
    const auto &cb = plugin.callbacks();
    if (cb.init)
        init.remove(cb.init, cb.userptr);
    if (cb.preFrame)
        preFrame.remove(cb.preFrame, cb.userptr);
    if (cb.preUpdate)
        preUpdate.remove(cb.preUpdate, cb.userptr);
    if (cb.postUpdate)
        postUpdate.remove(cb.postUpdate, cb.userptr);
    if (cb.preRender)
        preRender.remove(cb.preRender, cb.userptr);
    if (cb.postRender)
        postRender.remove(cb.postRender, cb.userptr);
    if (cb.preRenderUI)
        preRenderUI.remove(cb.preRenderUI, cb.userptr);
    if (cb.postRenderUI)
        postRenderUI.remove(cb.postRenderUI, cb.userptr);
    if (cb.close)
        close.remove(cb.close, cb.userptr);

    if (cb.windowEvent)
        windowEvent.remove(cb.windowEvent, cb.userptr);
    if (cb.gamepadAxisEvent)
        gamepadAxisEvent.remove(cb.gamepadAxisEvent, cb.userptr);
    if (cb.gamepadButtonEvent)
        gamepadButtonEvent.remove(cb.gamepadButtonEvent, cb.userptr);
    if (cb.gamepadConnectEvent)
        gamepadConnectEvent.remove(cb.gamepadConnectEvent, cb.userptr);
    if (cb.keyboardEvent)
        keyboardEvent.remove(cb.keyboardEvent, cb.userptr);
    if (cb.mouseButtonEvent)
        mouseButtonEvent.remove(cb.mouseButtonEvent, cb.userptr);
    if (cb.mouseMotionEvent)
        mouseMotionEvent.remove(cb.mouseMotionEvent, cb.userptr);
    if (cb.mouseScrollEvent)
        mouseScrollEvent.remove(cb.mouseScrollEvent, cb.userptr);
}

auto AppPluginMgr::addPluginCallbacks(const AppPlugin &plugin) -> void
{
    const auto &cb = plugin.callbacks();
    if (cb.init)
        init.add(cb.init, cb.userptr);
    if (cb.preFrame)
        preFrame.add(cb.preFrame, cb.userptr);
    if (cb.preUpdate)
        preUpdate.add(cb.preUpdate, cb.userptr);
    if (cb.postUpdate)
        postUpdate.add(cb.postUpdate, cb.userptr);
    if (cb.preRender)
        preRender.add(cb.preRender, cb.userptr);
    if (cb.postRender)
        postRender.add(cb.postRender, cb.userptr);
    if (cb.preRenderUI)
        preRenderUI.add(cb.preRenderUI, cb.userptr);
    if (cb.postRenderUI)
        postRenderUI.add(cb.postRenderUI, cb.userptr);
    if (cb.close)
        close.add(cb.close, cb.userptr);

    if (cb.windowEvent)
        windowEvent.add(cb.windowEvent, cb.userptr);
    if (cb.gamepadAxisEvent)
        gamepadAxisEvent.add(cb.gamepadAxisEvent, cb.userptr);
    if (cb.gamepadButtonEvent)
        gamepadButtonEvent.add(cb.gamepadButtonEvent, cb.userptr);
    if (cb.gamepadConnectEvent)
        gamepadConnectEvent.add(cb.gamepadConnectEvent, cb.userptr);
    if (cb.keyboardEvent)
        keyboardEvent.add(cb.keyboardEvent, cb.userptr);
    if (cb.mouseButtonEvent)
        mouseButtonEvent.add(cb.mouseButtonEvent, cb.userptr);
    if (cb.mouseMotionEvent)
        mouseMotionEvent.add(cb.mouseMotionEvent, cb.userptr);
    if (cb.mouseScrollEvent)
        mouseScrollEvent.add(cb.mouseScrollEvent, cb.userptr);
}

KAZE_TK_NAMESPACE_END
