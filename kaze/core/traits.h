#pragma once

#include <kaze/core/lib.h>

KAZE_NS_BEGIN

template <typename T>
struct funcptr;

template <typename R, typename... TArgs>
struct funcptr<R(TArgs...)> {
  using type = R (*)(TArgs...);
};

template <typename T>
using funcptr_t = typename funcptr<T>::type;

KAZE_NS_END
