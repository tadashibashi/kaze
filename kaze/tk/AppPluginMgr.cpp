#include "AppPluginMgr.h"

KAZE_NS_BEGIN

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

    if (cb.gpadAxisFilter)
        gpadAxisFilter.remove(cb.gpadAxisFilter, cb.userptr);
    if (cb.gpadButtonFilter)
        gpadButtonFilter.remove(cb.gpadButtonFilter, cb.userptr);
    if (cb.gpadConnectFilter)
        gpadConnectFilter.remove(cb.gpadConnectFilter, cb.userptr);
    if (cb.keyFilter)
        keyFilter.remove(cb.keyFilter, cb.userptr);
    if (cb.mbuttonFilter)
        mbuttonFilter.remove(cb.mbuttonFilter, cb.userptr);
    if (cb.mmotionFilter)
        mmotionFilter.remove(cb.mmotionFilter, cb.userptr);
    if (cb.mscrollFilter)
        mscrollFilter.remove(cb.mscrollFilter, cb.userptr);
    if (cb.textInputFilter)
        textInputFilter.remove(cb.textInputFilter, cb.userptr);
    if (cb.windowFilter)
        windowFilter.remove(cb.windowFilter, cb.userptr);

    if (cb.gpadAxisEvent)
        gpadAxisEvent.remove(cb.gpadAxisEvent, cb.userptr);
    if (cb.gpadButtonEvent)
        gpadButtonEvent.remove(cb.gpadButtonEvent, cb.userptr);
    if (cb.gpadConnectEvent)
        gpadConnectEvent.remove(cb.gpadConnectEvent, cb.userptr);
    if (cb.keyEvent)
        keyEvent.remove(cb.keyEvent, cb.userptr);
    if (cb.mbuttonEvent)
        mbuttonEvent.remove(cb.mbuttonEvent, cb.userptr);
    if (cb.mmotionEvent)
        mmotionEvent.remove(cb.mmotionEvent, cb.userptr);
    if (cb.mscrollEvent)
        mscrollEvent.remove(cb.mscrollEvent, cb.userptr);
    if (cb.textInputEvent)
        textInputEvent.remove(cb.textInputEvent, cb.userptr);
    if (cb.windowEvent)
        windowEvent.remove(cb.windowEvent, cb.userptr);
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

    if (cb.gpadAxisFilter)
        gpadAxisFilter.add(cb.gpadAxisFilter, cb.userptr);
    if (cb.gpadButtonFilter)
        gpadButtonFilter.add(cb.gpadButtonFilter, cb.userptr);
    if (cb.gpadConnectFilter)
        gpadConnectFilter.add(cb.gpadConnectFilter, cb.userptr);
    if (cb.keyFilter)
        keyFilter.add(cb.keyFilter, cb.userptr);
    if (cb.mbuttonFilter)
        mbuttonFilter.add(cb.mbuttonFilter, cb.userptr);
    if (cb.mmotionFilter)
        mmotionFilter.add(cb.mmotionFilter, cb.userptr);
    if (cb.mscrollFilter)
        mscrollFilter.add(cb.mscrollFilter, cb.userptr);
    if (cb.textInputFilter)
        textInputFilter.add(cb.textInputFilter, cb.userptr);
    if (cb.windowFilter)
        windowFilter.add(cb.windowFilter, cb.userptr);

    if (cb.gpadAxisEvent)
        gpadAxisEvent.add(cb.gpadAxisEvent, cb.userptr);
    if (cb.gpadButtonEvent)
        gpadButtonEvent.add(cb.gpadButtonEvent, cb.userptr);
    if (cb.gpadConnectEvent)
        gpadConnectEvent.add(cb.gpadConnectEvent, cb.userptr);
    if (cb.keyEvent)
        keyEvent.add(cb.keyEvent, cb.userptr);
    if (cb.mbuttonEvent)
        mbuttonEvent.add(cb.mbuttonEvent, cb.userptr);
    if (cb.mmotionEvent)
        mmotionEvent.add(cb.mmotionEvent, cb.userptr);
    if (cb.mscrollEvent)
        mscrollEvent.add(cb.mscrollEvent, cb.userptr);
    if (cb.textInputEvent)
        textInputEvent.add(cb.textInputEvent, cb.userptr);
    if (cb.windowEvent)
        windowEvent.add(cb.windowEvent, cb.userptr);
}

KAZE_NS_END
