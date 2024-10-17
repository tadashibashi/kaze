#pragma once
#ifndef kaze_core_memory_h_
#define kaze_core_memory_h_

#include <kaze/kaze.h>
#include <type_traits>

KAZE_NAMESPACE_BEGIN

/// Wrapper around const memory for a convenient unified, and type-safe interface
/// See {@link `makeRef`} for overloads to conveniently create Mem objects from various containers and arrays.
template <typename T>
class Mem {
public:
    using value_type = T;
    Mem() noexcept : m_data(), m_size() { }
    Mem(const T *data, const Size size) noexcept : m_data(data), m_size(size) { }

    [[nodiscard]]
    auto data() const noexcept { return m_data; }

    /// Get the number of elements
    [[nodiscard]]
    auto size() const noexcept
    {
        if constexpr (std::is_void_v<T>)
            return m_size;
        else
            return m_size / sizeof(T);
    }

    /// Get the number of bytes of the data
    [[nodiscard]]
    auto byteLength() const noexcept { return m_size; }

    // ReSharper disable once CppNonExplicitConversionOperator
    operator Mem<void>() const noexcept
    {
        return Mem<void>(m_data, m_size);
    }
private:
    const T *m_data{};
    Size m_size{};
};

using Memory = Mem<void>;


inline auto makeRef(Cstring cstr) -> Mem<char>
{
    return Mem<char>(cstr, std::strlen(cstr));
}

template <typename T>
auto makeRef(const T *data, const Size elems) -> Mem<T>
{
    if constexpr (std::is_void_v<T>)
        return Mem<T>(data, elems);
    else
        return Mem<T>(data, elems * sizeof(T));
}

template <typename ArrayType, typename T = std::remove_extent_t<ArrayType>> requires
    std::is_array_v<ArrayType>
auto makeRef(const ArrayType &array)
{
    return Mem<T>(array, sizeof(array));
}

template<
    typename ContainerType,
    typename T = std::remove_pointer_t<decltype(std::declval<ContainerType>().data())>> requires
        std::is_pointer_v<decltype(std::declval<ContainerType>().data())> &&
        std::is_integral_v<decltype(std::declval<ContainerType>().size())>
auto makeRef(const ContainerType &container)
{
    return Mem<T>(container.data(), container.size() * sizeof(T));
}

KAZE_NAMESPACE_END

#endif // kaze_core_memory_h_
