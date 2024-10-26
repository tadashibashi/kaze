#include "CursorMgr.h"
#include <kaze/core/platform/backend/backend.h>
#include <kaze/core/video/Window.h>

KAZE_NAMESPACE_BEGIN

CursorMgr::~CursorMgr()
{
    clear();
}

auto CursorMgr::create(const String &key, const Image &image, Vec2i anchor) -> Bool
{
    if (m_customCursors.contains(key)) // guard against duplicates
    {
        KAZE_CORE_ERRCODE(Error::DuplicateKey, "Key \"{}\" already exists in CursorMgr", key);
        return False;
    }

    CursorHandle handle;
    if ( !backend::cursor::createCustom(image.handle(), anchor.x, anchor.y, &handle) )
        return False;

    m_customCursors[key] = handle;
    return True;
}

auto CursorMgr::set(const String &key) -> Bool
{
    if ( !m_window )
    {
        KAZE_CORE_ERRCODE(Error::LogicErr, "window must be set before CursorMgr can set cursor");
        return False;
    }

    if (const auto it = m_customCursors.find(String(key));
        it == m_customCursors.end())
    {
        KAZE_CORE_ERRCODE(Error::MissingKeyErr,
            "Attempted to set cursor with key \"{}\", but it does not exist.", key);
        return False;
    }
    else
    {
        return backend::cursor::setCursor(m_window, it->second);
    }
}

auto CursorMgr::set(CursorType type) -> Bool
{
    if ( !m_window )
    {
        KAZE_CORE_ERRCODE(Error::LogicErr, "window must be set before CursorMgr can set cursor");
        return False;
    }

    // Get cursor if it exists
    CursorHandle cursor;
    if (auto it = m_systemCursors.find(type); it != m_systemCursors.end())
    {
        cursor = it->second;
    }
    else
    {
        // Otherwise, create and cache it
        if ( !backend::cursor::createStandard(type, &cursor) )
        {
            return False;
        }

        m_systemCursors[type] = cursor;
    }

    return backend::cursor::setCursor(
        m_window,
        cursor);
}

auto CursorMgr::erase(const String &key) -> Bool
{
    auto it = m_customCursors.find(key);
    if (it == m_customCursors.end())
    {
        KAZE_CORE_ERRCODE(Error::MissingKeyErr, "Cursor::erase: key \"{}\" does not exist in container", key);
        return False;
    }

    backend::cursor::destroy(it->second);
    m_customCursors.erase(it);
    return True;
}

auto CursorMgr::clear() -> void
{
    if ( !m_customCursors.empty() )
    {
        for (auto &[key, cursor] : m_customCursors)
        {
            backend::cursor::destroy(cursor);
        }
        m_customCursors.clear();
    }

    if ( !m_systemCursors.empty() )
    {
        for (auto &[key, cursor] : m_systemCursors)
        {
            backend::cursor::destroy(cursor);
        }
        m_systemCursors.clear();
    }
}

KAZE_NAMESPACE_END

