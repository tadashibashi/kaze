#pragma once

#include <kaze/core/lib.h>
#include <kaze/core/memory.h>

KAZE_NS_BEGIN

struct PoolID {
    PoolID(const Size index, const Size id) : index(index), id(id) { }
    PoolID(); // null id
    Size index, id;

    explicit operator bool() const;

    /// To be used with a hashing class to enable PoolID hashing
    struct Hasher {
        [[nodiscard]]
       auto operator()(const PoolID &id) const noexcept ->  Size {
            return id.id;
        }
    };

    /// To be used with a hashing class to enable PoolID hashing
    struct Equals {
        [[nodiscard]]
        auto operator()(const PoolID &a, const PoolID &b) const noexcept -> Bool
        {
            return a.id == b.id;
        }
    };
};

/// Abstract class.
/// Stores fixed blocks of memory, expanding when full capacity is reached.
/// This class is intended to be a generic base to group pools under.
/// \note Use Pool<T> for type-safe pools.
class PoolBase {
public:
    /// \param elemSize size of one slot in bytes
    explicit PoolBase(Size elemSize);
    virtual ~PoolBase();

    // Non-copyable
    PoolBase(const PoolBase &) = delete;
    PoolBase &operator=(const PoolBase &) = delete;

    // Movable
    PoolBase(PoolBase &&other) noexcept;
    PoolBase &operator=(PoolBase &&other) noexcept;

    /// Get a slot of pool data. Pool may expand if it is full.
    auto allocate() -> PoolID;

    /// Allocate data slots ahead of time to prevent dynamic resizes
    auto reserve(Size size) -> void;

    /// Returns memory ownership back to the pool.
    /// \note user should run any cleanup logic on the memory before calling deallocate.
    ///
    /// \param id
    void deallocate(const PoolID &id);

    /// Check if an id returned from `allocate` is valid. Does not differentiate between ids from other pools,
    /// so user must make sure that PoolID is from the correct pool.
    [[nodiscard]]
    auto isValid(const PoolID &id) const -> Bool { return id.index < m_size && m_meta[id.index].id.id == id.id; }

    /// Users of the pool should access memory via this function instead of caching pointers long-term
    /// as memory resizing can cause pointers to become invalidated should the pool be dynamically resized.
    /// Returns `nullptr` if id is invalid
    auto get(const PoolID &id) -> void *
    {
        return m_memory + id.index * m_elemSize;
    }

    auto tryFind(void *ptr, PoolID *outID) const -> Bool;

    [[nodiscard]]
    auto get(const PoolID &id) const -> const void *
    {
        return isValid(id) ? m_memory + id.index * m_elemSize : nullptr;
    }

    [[nodiscard]]
    auto maxSize() const -> Size { return m_size; }

    /// Size of one element in the pool
    [[nodiscard]]
    auto elemSize() const -> Size { return m_elemSize; }

    /// Deallocate all memory.
    /// Does not run any cleanup logic, though - please make sure to clean up memory before calling clear.
    auto clear() -> void;

    /// Raw memory
    [[nodiscard]]
    auto data() -> char * { return m_memory; }

    /// Raw memory
    [[nodiscard]]
    auto data() const -> const char * { return m_memory; }

    // /// DO NOT USE. All handles become invalidated, and there is no solution yet.
    // /// \param newSize    size to shrink to; if less than `aliveCount()`, it will use the alive count.
    // /// \param outIndices map containing inner id keys to their updated indices. (optional)
    // /// \returns whether shrink occurred. If current size is <= `newSize`, `false` will be returned.
    // bool shrink(Size newSize, std::unordered_map<Size, Size> *outIndices = nullptr);

protected:
    struct Meta {
        Meta(const PoolID id, const Size nextFree) : id(id), nextFree(nextFree) { }
        PoolID id;
        Size nextFree;
    };

    /// Check if pool is currently filled to maximum capacity
    [[nodiscard]] bool isFull() const;

    virtual void expand(Size newSize) = 0;

    char *m_memory;               ///< pointer to storage
    Meta *m_meta;                 ///< contains information on a slot of memory
    Size m_size;                ///< current pool size
    Size m_nextFree;            ///< next free pool index
    Size m_elemSize;            ///< size of each memory block
    Size m_idCounter;           ///< next id to set on `allocate`
};

// Implements type safety for non-trivial data types by calling move constructor on memory reallocation
template <Poolable T>
class Pool final : public PoolBase {
public:
    Pool() : PoolBase(sizeof(T)) { }
    explicit Pool(Size initSize) : PoolBase(sizeof(T))
    {
        reserve(initSize);
    }

    Pool(Pool &&other) : PoolBase(std::move(other)) { }
    Pool &operator=(Pool &&other)
    {
        if (this != &other)
        {
            cleanup(); // destruct all pool elements before freeing
            PoolBase::operator=(std::move(other));
        }

        return this;
    }

    ~Pool() override
    {
        cleanup();
    }

    void expand(Size newSize) override
    {
        const auto lastSize = m_size;
        if (lastSize >= newSize) // no need to expand if new size isn't greater
            return;

        if (m_memory == nullptr)
        {
            m_memory = (char *)std::malloc(newSize * sizeof(T));
            m_meta = (Meta *)std::malloc(newSize * sizeof(Meta));
        }
        else
        {
            auto temp = (char *)memory::alloc(newSize * sizeof(T));

            for (T *ptr = (T *)m_memory, *end = (T *)m_memory + lastSize, *target = (T *)temp; ptr != end; ++ptr, ++target)
            {
                new (target) T(std::move(*ptr));
                ptr->~T();
            }

            memory::free(m_memory);
            m_memory = temp;

            auto metaTemp = (Meta *)memory::alloc(newSize * sizeof(Meta));
            memory::copy(metaTemp, m_meta, lastSize * sizeof(Meta));
            memory::free(m_meta);
            m_meta = metaTemp;
        }

        // TODO: unroll these loops?
        // Initialize objects in new indices
        for (Size i = lastSize; i < newSize; ++i)
        {
            new (m_meta + i) Meta(PoolID(i, SIZE_MAX), i+1);
            new ((T *)m_memory + i) T();
        }

        m_meta[newSize - 1].nextFree = SIZE_MAX;
        m_size = newSize;
    }

private:
    void cleanup()
    {
        for (auto ptr = (T *)m_memory, end = (T *)m_memory + m_size; ptr != end; ++ptr)
        {
            ptr->~T();
        }
    }
};

KAZE_NS_END
