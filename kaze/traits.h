#pragma once
#ifndef kaze_traits_h_
#define kaze_traits_h_

#include <kaze/kaze.h>

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

#endif
