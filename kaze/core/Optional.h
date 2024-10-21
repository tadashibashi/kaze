#pragma once
#ifndef kaze_core_optional_h_
#define kaze_core_optional_h_

#include <kaze/core/lib.h>

KAZE_NAMESPACE_BEGIN


template <typename T>
// ReSharper disable once CppClassNeedsConstructorBecauseOfUninitializedMember
class Optional
{
    union
    {
        // ReSharper disable once CppUninitializedNonStaticDataMember
        T m_value;
    };
    bool m_hasValue{};
public:
    template <typename U = T, std::enable_if_t<std::is_copy_assignable_v<U>, int> = 0>
    explicit Optional(const T &value) : m_hasValue(true)
    {
        m_value = value;
    }

    auto operator=(const T &value) -> std::enable_if_t<std::is_copy_assignable_v<T>, Optional &>
    {
        m_value = value;
        m_hasValue = true;
        return *this;
    }

    auto operator=(T &&value) -> std::enable_if_t<std::is_move_assignable_v<T>, Optional &>
    {
        m_value = std::move(value);
        m_hasValue = true;
        return *this;
    }

    explicit operator bool() const
    {
        return m_hasValue;
    }

    T &value()
    {
        return m_value;
    }
};

KAZE_NAMESPACE_END

#endif // kaze_core_optional_h_
