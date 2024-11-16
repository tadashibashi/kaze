#pragma once
#include "Handle.h"
#include "Pool.h"

#include <kaze/core/debug.h>
#include <kaze/core/traits.h>

#include <mutex>
#include <typeindex>

KAZE_NS_BEGIN

/// Pool that manages storage of any number of types of concrete objects.
///
/// A poolable class must contain two publicly visible functions:
/// - `bool init_(TArgs args...)`
/// - `void release_()`
/// It must also properly implement move construction and assignment
///
/// When `MultiPool::allocate` is called, the pool class's `init` is called.
/// The args of `allocate` are forwarded to the poolable object's `init`.
/// When the pool object is deallocated, `release` is called (with no arguments).
///
/// These two functions are the setup and cleanup functions for your pooled objects.
/// Actual construction occurs during pool initialization and expansion when an allocation exceeds
/// the current size of a given pool and new elements are dynamically added.
/// The actual destructor is called when the pool is deleted.
///
/// For subclasses, make sure init and release calls its parent init and release if this is important.
/// Whether they are virtual or not is up to you.
///
/// Pool contains its own mutex, so that it is safe to use with multiple threads.
class MultiPool {
public:
    MultiPool() = default;
    ~MultiPool()
    {
        for (auto &[type, pool] : m_pools)
        {
            delete pool;
        }
    }

    /// Allocate an object, specified by type
    /// \tparam   T type of object to allocate
    /// \tparam   ...TArgs type of arguments to forward to it's `init_` function
    ///
    /// \param[in] ...args arguments to forward to the object's `init_` function.
    /// \return a handle to the new object; object can be accessed via pointer semantics; if an error occurred during the
    ///    call to `T::init` then a null object will be returned, taking back the allocation. In this case, check `popError()`
    ///    for  more details.
    template <Poolable T, typename...TArgs>
    auto allocate(TArgs &&...args) noexcept -> Handle<T>
    {
        static_assert(!std::is_abstract_v<T>, "Cannot allocate an abstract class");
        std::lock_guard lockGuard(m_mutex);

        PoolBase *pool = &getPool<T>();
        const auto elemSize = static_cast<uint32_t>(pool->elemSize());

        // Allocate new entity
        PoolID id = pool->allocate();
        try {
            // Init the newly retrieved entity
            ((T *)(pool->data() + id.index * elemSize))->init_(std::forward<TArgs>(args)...); // `T` poolable must implement `init`
        }
        catch (const std::exception &err) { // init threw an exception, deallocate
            KAZE_PUSH_ERR(Error::RuntimeErr, "Exception was thrown during Handle<{}>::allocate in object's ctor: {}",
                typeid(T).name(), err.what());
            pool->deallocate(id);
            return {};
        }
        catch (...) {                       // unknown error thrown, deallocate
            KAZE_PUSH_ERR(Error::RuntimeErr, "constructor threw unknown error");
            pool->deallocate(id);
            return {};
        }

        return Handle<T>(id, pool);
    }

    /// Deallocate a handle that was retrieved from `MultiPool::allocate()`
    /// \tparam T this actually does not matter here as the handle will deallocate from its
    ///           associated pool. It's just to satisfy the varied number of handles.
    /// \returns whether deallocation succeeded without problems. If false, handle was invalid, or an error was
    ///          thrown in the destructor. Check `popError()` for details. Object will still deallocate back to
    ///          the pool if if a valid handle was passed.
    template <typename T>
    auto deallocate(const Handle<T> &handle) noexcept -> Bool
    {
        std::lock_guard lockGuard(m_mutex);

        Bool dtorThrew = False;
        if (handle.isValid())
        {
            try // catch any exception propagated from client `release_()`
            {
                handle->release_();
            }
            catch (const std::exception &err)
            {
                KAZE_PUSH_ERR(Error::RuntimeErr, "Exception was thrown during Handle<{}>::deallocate during "
                    "`release`: {}", typeid(T).name(), err.what()); // don't propagate exception, just push as error
                dtorThrew = True;
            }
            catch (...)
            {
                KAZE_PUSH_ERR(Error::RuntimeErr, "Unknown exception thrown in pool object `release()`");
                dtorThrew = True;
            }
        }
        else
        {
            KAZE_PUSH_ERR(Error::InvalidHandle, "Invalid handle was passed to MultiPool::deallocate");
            return False;
        }

        handle.m_pool->deallocate(handle.m_id);
        return !dtorThrew;
    }

