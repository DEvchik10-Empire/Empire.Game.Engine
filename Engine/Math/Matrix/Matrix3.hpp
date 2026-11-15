#pragma once

#include "Aliases.hpp"
#include "Vector3.hpp"

using namespace Engine;

namespace Math
{
    struct Matrix3
    {
        f32 M00, M01, M02;
        f32 M10, M11, M12;
        f32 M20, M21, M22;

        Matrix3();

        Matrix3(
            f32 M00, f32 M01, f32 M02,
            f32 M10, f32 M11, f32 M12,
            f32 M20, f32 M21, f32 M22
        );

        static Matrix3 Identity();
        static Matrix3 Zero();

        f32 Determinant() const noexcept;

        Matrix3 Transposed() const noexcept;
        Matrix3 Inversed() const noexcept;

        Vector3 operator*(
            const Vector3& Vector
        ) const noexcept;

        Matrix3 operator*(
            const Matrix3& Other
        ) const noexcept;

        Matrix3 operator+(
            const Matrix3& Other
        ) const noexcept;

        Matrix3 operator-(
            const Matrix3& Other
        ) const noexcept;

        Matrix3 operator*(
            f32 Scalar
        ) const noexcept;

        Matrix3 operator/(
            f32 Scalar
        ) const noexcept;

        Matrix3& operator+=(
            const Matrix3& Other
        ) noexcept;

        Matrix3& operator-=(
            const Matrix3& Other
        ) noexcept;

        Matrix3& operator*=(
            f32 Scalar
        ) noexcept;

        Matrix3& operator/=(
            f32 Scalar
        ) noexcept;

        Matrix3& operator*=(
            const Matrix3& Other
        ) noexcept;

        b8 operator==(
            const Matrix3& Other
        ) const noexcept;

        b8 operator!=(
            const Matrix3& Other
        ) const noexcept;
    };

    Matrix3 operator*(
        f32 Scalar,
        const Matrix3& Matrix
    ) noexcept;
}