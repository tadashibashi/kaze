#pragma once

#include <kaze/core/lib.h>

KAZE_NS_BEGIN

// ===== funcptr_t =====
template <typename T>
struct funcptr;
template <typename R, typename... TArgs>
struct funcptr<R(TArgs...)> {
  using type = R (*)(TArgs...);
};

template <typename T>
using funcptr_t = typename funcptr<T>::type;

template <typename T>
struct member_func_return;
template <typename R, typename C, typename... Args>
struct member_func_return<R(C::*)(Args...)> {
  using type = R;
};
template <typename R, typename C, typename... Args>
struct member_func_return<R(C::*)(Args...) const> {
  using type = R;
};

template <typename T>
using member_func_return_t = member_func_return<T>::type;

KAZE_NS_END