    /// Try to find a handle from a raw pointer.
    /// \tparam T must be a concrete type, and target the final class (as opposed to a class up the heirarchy),
    ///           you are looking for a pool of literal objects.
    /// \param[in]  pointer   the pooled object pointer to query.
    /// \param[out] outHandle pointer to retrieve the handle if found.
    /// \returns whether a handle was found for the pointer; `outHandle` will remain unmodified on false.
    template <typename T>
    auto tryFind(T *pointer, Handle<T> *outHandle) const -> Bool
    {
        static_assert(!std::is_abstract_v<T>, "Cannot find an abstract pool object");

        std::lock_guard lockGuard(m_mutex);
        if (pointer == nullptr) return false;

        auto &pool = getPool<T>();

        PoolID id;
        if (!pool.tryFind(pointer, &id))
            return false;

        if (outHandle)
            *outHandle = Handle<T>(id, &pool);
        return true;
    }

    /// Reserve memory for a specific non-abstract Poolable type
    /// \param[in] size number of `T` elements to reserve space for
    template <Poolable T>
    void reserve(const Size size)
    {
        static_assert(!std::is_abstract_v<T>, "Cannot reserve space for an abstract class");

        std::lock_guard lockGuard(m_mutex);
        getPool<T>(size).second.reserve(size);
    }

    /// Much less efficient than `tryFind` as we need to query each pool,
    /// but written here in case if needed later.
    /// It gives you what you need to create a generic handle.
    auto tryFindGeneric(void *ptr, PoolBase **outPool, PoolID *outID, std::type_index *outTypeIndex) -> Bool
    {
        for (auto &[type, pool] : m_pools)
        {
            if (pool->tryFind(ptr, outID))
            {
                if (outTypeIndex)
                    *outTypeIndex = type;
                if (outPool)
                    *outPool = pool;
                return true;
            }
        }

        return false;
    }

private:
    /// Get an existing pool for type `T`, or it will create a new one if a pool for type T does not exist.
    /// \tparam T must be concrete, since allocations to the pool are like calling `new`.
    /// \returns `Pool<T> &`, which can be cast to `PoolBase &` for generic storage.
    template <Poolable T>
    auto &getPool() const
    {
        static_assert(!std::is_abstract_v<T>, "Cannot allocate an abstract class");

        if (const auto it = m_indices.find(std::type_index(typeid(T)));
            it != m_indices.end())
        {
            return *(Pool<T> *)m_poolPtrs[it->second];
        }

        auto newPool = new Pool<T>;
        m_pools.emplace(typeid(T), newPool);
        m_indices.emplace(
            typeid(T),
            static_cast<int>(m_poolPtrs.size()));
        m_poolPtrs.emplace_back(newPool);

        return *newPool;
    }

    auto tryFindTypeIndex(void *ptr, std::type_index type, PoolBase **outPool, PoolID *outID) -> Bool
    {
        if (auto it = m_indices.find(type); it != m_indices.end())
        {
            const auto index = it->second;
            if (index >= m_poolPtrs.size()) // this would be an internal error, throw here?
                return false;

            auto pool = m_poolPtrs[it->second];
            if (pool == nullptr) // just in case
                return false;

            if (pool->tryFind(ptr, outID))
            {
                if (outPool)
                    *outPool = pool;
                return true;
            }
        }

        return false;
    }

private: // Member variables
    mutable Map<std::type_index, PoolBase *> m_pools;    ///< the internal pools
    mutable std::recursive_mutex m_mutex;                ///< for syncing pool state across multiple threads

    // for optimized lookups of pools (may be negligable)
    mutable Dictionary<std::type_index, int> m_indices;  ///< index lookup table by type; use it to query m_poolPtrs
    mutable List<PoolBase *> m_poolPtrs;                 ///< reference to pools, queried quickly by type via `m_indices`
};

KAZE_NS_END

