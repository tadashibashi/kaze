#pragma once
#include "lib.h" // IWYU pragma: export
#include <kaze/internal/core/ManagedMem.h>
#include <kaze/internal/core/MemView.h>
#include <kaze/internal/core/memory.h>

KAZE_PUBLIC_NS_BEGIN

namespace memory = KAZE_NS_INTERNAL::memory;

using KAZE_NS_INTERNAL::ManagedMem;
using KAZE_NS_INTERNAL::MemView;

inline auto makeRef(KAZE_NS_INTERNAL::Cstring str) -> MemView<char>
{
    return KAZE_NS_INTERNAL::makeRef(str);
}

template <typename T>
auto makeRef(const T *data, const KAZE_NS_INTERNAL::Size elems) -> MemView<T>
{
    return KAZE_NS_INTERNAL::makeRef(data, elems);
}

template <typename ArrayType, typename T = std::remove_extent_t<ArrayType>> requires
    std::is_array_v<ArrayType>
auto makeRef(const ArrayType &array)
{
    return KAZE_NS_INTERNAL::makeRef(array);
}

template<
    typename ContainerType,
    typename T = std::remove_pointer_t<decltype(std::declval<ContainerType>().data())>> requires
        std::is_pointer_v<decltype(std::declval<ContainerType>().data())> &&
        std::is_integral_v<decltype(std::declval<ContainerType>().size())>
auto makeRef(const ContainerType &container)
{
    return KAZE_NS_INTERNAL::makeRef(container);
}

KAZE_PUBLIC_NS_END
