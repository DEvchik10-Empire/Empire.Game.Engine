#pragma once

#include "Aliases.hpp"
#include "Vector3.hpp"
#include "Vector4.hpp"

using namespace Engine;

namespace Math
{
    struct Matrix4
    {
        f32 M00, M01, M02, M03;
        f32 M10, M11, M12, M13;
        f32 M20, M21, M22, M23;
        f32 M30, M31, M32, M33;

        Matrix4() noexcept;

        Matrix4(
            f32 M00, f32 M01, f32 M02, f32 M03,
            f32 M10, f32 M11, f32 M12, f32 M13,
            f32 M20, f32 M21, f32 M22, f32 M23,
            f32 M30, f32 M31, f32 M32, f32 M33
        ) noexcept;

        static Matrix4 Identity() noexcept;
        static Matrix4 Zero() noexcept;

        static Matrix4 Translation(
            const Vector3& Position
        ) noexcept;

        static Matrix4 Scale(
            const Vector3& Scale
        ) noexcept;

        static Matrix4 RotationX(
            f32 Angle
        ) noexcept;

        static Matrix4 RotationY(
            f32 Angle
        ) noexcept;

        static Matrix4 RotationZ(
            f32 Angle
        ) noexcept;

        static Matrix4 Rotation(
            const Vector3& Rotation
        ) noexcept;

        static Matrix4 Perspective(
            f32 FieldOfView,
            f32 AspectRatio,
            f32 NearPlane,
            f32 FarPlane
        ) noexcept;

        static Matrix4 Orthographic(
            f32 Left,
            f32 Right,
            f32 Bottom,
            f32 Top,
            f32 NearPlane,
            f32 FarPlane
        ) noexcept;

        static Matrix4 LookAt(
            const Vector3& Eye,
            const Vector3& Target,
            const Vector3& Up
        ) noexcept;

        f32 Determinant() const noexcept;

        Matrix4 Transposed() const noexcept;

        Matrix4 Inversed() const noexcept;

        Vector4 operator*(
            const Vector4& Vector
        ) const noexcept;

        Matrix4 operator*(
            const Matrix4& Other
        ) const noexcept;

        Matrix4 operator+(
            const Matrix4& Other
        ) const noexcept;

        Matrix4 operator-(
            const Matrix4& Other
        ) const noexcept;

        Matrix4 operator*(
            f32 Scalar
        ) const noexcept;

        Matrix4 operator/(
            f32 Scalar
        ) const noexcept;

        Matrix4& operator+=(
            const Matrix4& Other
        ) noexcept;

        Matrix4& operator-=(
            const Matrix4& Other
        ) noexcept;

        Matrix4& operator*=(
            f32 Scalar
        ) noexcept;

        Matrix4& operator/=(
            f32 Scalar
        ) noexcept;

        Matrix4& operator*=(
            const Matrix4& Other
        ) noexcept;

        b8 operator==(
            const Matrix4& Other
        ) const noexcept;

        b8 operator!=(
            const Matrix4& Other
        ) const noexcept;
    };

    Matrix4 operator*(
        f32 Scalar,
        const Matrix4& Matrix
    ) noexcept;
}