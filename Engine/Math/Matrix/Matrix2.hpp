#pragma once

#include "Aliases.hpp"
#include "Vector2.hpp"

using namespace Engine;

namespace Math
{
    struct Matrix2
    {
        f32 M00, M01;
        f32 M10, M11;

        Matrix2();

        Matrix2(
            f32 M00, f32 M01,
            f32 M10, f32 M11
        );

        static Matrix2 Identity();
        static Matrix2 Zero();

        f32 Determinant() const noexcept;

        Matrix2 Transposed() const noexcept;
        Matrix2 Inversed() const noexcept;

        Vector2 operator*(
            const Vector2& Vector
        ) const noexcept;

        Matrix2 operator*(
            const Matrix2& Other
        ) const noexcept;

        Matrix2 operator+(
            const Matrix2& Other
        ) const noexcept;

        Matrix2 operator-(
            const Matrix2& Other
        ) const noexcept;

        Matrix2 operator*(
            f32 Scalar
        ) const noexcept;

        Matrix2 operator/(
            f32 Scalar
        ) const noexcept;

        Matrix2& operator+=(
            const Matrix2& Other
        ) noexcept;

        Matrix2& operator-=(
            const Matrix2& Other
        ) noexcept;

        Matrix2& operator*=(
            f32 Scalar
        ) noexcept;

        Matrix2& operator/=(
            f32 Scalar
        ) noexcept;

        Matrix2& operator*=(
            const Matrix2& Other
        ) noexcept;

        b8 operator==(
            const Matrix2& Other
        ) const noexcept;

        b8 operator!=(
            const Matrix2& Other
        ) const noexcept;
    };

    Matrix2 operator*(
        f32 Scalar,
        const Matrix2& Matrix
    ) noexcept;
}