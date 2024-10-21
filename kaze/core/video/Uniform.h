#pragma once
#ifndef kaze_core_video_uniform_h_
#define kaze_core_video_uniform_h_

#include <kaze/core/lib.h>
#include <kaze/core/math/Matrix.h>
#include <kaze/core/math/Vec/Vec4.h>

KAZE_NAMESPACE_BEGIN

class Color;
class Texture2D;

enum class UniformType {
    Unknown = -1,
    Vec4 = 0,     ///< 4 floats vector.
    Mat3,         ///< 3x3 matrix.
    Mat4,         ///< 4x4 matrix.
};

/// Wrapper around a Uniform id to set its value and get its info
class Uniform {
public:
    static const Uint16 InvalidHandle; ///< Invalid handle sentry constant

    /// \param[in]  handle   handle to emplace [optional, default: `InvalidHandle`]
    explicit Uniform(const Uint16 handle = InvalidHandle) : m_handle(handle) { }

    // ----- Setters ----------------------------------------------------------

    auto operator= (const Color &value) -> Uniform &;
    auto operator= (const Vec4f &value) -> Uniform &;
    auto operator= (const Mat3f &value) -> Uniform &;
    auto operator= (const Mat4f &value) -> Uniform &;

    /// Set a color to normalized Vec4, if the uniform is of type Vec4
    /// \param[in]  value   the color to set
    /// \returns this object.
    auto setColor(const Color &value) -> Uniform &;

    /// Set a Vec4, if the uniform is of type Vec4
    /// \param[in]  value   the vector to set
    /// \returns this object.
    auto setVec4(const Vec4f &value) -> Uniform &;

    /// Set a 4x4 matrix, if the uniform is of type Mat4
    /// \param[in]  value   matrix to set
    /// \returns this object.
    auto setMat4(const Mat4f &value) -> Uniform &;

    /// Set a 4x4 matrix, if the uniform is of type Mat4
    /// \param[in]  array   array of 16 floats in column-major order
    /// \returns this object.
    auto setMat4(const Float *array) -> Uniform &;

    /// Set a 3x3 matrix, if the uniform is of type Mat3
    /// \param[in]  value   matrix to set
    /// \returns this object.
    auto setMat3(const Mat3f &value) -> Uniform &;

    /// Set a 3x3 matrix, if the uniform is of type Mat3
    /// \param[in]  array   array of 9 floats in column-major order
    /// \returns this object.
    auto setMat3(const Float *array) -> Uniform &;

    // ----- Getters ----------------------------------------------------------

    /// Get the uniform type
    [[nodiscard]]
    auto type() const noexcept -> UniformType;

    /// Get the uniform name string
    [[nodiscard]]
    auto name() const noexcept -> String;

    /// Get low level handle id
    [[nodiscard]]
    auto handle() const noexcept -> Uint16 { return m_handle; }

    /// Check whether handle id is valid
    [[nodiscard]]
    auto isValid() const noexcept -> Bool { return m_handle != InvalidHandle; }

    [[nodiscard]]
    explicit operator bool() const noexcept { return m_handle != InvalidHandle; }
private:
   Uint16 m_handle;   ///< low level uniform handle id
};

KAZE_NAMESPACE_END

#endif // kaze_core_video_uniform_h_
