#pragma once

#include "../../../Kernel/Types.hpp"
#include "../Empire.Engine/Engine/Math//Vector/Vector3.hpp"

namespace Engine
{
    namespace Math
    {
        using namespace Kernel;

        struct Matrix3
        {
            Types::f32 M00, M01, M02;
            Types::f32 M10, M11, M12;
            Types::f32 M20, M21, M22;

            Matrix3();

            Matrix3(Types::f32 M00, Types::f32 M01, Types::f32 M02, Types::f32 M10, Types::f32 M11, Types::f32 M12, Types::f32 M20, Types::f32 M21, Types::f32 M22);

            static Matrix3 Identity();
            static Matrix3 Zero();

            Types::f32 Determinant() const noexcept;

            Matrix3 Transposed() const noexcept;
            Matrix3 Inversed() const noexcept;

            Vector3 operator*(const Vector3& Vector) const noexcept;

            Matrix3 operator*(const Matrix3& Other) const noexcept;

            Matrix3 operator+(const Matrix3& Other) const noexcept;

            Matrix3 operator-(const Matrix3& Other) const noexcept;

            Matrix3 operator*(Types::f32 Scalar) const noexcept;

            Matrix3 operator/(Types::f32 Scalar) const noexcept;

            Matrix3& operator+=(const Matrix3& Other) noexcept;

            Matrix3& operator-=(const Matrix3& Other) noexcept;

            Matrix3& operator*=(Types::f32 Scalar) noexcept;

            Matrix3& operator/=(Types::f32 Scalar) noexcept;

            Matrix3& operator*=(const Matrix3& Other) noexcept;

            Types::b8 operator==(const Matrix3& Other) const noexcept;

            Types::b8 operator!=(const Matrix3& Other) const noexcept;
        };

        Matrix3 operator*(Types::f32 Scalar, const Matrix3& Matrix) noexcept;
    }
}
