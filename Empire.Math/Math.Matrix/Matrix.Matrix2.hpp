#pragma once

#include "Empire.Kernel/Kernel.Types/Types.hpp"
#include "Empire.Math/Math.Vector/Vector.Vector2.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireMath
        {
            namespace MathMatrix
            {
                using namespace EmpireKernel;
                using MathVector::Vector2;

                struct Matrix2
                {
                    KernelTypes::Float M00, M01;
                    KernelTypes::Float M10, M11;

                    Matrix2();

                    Matrix2(KernelTypes::Float M00, KernelTypes::Float M01, KernelTypes::Float M10, KernelTypes::Float M11);

                    static Matrix2 Identity();
                    static Matrix2 Zero();

                    KernelTypes::Float Determinant() const noexcept;

                    Matrix2 Transposed() const noexcept;
                    Matrix2 Inversed() const noexcept;

                    Vector2 operator*(const Vector2& Vector) const noexcept;

                    Matrix2 operator*(const Matrix2& Other) const noexcept;

                    Matrix2 operator+(const Matrix2& Other) const noexcept;

                    Matrix2 operator-(const Matrix2& Other) const noexcept;

                    Matrix2 operator*(KernelTypes::Float Scalar) const noexcept;

                    Matrix2 operator/(KernelTypes::Float Scalar) const noexcept;

                    Matrix2& operator+=(const Matrix2& Other) noexcept;

                    Matrix2& operator-=(const Matrix2& Other) noexcept;

                    Matrix2& operator*=(KernelTypes::Float Scalar) noexcept;

                    Matrix2& operator/=(KernelTypes::Float Scalar) noexcept;

                    Matrix2& operator*=(const Matrix2& Other) noexcept;

                    KernelTypes::Bool operator==(const Matrix2& Other) const noexcept;

                    KernelTypes::Bool operator!=(const Matrix2& Other) const noexcept;
                };

                Matrix2 operator*(KernelTypes::Float Scalar, const Matrix2& Matrix) noexcept;
            }
        }
    }
}
