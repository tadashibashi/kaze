#pragma once
#ifndef kaze_core_traits_h_
#define kaze_core_traits_h_

#include <kaze/core/lib.h>

KAZE_NAMESPACE_BEGIN

template <typename T>
struct funcptr;

template <typename R, typename... TArgs>
struct funcptr<R(TArgs...)> {
  using type = R (*)(TArgs...);
};

template <typename T>
using funcptr_t = typename funcptr<T>::type;

KAZE_NAMESPACE_END

#endif // kaze_core_traits_h_
