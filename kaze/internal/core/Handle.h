#pragma once
#include "Pool.h"
#include <kaze/internal/core/debug.h>

KAZE_NS_BEGIN

/// Pool handle
template <typename T>
class Handle {
public:
    Handle() : m_id(), m_pool() { } // invalid null handle
    Handle(PoolID id, PoolBase *pool) : m_id(id), m_pool(pool) { }
    ~Handle() = default;

    static auto makeConst(const Handle<T> &handle) -> Handle<const T>
    {
        return Handle<const T> (handle.m_id, handle.m_pool);
    }

    /// Get raw pointer; make sure to check `Handle::isValid()` before use.
    [[nodiscard]]
    T *get() const { return (T *)m_pool->get(m_id); }

    /// Get raw pointer; make sure to check `Handle::isValid()` before use.
    [[nodiscard]]
    T &operator *() const { return *(T *)m_pool->get(m_id); }

    /// Get the raw internal pointer, dynamically casting it to the target type.
    template <typename U>
    [[nodiscard]]
    auto getAs() const -> U *
    {
        return dynamic_cast<U *>(get());
    }

    /// Check with owning pool that this handle is valid
    [[nodiscard]]
    auto isValid() const -> Bool
    {
        // checking for `nullptr` covers default-constructed null handles
        return m_pool != nullptr && m_pool->isValid(m_id);
    }

    /// Access operator, runs a quick validation check before access so that pooled types should check for
    /// handle validation before performing the operation.
    [[nodiscard]]
    T *operator ->() const
    {
        if (!isValid())
            KAZE_PUSH_ERR(Error::InvalidHandle, "Attempted to access invalid Handle<{}>", typeid(T).name());
        return (T *)m_pool->get(m_id);
    }

    /// \returns whether internal pool id was set
    [[nodiscard]]
    explicit operator Bool() const
    {
        return static_cast<Bool>(m_id);
    }

    /// Short-hand function version of the handle-casting operator, so you don't have to write out
    /// `static_cast<Handle<U>>(handle)` by hand. Target type must be dynamically castable
    /// from the Handle's current type, otherwise an invalid handle will be returned.
    template <typename U>
    [[nodiscard]]
    auto cast() const -> Handle<U>
    {
        return static_cast<Handle<U>>(*this);
    }

    /// Handle is castable to another handle as long as it is in the inheritance hierarchy.
    /// And invalid handle will be returned otherwise.
    template <typename U>
    [[nodiscard]]
    explicit operator Handle<U>() const
    {
        auto uptr = dynamic_cast<U *>(get());
        return uptr ? Handle<U>(m_id, m_pool) : Handle<U>();
    }

    template <typename U>
    [[nodiscard]]
    auto operator ==(const Handle<U> &other) const -> Bool
    {
        return m_id.id == other.m_id.id && m_pool == other.m_pool;
    }

    template <typename U>
    [[nodiscard]]
    auto operator !=(const Handle<U> &other) const -> Bool
    {
        return !operator==(other);
    }

    [[nodiscard]]
    auto id() const -> PoolID { return m_id; }

private:
    friend class MultiPool;
    PoolID m_id;
    mutable PoolBase *m_pool;
};

KAZE_NS_END
