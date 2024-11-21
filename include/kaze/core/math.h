#pragma once
#include "lib.h" // IWYU pragma: export
#include <kaze/internal/core/math/mathf.h>
#include <kaze/internal/core/math/easings.h>
#include <kaze/internal/core/math/geometry.h>
#include <kaze/internal/core/math/Circle.h>
#include <kaze/internal/core/math/Matrix.h>
#include <kaze/internal/core/math/Rect.h>
#include <kaze/internal/core/math/Rect3D.h>
#include <kaze/internal/core/math/Vec.hpp>

KAZE_PUBLIC_NS_BEGIN

namespace mathf = KAZE_NS_INTERNAL::mathf;

using KAZE_NS_INTERNAL::Circle;
using Circi = KAZE_NS_INTERNAL::Circi;
using Circf = KAZE_NS_INTERNAL::Circf;
using Circd = KAZE_NS_INTERNAL::Circd;

using KAZE_NS_INTERNAL::Matrix;
using Mat3f = KAZE_NS_INTERNAL::Mat3f;
using Mat3d = KAZE_NS_INTERNAL::Mat3d;
using Mat4f = KAZE_NS_INTERNAL::Mat4f;
using Mat4d = KAZE_NS_INTERNAL::Mat4d;

using KAZE_NS_INTERNAL::Rect;
using Recti = KAZE_NS_INTERNAL::Recti;
using Rectf = KAZE_NS_INTERNAL::Rectf;
using Rectd = KAZE_NS_INTERNAL::Rectd;

using KAZE_NS_INTERNAL::Rect3D;
using Rect3Di = KAZE_NS_INTERNAL::Rect3Di;
using Rect3Df = KAZE_NS_INTERNAL::Rect3Df;
using Rect3Dd = KAZE_NS_INTERNAL::Rect3Dd;

using KAZE_NS_INTERNAL::Vec;
template <KAZE_NS_INTERNAL::Arithmetic T>
using Vec1 = KAZE_NS_INTERNAL::Vec<T, 1>;
using Vec1f = KAZE_NS_INTERNAL::Vec<KAZE_NS_INTERNAL::Float, 1>;
using Vec1d = KAZE_NS_INTERNAL::Vec<KAZE_NS_INTERNAL::Double, 1>;
using Vec1i = KAZE_NS_INTERNAL::Vec<KAZE_NS_INTERNAL::Int, 1>;

template <KAZE_NS_INTERNAL::Arithmetic T>
using Vec2 = KAZE_NS_INTERNAL::Vec<T, 2>;
using Vec2f = KAZE_NS_INTERNAL::Vec<KAZE_NS_INTERNAL::Float, 2>;
using Vec2d = KAZE_NS_INTERNAL::Vec<KAZE_NS_INTERNAL::Double, 2>;
using Vec2i = KAZE_NS_INTERNAL::Vec<KAZE_NS_INTERNAL::Int, 2>;

template <KAZE_NS_INTERNAL::Arithmetic T>
using Vec3 = KAZE_NS_INTERNAL::Vec<T, 3>;
using Vec3f = KAZE_NS_INTERNAL::Vec<KAZE_NS_INTERNAL::Float, 3>;
using Vec3d = KAZE_NS_INTERNAL::Vec<KAZE_NS_INTERNAL::Double, 3>;
using Vec3i = KAZE_NS_INTERNAL::Vec<KAZE_NS_INTERNAL::Int, 3>;

template <KAZE_NS_INTERNAL::Arithmetic T>
using Vec4 = KAZE_NS_INTERNAL::Vec<T, 4>;
using Vec4f = KAZE_NS_INTERNAL::Vec<KAZE_NS_INTERNAL::Float, 4>;
using Vec4d = KAZE_NS_INTERNAL::Vec<KAZE_NS_INTERNAL::Double, 4>;
using Vec4i = KAZE_NS_INTERNAL::Vec<KAZE_NS_INTERNAL::Int, 4>;

KAZE_PUBLIC_NS_END
