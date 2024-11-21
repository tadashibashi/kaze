#pragma once
#include "lib.h" // IWYU pragma: export
#include <kaze/internal/core/Handle.h>
#include <kaze/internal/core/MultiPool.h>
#include <kaze/internal/core/Pool.h>

#include <kaze/internal/core/concepts.h>

KAZE_PUBLIC_NS_BEGIN

using KAZE_NS_INTERNAL::Handle;
using KAZE_NS_INTERNAL::Pool;
using KAZE_NS_INTERNAL::Poolable;
using KAZE_NS_INTERNAL::PoolBase;
using KAZE_NS_INTERNAL::PoolID;

KAZE_PUBLIC_NS_END
