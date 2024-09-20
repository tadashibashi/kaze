#pragma once
#ifndef nova_math_matrix_h_
#define nova_math_matrix_h_

#include <nova/nova.h>
#include <nova/concepts.h>
#include <nova/math/Vec.hpp>
#include <glm/matrix.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

NOVA_NAMESPACE_BEGIN
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

    /// assumes the array is Cols * Rows long, sorted with column-priority
    static constexpr Matrix fromArray(const T array[Cols * Rows]) noexcept
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

    [[nodiscard]] static constexpr auto fromOrtho(T left, T right, T top, T bottom) -> std::enable_if_t<Cols==4 && Rows==4, Matrix>
    {
        Matrix result;
        result.m_mat = glm::ortho(left, right, top, bottom);
        return result;
    }

    [[nodiscard]] static constexpr auto fromPerspective(T fovy, T aspect, T near, T far) -> std::enable_if_t<Cols==4 && Rows==4, Matrix>
    {
        Matrix result;
        result.m_mat = glm::perspective(fovy, aspect, near, far);
        return result;
    }

    [[nodiscard]] constexpr auto isNaN() const noexcept -> std::enable_if_t<std::is_base_of_v<VecBase<T, Rows>, Vec<T, Rows>>, bool>
    {
        for (Size col = 0; col < Cols; ++col)
        {
            if (m_cols[col].isNaN())
                return true;
        }

        return false;
    }

    // ===== Direct getters ===================================================

    [[nodiscard]] const T *data() const noexcept { return &m_cols.data()[0]; }
    [[nodiscard]] T *data() noexcept { return &m_cols.data()[0]; }

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

    [[nodiscard]] constexpr auto toTranslated(const Vec<T, 2> &v) const -> std::enable_if_t<Cols == 4 && Rows == 4, Matrix>
    {
        Matrix result;
        result.m_mat = glm::translate(m_mat, glm::vec<3, T>{
            v.x,
            v.y,
            static_cast<T>(0)
        });
        return result;
    }

    [[nodiscard]] constexpr auto toTranslated(const Vec<T, 3> &v) const -> std::enable_if_t<Cols == 4 && Rows == 4, Matrix>
    {
        Matrix result;
        result.m_mat = glm::translate(m_mat, glm::vec<3, T>{
            v.x,
            v.y,
            v.z
        });
        return result;
    }

    constexpr auto translate(const Vec<T, 2> &v) -> std::enable_if_t<Cols == 4 && Rows == 4, Matrix &>
    {
        m_mat = glm::translate(m_mat, glm::vec<3, T>{
            v.x,
            v.y,
            static_cast<T>(0)
        });
        return *this;
    }

    constexpr auto translate(const Vec<T, 3> &v) -> std::enable_if_t<Cols == 4 && Rows == 4, Matrix &>
    {
        m_mat = glm::translate(m_mat, glm::vec<3, T>{
            v.x,
            v.y,
            v.z
        });
        return *this;
    }

    constexpr auto toInverse() const -> std::enable_if_t<Cols == Rows, Matrix>
    {
        Matrix result;
        result.m_mat = glm::inverse(m_mat);
        return result;
    }

    constexpr auto inverse() -> std::enable_if_t<Cols == Rows, Matrix &>
    {
        m_mat = glm::inverse(m_mat);
        return *this;
    }

    constexpr auto toScaled(const Vec<T, 3> &v) const -> std::enable_if_t<Cols == 4 && Rows == 4, Matrix>
    {
        Matrix result;
        result.m_mat = glm::scale(m_mat, glm::vec<3, T>{
            v.x,
            v.y,
            v.z
        });

        return result;
    }

    constexpr auto scale(const Vec<T, 3> &v) const -> std::enable_if_t<Cols == 4 && Rows == 4, Matrix>
    {
        m_mat = glm::scale(m_mat, glm::vec<3, T>{
            v.x,
            v.y,
            v.z
        });

        return *this;
    }

    constexpr auto toScaled(const Vec<T, 2> &v) const -> std::enable_if_t<Cols == 4 && Rows == 4, Matrix>
    {
        Matrix result;
        result.m_mat = glm::scale(m_mat, glm::vec<3, T>{
            v.x,
            v.y,
            static_cast<T>(1)
        });

        return result;
    }

    constexpr auto scale(const Vec<T, 2> &v) const -> std::enable_if_t<Cols == 4 && Rows == 4, Matrix>
    {
        m_mat = glm::scale(m_mat, glm::vec<3, T>{
            v.x,
            v.y,
            static_cast<T>(1)
        });

        return *this;
    }

    constexpr auto toRotated(T angle, const Vec<T, 3> &axis) const -> std::enable_if_t<Cols == 4 && Rows == 4, Matrix>
    {
        Matrix result;
        result.m_mat = glm::rotate(m_mat, angle, glm::vec<3, T>{
            axis.x,
            axis.y,
            axis.z
        });

        return result;
    }

    constexpr auto rotate(T angle, const Vec<T, 3> &v) const -> std::enable_if_t<Cols == 4 && Rows == 4, Matrix>
    {
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
        FixedArray< Vec<T, Rows>, Cols > m_cols{};
        glm::mat<Cols, Rows, T> m_mat;
    };

};

template <Arithmetic ST, Arithmetic MT, Size Cols, Size Rows>
constexpr Matrix<MT, Cols, Rows> operator *(ST scalar, const Matrix<MT, Cols, Rows> &mat) noexcept
{
    return mat * scalar;
}

using Mat4f = Matrix<Float, 4, 4>;
using Mat4d = Matrix<Double, 4, 4>;

NOVA_NAMESPACE_END

#endif // nova_math_matrix_h_
