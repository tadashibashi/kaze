/// @file Camera2D.h
/// Camera2D class
#pragma once
#ifndef kaze_app_camera2d_h_
#define kaze_app_camera2d_h_

#include <kaze/kaze.h>
#include <kaze/math/Matrix.h>
#include <kaze/math/Rect.h>
#include <kaze/video/Window.h>

KAZE_NAMESPACE_BEGIN

class Camera2D {
public:

    /// Set the viewport rectangle
    /// @param[in] viewport rectangle to set
    /// @returns this class
    auto setViewport(Recti viewport) -> Camera2D &;

    /// Get the viewport rectangle
    [[nodiscard]] auto getViewport() const -> Recti { return m_viewport; }

    /// Set the normalized origin/anchor point of the camera in the viewport.
    /// @param[in] origin origin anchor point to set
    /// @returns this class
    auto setOrigin(Vec2f origin) noexcept -> Camera2D &;

    /// Get the normalized origin/anchor point of the camera in the viewport.
    [[nodiscard]] auto getOrigin() const noexcept -> Vec2f { return m_origin; }

    /// Set the logical 2D world coordinates of the camera
    /// @param[in] position the position to set
    /// @returns this class
    auto setPosition(Vec2f position) noexcept -> Camera2D &;

    /// Get the logical 2D world coordinates of the camera
    [[nodiscard]]
    auto getPosition() const noexcept -> Vec2f;

    /// Set the rotation, in radians, of the camera about the Z-axis
    /// @param[in] radians number of radians, where 0 is at the top, and moves clockwise
    auto setRotation(Float radians) noexcept -> Camera2D &;

    /// Get the rotation, in radians, of the camera about the Z-axis
    [[nodiscard]]
    auto getRotation() const noexcept -> Float;

    /// Set the rotation, in degrees, of the camera about the Z-axis
    /// @param[in] degrees number of degrees where 0 is at the top, and moves clockwise
    /// @returns this class
    auto setRotationDegrees(Float degrees) noexcept -> Camera2D &;

    /// Get the rotation, in degrees, of the camera about the Z-axis
    [[nodiscard]]
    auto getRotationDegrees() const noexcept -> Float;

    /// Set the scale the camera is zoomed to
    auto setScale(Vec2f scale) noexcept -> Camera2D &;

    /// Get the scale the camera is zoomed to
    [[nodiscard]]
    auto getScale() const noexcept -> Vec2f;

    /// Pointer to the inverse of the view matrix, 16-floats long in column major order
    [[nodiscard]]
    auto getInvView() const noexcept -> const Float * { applyChanges(); return m_invView.data(); }

    /// Pointer to the view matrix, 16-floats long in column-major order
    [[nodiscard]]
    auto getView() const noexcept -> const Float * { applyChanges(); return m_view.data(); }

    [[nodiscard]]
    auto getInvProj() const noexcept -> const Float * { applyChanges(); return m_invProj.data(); }

    [[nodiscard]]
    auto getProj() const noexcept -> const Float * { applyChanges(); return m_proj.data(); }

    /// Convert view to world coordinates
    [[nodiscard]]
    auto viewToWorld(Vec2f viewCoords) const noexcept -> Vec2f;

    /// Convert world to view coordinates
    [[nodiscard]]
    auto worldToView(Vec2f worldCoords) const noexcept -> Vec2f;

    /// Convert clip space to world coordinates
    [[nodiscard]]
    auto clipSpaceToWorld(Vec2f clipSpace) const noexcept -> Vec2f;

    /// Convert world to clip space coordinates
    [[nodiscard]]
    auto worldToClipSpace(Vec2f worldCoords) const noexcept -> Vec2f;

    [[nodiscard]]
    auto screenToWorld(Vec2f screenCoords, const Window &window) const noexcept -> Vec2f;

    [[nodiscard]]
    auto worldToScreen(Vec2f worldCoords, const Window &window) const noexcept -> Vec2f;
private:
  auto applyChanges() const -> void;

  mutable Mat4f m_view{}, m_invView{}, m_proj{}, m_invProj{};
  Vec2f m_origin{0.5f, 0.5f}, m_scale{1.f, 1.f};
  Recti m_viewport{};
  Float m_rotation{}; ///< rotation along Z-axis in radians
  Vec2f m_position{};
  mutable Bool m_wasChanged{true};
};

KAZE_NAMESPACE_END

#endif // kaze_app_camera2d_h_
