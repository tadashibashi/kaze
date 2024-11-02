#pragma once

#include <kaze/core/lib.h>
#include <kaze/core/concepts.h>
#include <kaze/core/math/Vec.hpp>

#include <glm/matrix.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "private/mat4f.h"

KAZE_NS_BEGIN
template <FloatingPoint T, Size Cols, Size Rows>
struct alignas(16) Matrix {
    static_assert(Cols > 0, "Matrix must have an X-axis length greater than 0");
    static_assert(Rows > 0, "Matrix must have an Y-axis length greater than 0");

    static constexpr Size DataLength = Cols * Rows;
    static constexpr Size ColumnLength = Cols;
    static constexpr Size RowLength = Rows;

    constexpr Matrix() noexcept : m_mat(static_cast<T>(1))
    {}

    constexpr Matrix(const Matrix &other) : m_mat(other.m_mat)
    {}

    constexpr auto operator=(const Matrix &other) -> Matrix &
    {
        m_mat = other.m_mat;
        return *this;
    }

    constexpr Matrix(Matrix &&other) noexcept : m_mat(std::move(other.m_mat))
    { }

    constexpr Matrix &operator=(Matrix &&other) noexcept
    {
        if (this == &other) return *this;

        m_mat = std::move(other.m_mat);
        return *this;
    }

    /// assumes the array is Cols * Rows long, sorted with column-priority
    static constexpr auto fromArray(const T array[Cols * Rows]) noexcept -> Matrix
    {
        Matrix mat;
        for (Size col = 0; col < Cols; ++col)
        {
            for (Size row = 0; row < Rows; ++row)
            {
                mat.m_mat[col][row] = array[col * Rows + row];
            }
        }

        return mat;
    }

    [[nodiscard]] static constexpr auto fromOrtho(T left, T right, T top, T bottom) -> Matrix<T, Cols, Rows>
    {
        static_assert(Cols == 4 && Rows == 4, "This function is only available in a 4x4 Matrix");
        Matrix<Float, 4, 4> result;
        mathf::mat4f::ortho(result.data(), left, right, bottom, top, 0, 100.f, 0);

        if constexpr (std::is_same_v<Float, T>)
            return result;
        else
            return (Matrix<T, 4, 4>)result;
    }

    [[nodiscard]] static constexpr auto fromPerspective(T fovy, T aspect, T near, T far) -> Matrix<T, Cols, Rows>
    {
        static_assert(Cols == 4 && Rows == 4, "This function is only available in a 4x4 Matrix");
        Matrix<Float, 4, 4> result;
        mathf::mat4f::proj(result.data(), fovy, aspect, near, far);

        if constexpr (std::is_same_v<Float, T>)
            return result;
        else
            return (Matrix<T, 4, 4>)result;
    }

    [[nodiscard]] constexpr auto isNaN() const noexcept -> bool
    {
        static_assert(std::is_base_of_v<VecBase<T, Rows>, Vec<T, Rows>>, "This function is only available if VecBase was subclassed for this template's vector columns");
        for (Size col = 0; col < Cols; ++col)
        {
            if (m_cols[col].isNaN())
                return true;
        }

        return false;
    }

    // ===== Direct getters ===================================================

    [[nodiscard]] const T *data() const noexcept { return &m_cols[0][0]; }
    [[nodiscard]] T *data() noexcept { return &m_cols[0][0]; }

    /// Get a column in the matrix - no bounds check.
    [[nodiscard]] constexpr Vec<T, Rows> &operator[](const Size column) noexcept
    {
        return m_cols[column];
    }

    /// Get a column in the matrix - no bounds check.
    [[nodiscard]] constexpr const Vec<T, Rows> &operator[](const Size column) const noexcept
    {
        return m_cols[column];
    }

    /// Get a column in the matrix, with bounds checking.
    /// @throws std::out_of_range if index is beyond the number of columns
    [[nodiscard]] constexpr Vec<T, Rows> &at(const Size column)
    {
        if (column >= Cols)
            throw std::out_of_range("column is out of range in Matrix");
        return m_cols[column];
    }

    /// Get a column in the matrix, with bounds checking.
    /// @throws std::out_of_range if index is beyond the number of columns
    [[nodiscard]] constexpr const Vec<T, Rows> &at(const Size column) const
    {
        if (column >= Cols)
            throw std::out_of_range("column is out of range in Matrix");
        return m_cols[column];
    }

