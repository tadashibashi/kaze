#include "mat4f.h"
#include <bx/math.h>
#include <kaze/internal/core/math/mathf.h>

KAZE_NS_BEGIN

namespace mathf::mat4f {
    auto lookAt(
        Float *result,
        const Vec3f &eye,
        const Vec3f &at,
        const Vec3f &up) -> void
    {
        bx::mtxLookAt(result,
            {eye.x, eye.y, eye.z},
            {at.x, at.y, at.z},
            {up.x, up.y, up.z}
        );
    }

    auto proj(
        Float *result,
        Float ut,
        Float dt,
        Float lt,
        Float rt,
        Float near,
        Float far) -> void
    {
        bx::mtxProj(result,
            ut,
            dt,
            lt,
            rt,
            near,
            far,
            mathf::getHomogenousNDC());
    }

    auto proj(
        Float *result,
        const Vec4f &fov,
        Float near,
        Float far) -> void
    {
        bx::mtxProj(result,
            fov.ptr,
            near,
            far,
            mathf::getHomogenousNDC());
    }

    auto proj(
        Float *result,
        Float fovy,
        Float aspect,
        Float near,
        Float far) -> void
    {
        bx::mtxProj(result,
            fovy,
            aspect,
            near,
            far,
            mathf::getHomogenousNDC());
    }

    auto projInf(
        Float *result,
        const Vec4f &fov,
        Float near) -> void
    {
        bx::mtxProjInf(result,
            fov.ptr,
            near,
            mathf::getHomogenousNDC());
    }

    auto projInf(
        Float *result,
        Float ut,
        Float dt,
        Float lt,
        Float rt,
        Float near) -> void
    {
        bx::mtxProjInf(result,
            ut,
            dt,
            lt,
            rt,
            near,
            mathf::getHomogenousNDC());
    }

    auto projInf(
        Float *result,
        Float fovy,
        Float aspect,
        Float near) -> void
    {
        bx::mtxProjInf(result,
            fovy,
            aspect,
            near,
            mathf::getHomogenousNDC());
    }

    auto ortho(
        Float *result,
        Float left,
        Float right,
        Float bottom,
        Float top,
        Float near,
        Float far,
        Float offset) -> void
    {
        bx::mtxOrtho(result,
            left,
            right,
            bottom,
            top,
            near,
            far,
            offset,
            mathf::getHomogenousNDC());
    }
}

KAZE_NS_END
