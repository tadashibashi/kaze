#pragma once
#ifndef kaze_core_input_cursor_h_
#define kaze_core_input_cursor_h_

#include <kaze/core/lib.h>
#include <kaze/core/input/CursorConstants.h>
#include <kaze/core/math/Vec/Vec2.h>
#include <kaze/core/video/Image.h>

#include <concepts>

KAZE_NAMESPACE_BEGIN

class Window;

/// Cursor manager for a window
template <std::equality_comparable K>
class CursorMgr {
public:
    CursorMgr() : m_window(), m_customCursors(), m_systemCursors() { }
    explicit CursorMgr(const Window *window) : m_window(window), m_customCursors(), m_systemCursors() { }

    auto setWindow(const Window *window) -> CursorMgr & { m_window = window; return *this; }
    auto getWindow() const noexcept -> const Window * { return m_window; }

    /// Define a custom cursor to later be set
    /// \param[in]  key      id to reference this cursor
    /// \param[in]  image    image with pixel data
    /// \param[in]  anchor   offset where the pointer location exists within the image
    /// \returns whether the operation succeeded.
    auto create(const K &key, const Image &image, Vec2i anchor) -> Bool;

    /// Set a custom cursor
    /// \param[in]  key   the key of custom cursor to set (case-sensitive)
    /// \returns whether the operation succeeded.
    auto set(const K &key) -> Bool;

    /// Set a system cursor
    /// \param[in]  type   type of system cursor to set
    /// \returns whether the operation succeeded.
    auto set(CursorType type) -> Bool;

    /// Check if a custom cursor with a key exists in this container
    /// \param[in]  key   key of the cursor to check for (case-sensitive)
    auto contains(StringView key) const noexcept -> Bool;

    /// Erase a custom cursor
    /// \returns whether an entry was deleted.
    auto erase(StringView key) -> Bool;

    /// Clear all internal cursors from memory
    /// \returns whether operation succeeded.
    auto clear() -> CursorMgr &;
private:
    auto createImpl(const Image &image, Vec2i anchor) -> CursorHandle;
    auto setImpl(CursorType type) -> Bool;
    auto setImpl(CursorHandle handle) -> Bool;
    auto destroyImpl(CursorHandle handle) -> Bool;

    const Window *m_window;
    Dictionary<CursorType, CursorHandle> m_systemCursors;
    Dictionary<Size, CursorHandle> m_customCursors;
};

KAZE_NAMESPACE_END


#endif // kaze_core_input_cursor_h_
