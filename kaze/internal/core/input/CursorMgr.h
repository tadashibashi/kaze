#pragma once
#include <kaze/internal/core/lib.h>
#include <kaze/internal/core/ImageContainer.h>
#include <kaze/internal/core/WindowConstants.h>
#include <kaze/internal/core/input/CursorConstants.h>
#include <kaze/internal/core/math/Vec/Vec2.h>

KAZE_NS_BEGIN

/// Cursor manager for a window
class CursorMgr {
public:
    CursorMgr() : m_window(), m_customCursors(), m_systemCursors() { }
    ~CursorMgr();

    auto setWindow(WindowHandle window) -> CursorMgr & { m_window = window; return *this; }
    auto getWindow() const noexcept -> WindowHandle { return m_window; }

    /// Define a custom cursor to later be set
    /// \param[in]  key      id to reference this cursor
    /// \param[in]  image    image with pixel data
    /// \param[in]  anchor   offset where the pointer location exists within the image
    /// \returns whether the operation succeeded.
    auto create(const String &key, const ImageContainer &image, Vec2i anchor) -> Bool;

    /// Set a custom cursor
    /// \param[in]  key   the key of custom cursor to set (case-sensitive)
    /// \returns whether the operation succeeded.
    auto set(const String &key) -> Bool;

    /// Set a system cursor
    /// \param[in]  type   type of system cursor to set
    /// \returns whether the operation succeeded.
    auto set(CursorType type) -> Bool;

    /// Check if a custom cursor with a key exists in this container
    /// \param[in]  key   key of the cursor to check for (case-sensitive)
    auto contains(const String &key) const noexcept -> Bool { return m_customCursors.contains(key); }

    /// Erase a custom cursor
    /// \returns whether an entry was deleted.
    auto erase(const String &key) -> Bool;

    /// Clear all internal cursors from memory
    /// \returns whether operation succeeded.
    auto clear() -> void;
private:
    WindowHandle m_window;
    Dictionary<CursorType, CursorHandle> m_systemCursors;
    Map<String, CursorHandle> m_customCursors;
};

KAZE_NS_END
