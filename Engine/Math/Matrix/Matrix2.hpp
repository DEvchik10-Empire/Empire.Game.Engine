#pragma once

#include "../../../Kernel/Types.hpp"
#include "../Empire.Engine/Engine/Math/Vector/Vector2.hpp"

namespace Engine
{
    namespace Math
    {
        using namespace Kernel;

        struct Matrix2
        {
            Types::f32 M00, M01;
            Types::f32 M10, M11;

            Matrix2();

            Matrix2(Types::f32 M00, Types::f32 M01, Types::f32 M10, Types::f32 M11);

            static Matrix2 Identity();
            static Matrix2 Zero();

            Types::f32 Determinant() const noexcept;

            Matrix2 Transposed() const noexcept;
            Matrix2 Inversed() const noexcept;

            Vector2 operator*(const Vector2& Vector) const noexcept;

            Matrix2 operator*(const Matrix2& Other) const noexcept;

            Matrix2 operator+(const Matrix2& Other) const noexcept;

            Matrix2 operator-(const Matrix2& Other) const noexcept;

            Matrix2 operator*(Types::f32 Scalar) const noexcept;

            Matrix2 operator/(Types::f32 Scalar) const noexcept;

            Matrix2& operator+=(const Matrix2& Other) noexcept;

            Matrix2& operator-=(const Matrix2& Other) noexcept;

            Matrix2& operator*=(Types::f32 Scalar) noexcept;

            Matrix2& operator/=(Types::f32 Scalar) noexcept;

            Matrix2& operator*=(const Matrix2& Other) noexcept;

            Types::b8 operator==(const Matrix2& Other) const noexcept;

            Types::b8 operator!=(const Matrix2& Other) const noexcept;
        };

        Matrix2 operator*(Types::f32 Scalar, const Matrix2& Matrix) noexcept;
    }
}
