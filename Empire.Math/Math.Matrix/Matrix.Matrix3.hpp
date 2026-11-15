#pragma once

#include "Empire.Kernel/Kernel.Types/Types.hpp"
#include "Empire.Math/Math.Vector/Vector.Vector3.hpp"

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

                struct Matrix3
                {
                    KernelTypes::Float M00, M01, M02;
                    KernelTypes::Float M10, M11, M12;
                    KernelTypes::Float M20, M21, M22;

                    Matrix3();

                    Matrix3(KernelTypes::Float M00, KernelTypes::Float M01, KernelTypes::Float M02, KernelTypes::Float M10, KernelTypes::Float M11, KernelTypes::Float M12, KernelTypes::Float M20, KernelTypes::Float M21, KernelTypes::Float M22);

                    static Matrix3 Identity();
                    static Matrix3 Zero();

                    KernelTypes::Float Determinant() const noexcept;

                    Matrix3 Transposed() const noexcept;
                    Matrix3 Inversed() const noexcept;

                    Vector3 operator*(const Vector3& Vector) const noexcept;

                    Matrix3 operator*(const Matrix3& Other) const noexcept;

                    Matrix3 operator+(const Matrix3& Other) const noexcept;

                    Matrix3 operator-(const Matrix3& Other) const noexcept;

                    Matrix3 operator*(KernelTypes::Float Scalar) const noexcept;

                    Matrix3 operator/(KernelTypes::Float Scalar) const noexcept;

                    Matrix3& operator+=(const Matrix3& Other) noexcept;

                    Matrix3& operator-=(const Matrix3& Other) noexcept;

                    Matrix3& operator*=(KernelTypes::Float Scalar) noexcept;

                    Matrix3& operator/=(KernelTypes::Float Scalar) noexcept;

                    Matrix3& operator*=(const Matrix3& Other) noexcept;

                    KernelTypes::Bool operator==(const Matrix3& Other) const noexcept;

                    KernelTypes::Bool operator!=(const Matrix3& Other) const noexcept;
                };

                Matrix3 operator*(KernelTypes::Float Scalar, const Matrix3& Matrix) noexcept;
            }
        }
    }
}
