#pragma once
#include <kaze/internal/core/lib.h>
#include <kaze/internal/core/math/Vec/Vec3.h>
#include <kaze/internal/core/math/Vec/Vec4.h>

KAZE_NS_BEGIN

/// Wrapper around bgfx matrix functions due to handling platform-based normalization
namespace mathf::mat4f {
    auto lookAt(
        Float *result,
        const Vec3f &eye,
        const Vec3f &at,
        const Vec3f &up = { 0.0f, 1.0f, 0.0f }) -> void;

    auto proj(
        Float *result,
        Float ut,
        Float dt,
        Float lt,
        Float rt,
        Float near,
        Float far) -> void;

    auto proj(
        Float *result,
        const Vec4f &fov,
        Float near,
        Float far) -> void;

    auto proj(
        Float *result,
        Float fovy,
        Float aspect,
        Float near,
        Float far) -> void;

    auto projInf(
        Float *result,
        const Vec4f &fov,
        Float near) -> void;

    auto projInf(
        Float *result,
        Float ut,
        Float dt,
        Float lt,
        Float rt,
        Float near) -> void;

    auto projInf(
        Float *result,
        Float fovy,
        Float aspect,
        Float near) -> void;

    auto ortho(
        Float *result,
        Float left,
        Float right,
        Float bottom,
        Float top,
        Float near,
        Float far,
        Float offset) -> void;
}

KAZE_NS_END
