#pragma once

#include "Empire.Kernel/Kernel.Types/Types.hpp"
#include "Empire.Math/Math.Matrix/Matrix.DepthRange.hpp"
#include "Empire.Math/Math.Vector/Vector.Vector3.hpp"
#include "Empire.Math/Math.Vector/Vector.Vector4.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireMath
        {
            namespace MathMatrix
            {
                using namespace EmpireKernel;
                using MathVector::Vector3;
                using MathVector::Vector4;

                struct Matrix4
                {
                    KernelTypes::Float M00, M01, M02, M03;
                    KernelTypes::Float M10, M11, M12, M13;
                    KernelTypes::Float M20, M21, M22, M23;
                    KernelTypes::Float M30, M31, M32, M33;

                    Matrix4() noexcept;

                    Matrix4(KernelTypes::Float M00, KernelTypes::Float M01, KernelTypes::Float M02, KernelTypes::Float M03, KernelTypes::Float M10, KernelTypes::Float M11, KernelTypes::Float M12, KernelTypes::Float M13, KernelTypes::Float M20, KernelTypes::Float M21, KernelTypes::Float M22, KernelTypes::Float M23, KernelTypes::Float M30, KernelTypes::Float M31, KernelTypes::Float M32, KernelTypes::Float M33) noexcept;

                    static Matrix4 Identity() noexcept;
                    static Matrix4 Zero() noexcept;

                    static Matrix4 Translation(const Vector3& Position) noexcept;

                    static Matrix4 Scale(const Vector3& Scale) noexcept;

                    static Matrix4 RotationX(KernelTypes::Float Angle) noexcept;

                    static Matrix4 RotationY(KernelTypes::Float Angle) noexcept;

                    static Matrix4 RotationZ(KernelTypes::Float Angle) noexcept;

                    static Matrix4 Rotation(const Vector3& Rotation) noexcept;

                    static Matrix4 Perspective(KernelTypes::Float FieldOfView, KernelTypes::Float AspectRatio, KernelTypes::Float NearPlane, KernelTypes::Float FarPlane, DepthRange Depth = DepthRange::ZeroToOne) noexcept;

                    static Matrix4 Orthographic(KernelTypes::Float Left, KernelTypes::Float Right, KernelTypes::Float Bottom, KernelTypes::Float Top, KernelTypes::Float NearPlane, KernelTypes::Float FarPlane, DepthRange Depth = DepthRange::ZeroToOne) noexcept;

                    static Matrix4 LookAt(const Vector3& Eye, const Vector3& Target, const Vector3& Up) noexcept;

                    KernelTypes::Float Determinant() const noexcept;

                    Matrix4 Transposed() const noexcept;

                    Matrix4 Inversed() const noexcept;

                    Vector4 operator*(const Vector4& Vector) const noexcept;

                    Matrix4 operator*(const Matrix4& Other) const noexcept;

                    Matrix4 operator+(const Matrix4& Other) const noexcept;

                    Matrix4 operator-(const Matrix4& Other) const noexcept;

                    Matrix4 operator*(KernelTypes::Float Scalar) const noexcept;

                    Matrix4 operator/(KernelTypes::Float Scalar) const noexcept;

                    Matrix4& operator+=(const Matrix4& Other) noexcept;

                    Matrix4& operator-=(const Matrix4& Other) noexcept;

                    Matrix4& operator*=(KernelTypes::Float Scalar) noexcept;

                    Matrix4& operator/=(KernelTypes::Float Scalar) noexcept;

                    Matrix4& operator*=(const Matrix4& Other) noexcept;

                    KernelTypes::Bool operator==(const Matrix4& Other) const noexcept;

                    KernelTypes::Bool operator!=(const Matrix4& Other) const noexcept;
                };

                Matrix4 operator*(KernelTypes::Float Scalar, const Matrix4& Matrix) noexcept;
            }
        }
    }
}
