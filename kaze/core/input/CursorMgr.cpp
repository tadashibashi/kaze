#include "CursorMgr.h"
#include <kaze/core/platform/backend/backend.h>
#include <kaze/core/video/Window.h>

KAZE_NAMESPACE_BEGIN

auto CursorMgr::create(StringView key, const Image &image, Int hotX, Int hotY) -> Bool
{
    String cursorKey(key);
    if (m_customCursors.contains(cursorKey)) // guard against duplicates
    {
        KAZE_CORE_ERRCODE(Error::DuplicateKey, "Key \"{}\" already exists in CursorMgr", key);
        return False;
    }

    CursorHandle handle;
    if ( !backend::cursor::createCustom(image.handle(), hotX, hotY, &handle) )
        return False;

    m_customCursors[cursorKey] = handle;
    return True;
}

auto CursorMgr::set(StringView key) -> Bool
{
    if ( !m_window )
    {
        KAZE_CORE_ERRCODE(Error::LogicErr, "window must be set before CursorMgr can set cursor");
        return False;
    }

    auto it = m_customCursors.find(String(key));

    if (it == m_customCursors.end())
    {
        KAZE_CORE_ERRCODE(Error::MissingKeyErr,
            "Attempted to set cursor with key \"{}\", but it does not exist.", key);
        return False;
    }

    backend::cursor::setCursor(const_cast<WindowHandle>(m_window->getHandle()), it->second);

    return True;
}

auto CursorMgr::set(CursorType type) -> Bool
{
    if ( !m_window )
    {
        KAZE_CORE_ERRCODE(Error::LogicErr, "window must be set before CursorMgr can set cursor");
        return False;
    }

    auto it = m_systemCursors.find(type);

    if (it == m_customCursors.end())
    {
        KAZE_CORE_ERRCODE(Error::MissingKeyErr,
            "Attempted to set cursor with key \"{}\", but it does not exist.", key);
        return False;
    }

    backend::cursor::setCursor(const_cast<WindowHandle>(m_window->getHandle()), it->second);

    return True;
}

auto CursorMgr::createImpl(const Image &image, Vec2i anchor) -> CursorHandle
{
    
}
auto CursorMgr::setImpl(CursorType type) -> Bool
{
    
}
auto CursorMgr::setImpl(CursorHandle handle) -> Bool
{
    
}
auto CursorMgr::destroyImpl(CursorHandle handle) -> Bool
{
    
}

KAZE_NAMESPACE_END