    // ===== Transformation ===================================================

    [[nodiscard]] constexpr auto toTranslated(const Vec<T, 2> &v) const -> Matrix<T, Cols, Rows>
    {
        static_assert(Cols == 4 && Rows == 4, "This function is only available in a 4x4 Matrix");
        Matrix result;
        result.m_mat = glm::translate(m_mat, glm::vec<3, T>{
            v.x,
            v.y,
            static_cast<T>(0)
        });
        return result;
    }

    [[nodiscard]] constexpr auto toTranslated(const Vec<T, 3> &v) const -> Matrix<T, Cols, Rows>
    {
        static_assert(Cols == 4 && Rows == 4, "This function is only available in a 4x4 Matrix");
        Matrix result;
        result.m_mat = glm::translate(m_mat, glm::vec<3, T>{
            v.x,
            v.y,
            v.z
        });
        return result;
    }

    constexpr auto translate(const Vec<T, 2> &v) -> Matrix<T, Cols, Rows> &
    {
        static_assert(Cols == 4 && Rows == 4, "This function is only available in a 4x4 Matrix");
        m_mat = glm::translate(m_mat, glm::vec<3, T>{
            v.x,
            v.y,
            static_cast<T>(0)
        });
        return *this;
    }

    constexpr auto translate(const Vec<T, 3> &v) -> Matrix<T, Cols, Rows> &
    {
        static_assert(Cols == 4 && Rows == 4, "This function is only available in a 4x4 Matrix");
        m_mat = glm::translate(m_mat, glm::vec<3, T>{
            v.x,
            v.y,
            v.z
        });
        return *this;
    }

    constexpr auto toInverse() const -> Matrix
    {
        static_assert(Cols == 4 && Rows == 4, "This function is only available in a 4x4 Matrix");
        Matrix result;
        result.m_mat = glm::inverse(m_mat);
        return result;
    }

    constexpr auto inverse() -> Matrix<T, Cols, Rows> &
    {
        static_assert(Cols == 4 && Rows == 4, "This function is only available in a 4x4 Matrix");
        m_mat = glm::inverse(m_mat);
        return *this;
    }

    constexpr auto toScaled(const Vec<T, 3> &v) const -> Matrix<T, Cols, Rows>
    {
        static_assert(Cols == 4 && Rows == 4, "This function is only available in a 4x4 Matrix");
        Matrix result;
        result.m_mat = glm::scale(m_mat, glm::vec<3, T>{
            v.x,
            v.y,
            v.z
        });

        return result;
    }

    constexpr auto scale(const Vec<T, 3> &v) -> Matrix<T, Cols, Rows> &
    {
        static_assert(Cols == 4 && Rows == 4, "This function is only available in a 4x4 Matrix");
        m_mat = glm::scale(m_mat, glm::vec<3, T>{
            v.x,
            v.y,
            v.z
        });

        return *this;
    }

    constexpr auto toScaled(const Vec<T, 2> &v) const -> Matrix<T, Cols, Rows>
    {
        static_assert(Cols == 4 && Rows == 4, "This function is only available in a 4x4 Matrix");
        Matrix result;
        result.m_mat = glm::scale(m_mat, glm::vec<3, T>{
            v.x,
            v.y,
            static_cast<T>(1)
        });

        return result;
    }

    constexpr auto scale(const Vec<T, 2> &v) -> Matrix<T, Cols, Rows> &
    {
        static_assert(Cols == 4 && Rows == 4, "This function is only available in a 4x4 Matrix");
        m_mat = glm::scale(m_mat, glm::vec<3, T>{
            v.x,
            v.y,
            static_cast<T>(1)
        });

        return *this;
    }

    constexpr auto toRotated(T angle, const Vec<T, 3> &axis) const -> Matrix<T, Cols, Rows>
    {
        static_assert(Cols == 4 && Rows == 4, "This function is only available in a 4x4 Matrix");
        Matrix result;
        result.m_mat = glm::rotate(m_mat, angle, glm::vec<3, T>{
            axis.x,
            axis.y,
            axis.z
        });

        return result;
    }

    constexpr auto rotate(T angle, const Vec<T, 3> &v) -> Matrix<T, Cols, Rows> &
    {
        static_assert(Cols == 4 && Rows == 4, "This function is only available in a 4x4 Matrix");
        m_mat = glm::rotate(m_mat, angle, glm::vec<3, T>{
            v.x,
            v.y,
            v.z
        });

        return *this;
    }

