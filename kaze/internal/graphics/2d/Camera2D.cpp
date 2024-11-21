#include "Camera2D.h"

KGFX_NS_BEGIN

auto Camera2D::setOrigin(const Vec2f origin) noexcept -> Camera2D &
{
    m_origin = origin;
    m_wasChanged = True;
    return *this;
}

auto Camera2D::setViewport(const Recti viewport) -> Camera2D &
{
    if (viewport == m_viewport) return *this;

    m_viewport = viewport;
    m_proj = Mat4f::fromOrtho(viewport.left(), viewport.right(), viewport.top(), viewport.bottom());
    m_invProj = m_proj.toInverse();

    m_wasChanged = True;
    return *this;
}

auto Camera2D::viewToWorld(const Vec2f viewCoords) const noexcept -> Vec2f
{
    applyChanges();
    const auto worldCoords = m_invView * Vec4f(viewCoords.x, viewCoords.y, 0, 1);
    return {worldCoords.x, worldCoords.y};
}

auto Camera2D::worldToView(Vec2f worldCoords) const noexcept -> Vec2f
{
    applyChanges();
    const auto viewCoords = m_view * Vec4f(worldCoords.x, worldCoords.y, 0, 1.f);
    return {viewCoords.x, viewCoords.y};
}

auto Camera2D::clipSpaceToWorld(Vec2f clipSpace) const noexcept -> Vec2f
{
    applyChanges();
    const auto viewCoords = m_invProj * Vec4f(clipSpace.x, clipSpace.y, 0, 1.f);
    const auto worldCoords = m_invView * viewCoords;
    return {worldCoords.x, worldCoords.y};
}

auto Camera2D::worldToClipSpace(Vec2f worldCoords) const noexcept -> Vec2f
{
    applyChanges();
    const auto viewCoords = m_view * Vec4f(worldCoords.x, worldCoords.y, 0, 1.f);
    const auto clipSpace = m_proj * viewCoords;
    return {clipSpace.x, clipSpace.y};
}

auto Camera2D::screenToWorld(Vec2f screenCoords, const Window &window) const noexcept -> Vec2f
{
    const auto dpiScale = window.getDPIScale();
    const auto size = window.getSize() / dpiScale;

    const auto ndc = Vec2f {
        (2 * screenCoords.x) / size.x - 1.f,
        1.f - (2.f * screenCoords.y) / size.y
    };

    return clipSpaceToWorld(ndc);
}

auto Camera2D::worldToScreen(Vec2f worldCoords, const Window &window) const noexcept -> Vec2f
{
    const auto dpiScale = window.getDPIScale();
    const auto size = window.getSize() * dpiScale;

    const auto ndc = worldToClipSpace(worldCoords);
    return {
        (ndc.x + 1) * .5f * size.x,
        (1.f - ndc.y) * .5f * size.y
    };
}

auto Camera2D::applyChanges() const -> void
{
    if ( !m_wasChanged ) return;

    m_view = Mat4f()
        .translate(m_origin * m_viewport.size)
        .rotate(m_rotation, {0.f, 0.f, 1.f})
        .scale(m_scale)
        .translate(-m_position);
    m_invView = m_view.toInverse();
    m_wasChanged = KAZE_FALSE;
}

auto Camera2D::setPosition(Vec2f position) noexcept -> Camera2D &
{
    m_position = position;
    m_wasChanged = KAZE_TRUE;
    return *this;
}

auto Camera2D::getPosition() const noexcept -> Vec2f
{
    return m_position;
}

auto Camera2D::setRotation(const Float radians) noexcept -> Camera2D &
{
    m_rotation = mathf::fmod(radians, static_cast<Float>(mathf::TwoPi));
    m_wasChanged = KAZE_TRUE;
    return *this;
}

auto Camera2D::getRotation() const noexcept -> Float
{
    return m_rotation;
}

auto Camera2D::setRotationDegrees(const Float degrees) noexcept -> Camera2D &
{
    m_rotation = mathf::fmod(mathf::toRadians(degrees), static_cast<Float>(mathf::TwoPi));
    m_wasChanged = KAZE_TRUE;
    return *this;
}

auto Camera2D::getRotationDegrees() const noexcept -> Float
{
    return mathf::toDegrees(m_rotation);
}

auto Camera2D::setScale(Vec2f scale) noexcept -> Camera2D &
{
    m_scale = scale;
    m_wasChanged = KAZE_TRUE;
    return *this;
}

auto Camera2D::getScale() const noexcept -> Vec2f
{
    return m_scale;
}

KGFX_NS_END
