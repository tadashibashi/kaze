#pragma once
#include <kaze/core/lib.h>
#include <kaze/core/memory.h>

KAZE_NS_BEGIN

class ManagedMem {
    static auto defaultDealloc(void *mem, void *userptr) {
        memory::free(mem);
    }
public:
    /// Null memory
    ManagedMem() : m_data(), m_size(), m_deallocator(), m_userptr() { }
    ManagedMem(
        void *data,
        Size size,
        funcptr_t<void(void *mem, void *userptr)> deallocator = Null,
        void *userptr = Null) :
            m_data(data),
            m_size(size),
            m_deallocator(deallocator ? deallocator : defaultDealloc),
            m_userptr(userptr)
    { }

    [[nodiscard]]
    auto data() const -> const void * { return m_data; }

    [[nodiscard]]
    auto data() -> void * { return m_data; }

    [[nodiscard]]
    auto size() const -> Size { return m_size; }

    auto release() -> void
    {
        KAZE_ASSERT(m_deallocator != Null);

        m_deallocator(m_data, m_userptr);
        m_data = Null;
        m_userptr = Null;
        m_size = 0;
    }

    [[nodiscard]]
    auto deallocator() const { return m_deallocator; }

    [[nodiscard]]
    auto userptr() const { return m_userptr; }
private:
    void *m_data;
    Size m_size;
    funcptr_t<void(void *mem, void *userptr)> m_deallocator;
    void *m_userptr;
};

KAZE_NS_END