    // ===== Equality operators ===============================================

    [[nodiscard]] bool operator==(const Matrix<T, Cols, Rows> &other) const noexcept
    {
        return m_mat == other.m_mat;
    }

    [[nodiscard]] bool operator!=(const Matrix<T, Cols, Rows> &other) const noexcept
    {
        return m_mat != other.m_mat;
    }

    // ===== Casting operators ================================================

    template <Arithmetic U>
    [[nodiscard]] constexpr explicit operator Matrix<U, Cols, Rows>() const noexcept
    {
        Matrix<U, Cols, Rows> result;
        for (Size col = 0; col < Cols; ++col)
            result.m_cols[col] = static_cast<Vec<U, Rows>>(m_cols[col]);
        return result;
    }

    // ===== Math operators ===================================================

    template <Arithmetic U>
    [[nodiscard]] constexpr Matrix<U, Cols, Rows> operator +(const Matrix<U, Cols, Rows> &other) const noexcept
    {
        Matrix<U, Cols, Rows> result;
        result.m_mat = m_mat + other.m_mat;
        return result;
    }

    template <Arithmetic U>
    [[nodiscard]] constexpr Matrix<T, Cols, Rows> operator -(const Matrix<U, Cols, Rows> &other) const noexcept
    {
        Matrix<T, Cols, Rows> result;
        result.m_mat = m_mat - other.m_mat;
        return result;
    }

    template <Arithmetic U, Size UCols>
    [[nodiscard]] constexpr auto operator *(const Matrix<U, UCols, Rows> &other) const -> Matrix<T, Rows, UCols>
    {
        Matrix<T, Rows, UCols> result;
        result.m_mat = m_mat * other.m_mat;
        return result;
    }

    template <Arithmetic U>
    [[nodiscard]] constexpr auto operator *(const Vec<U, Rows> &v) const -> Vec<U, Rows>
    {
        glm::vec<Rows, U> temp;
        for (int r = 0; r < Rows; ++r)
            temp[r] = v[r];
        temp = m_mat * temp;

        Vec<U, Rows> result;
        for (int r = 0; r < Rows; ++r)
            result[r] = temp[r];
        return result;
    }

    // ----- Math assignment -----

    template <Arithmetic U>
    constexpr Matrix &operator +=(const Matrix<U, Cols, Rows> &other) noexcept
    {
        m_mat += other.m_mat;
        return *this;
    }

    template <Arithmetic U>
    constexpr Matrix &operator -=(const Matrix<U, Cols, Rows> &other) noexcept
    {
        m_mat -= other.m_mat;
        return *this;
    }

    template <Arithmetic U>
    [[nodiscard]] constexpr auto operator *=(const Matrix<U, Cols, Cols> &other) noexcept -> Matrix &
    {
        m_mat *= other.m_mat;
        return *this;
    }

    // ----- Scalar operators -----
    template <Arithmetic U>
    [[nodiscard]] constexpr Matrix operator *(const U scalar) const noexcept
    {
        Matrix result;
        result.m_mat = m_mat * scalar;
        return result;
    }

    template <Arithmetic U>
    [[nodiscard]] constexpr Matrix operator /(const U scalar) const noexcept
    {
        Matrix result;
        result.m_mat = m_mat / scalar;
        return result;
    }

    template <Arithmetic U>
    constexpr Matrix &operator *=(const U scalar) noexcept
    {
        m_mat *= scalar;
        return *this;
    }

    template <Arithmetic U>
    constexpr Matrix &operator /=(const U scalar) noexcept
    {
        m_mat /= scalar;
        return *this;
    }

private:
    union
    {
        Array< Vec<T, Rows>, Cols > m_cols{};
        glm::mat<Cols, Rows, T> m_mat;
    };

};

template <Arithmetic ST, Arithmetic MT, Size Cols, Size Rows>
constexpr Matrix<MT, Cols, Rows> operator *(ST scalar, const Matrix<MT, Cols, Rows> &mat) noexcept
{
    return mat * scalar;
}

using Mat3f = Matrix<Float, 3, 3>;
using Mat3d = Matrix<Double, 3, 3>;
using Mat4f = Matrix<Float, 4, 4>;
using Mat4d = Matrix<Double, 4, 4>;


KAZE_NS_END
