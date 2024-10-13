#pragma once
#ifndef kaze_video_uniformmgr_h_
#define kaze_video_uniformmgr_h_

#include <kaze/kaze.h>
#include <kaze/math/Matrix.h>
#include <kaze/math/Vec/Vec4.h>

KAZE_NAMESPACE_BEGIN

class Color;
class Texture2D;

enum class UniformType {
    Unknown,
    Vec4, ///< 4 floats vector.
    Mat3, ///< 3x3 matrix.
    Mat4, ///< 4x4 matrix.
};

/// Wrapper around a Uniform id to set its value and get its info
class Uniform {
public:
    static const Uint16 InvalidHandle;

    explicit Uniform(const Uint16 handle = InvalidHandle) : m_handle(handle) { }
    auto operator= (const Color &value) -> Uniform &;
    auto operator= (const Vec4f &value) -> Uniform &;
    auto operator= (const Mat3f &value) -> Uniform &;
    auto operator= (const Mat4f &value) -> Uniform &;

    /// Set a color to normalized Vec4, if the uniform is of type Vec4
    /// @param[in]  value   the color to set
    /// @returns this object.
    auto setColor(const Color &value) -> Uniform &;

    /// Set a Vec4, if the uniform is of type Vec4
    /// @param[in]  value   the vector to set
    /// @returns this object.
    auto setVec4(const Vec4f &value) -> Uniform &;

    /// Set a 4x4 matrix, if the uniform is of type Mat4
    /// @param[in]  value   matrix to set
    /// @returns this object.
    auto setMat4(const Mat4f &value) -> Uniform &;

    /// Set a 4x4 matrix, if the uniform is of type Mat4
    /// @param[in]  array   array of 16 floats in column-major order
    /// @returns this object.
    auto setMat4(const Float *array) -> Uniform &;

    /// Set a 3x3 matrix, if the uniform is of type Mat3
    /// @param[in]  value   matrix to set
    /// @returns this object.
    auto setMat3(const Mat3f &value) -> Uniform &;

    /// Set a 3x3 matrix, if the uniform is of type Mat3
    /// @param[in]  array   array of 9 floats in column-major order
    /// @returns this object.
    auto setMat3(const Float *array) -> Uniform &;

    /// Get low level handle id
    [[nodiscard]]
    auto handle() const noexcept -> Uint16 { return m_handle; }

    /// Check whether handle id is valid
    [[nodiscard]]
    auto isValid() const noexcept -> Bool { return m_handle != InvalidHandle; }

    [[nodiscard]]
    explicit operator bool() const noexcept { return m_handle != InvalidHandle; }

    /// Get the uniform type
    [[nodiscard]]
    auto type() const noexcept -> UniformType;

    /// Get the uniform name string
    [[nodiscard]]
    auto name() const noexcept -> String;
private:
   Uint16 m_handle;   ///< low level uniform handle id
};

class UniformMgr {
public:
    static constexpr Int MaxTextureSlots = 16;

    UniformMgr();
    ~UniformMgr();

    /// Clear each uniform.
    auto clear() -> void;

    /// Check if there are zero uniforms in the manager.
    auto empty() const noexcept -> Bool;

    /// Number of uniforms in the manager.
    auto size() const noexcept -> Size;

    /// Create a uniform handle.
    /// @note Overwrites any existing entry with the same name.
    /// @param[in]  name   name of the uniform
    /// @param[in]  type   type of the uniform
    /// @returns uniform handle, or `UniformMgr::InvalidHandle` on error.
    auto create(StringView name, UniformType type) -> Uniform;

    /// Get a uniform handle that was previously created
    /// @param[in] name  name of the uniform
    /// @returns uniform handle, or `UniformMgr::InvalidHandle` on error.
    auto getUniform(StringView name) const -> Uniform;

    /// Set the current texture
    /// @param[in]  slot     texture slot (0 - 15)
    /// @param[in]  texture  texture to send to the graphics card
    auto setTexture(Int slot, const Texture2D &texture) const -> void;
private:
    struct Impl;
    Impl *m;
};

KAZE_NAMESPACE_END

#endif
