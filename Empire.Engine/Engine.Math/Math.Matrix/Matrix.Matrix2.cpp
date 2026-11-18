#include "Empire.Engine/Engine.Math/Math.Matrix/Matrix.Matrix2.hpp"
#include "Empire.Engine/Engine.Math/Math.Scalar/Scalar.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireEngine
        {
            namespace EngineMath
            {
                namespace MathMatrix
                {
                    using namespace EngineKernel;

                    Matrix2::Matrix2()
                        : M00(1.0f), M01(0.0f)
                        , M10(0.0f), M11(1.0f)
                    {
                    }

                    Matrix2::Matrix2(KernelTypes::Float M00, KernelTypes::Float M01, KernelTypes::Float M10, KernelTypes::Float M11)
                        : M00(M00), M01(M01)
                        , M10(M10), M11(M11)
                    {
                    }

                    Matrix2 Matrix2::Identity()
                    {
                        return Matrix2();
                    }

                    Matrix2 Matrix2::Zero()
                    {
                        return {
                            0.0f, 0.0f,
                            0.0f, 0.0f
                        };
                    }

                    KernelTypes::Float Matrix2::Determinant() const noexcept
                    {
                        return
                            M00 * M11 -
                            M01 * M10;
                    }

                    Matrix2 Matrix2::Transposed() const noexcept
                    {
                        return {
                            M00, M10,
                            M01, M11
                        };
                    }

                    Matrix2 Matrix2::Inversed() const noexcept
                    {
                        const KernelTypes::Float DeterminantValue =
                            Determinant();

                        if (MathScalar::IsZero(DeterminantValue))
                        {
                            return Zero();
                        }

                        const KernelTypes::Float InverseDeterminant =
                            1.0f / DeterminantValue;

                        return {
                             M11 * InverseDeterminant,
                            -M01 * InverseDeterminant,

                            -M10 * InverseDeterminant,
                             M00 * InverseDeterminant
                        };
                    }

                    Vector2 Matrix2::operator*(const Vector2& Vector) const noexcept
                    {
                        return {
                            M00 * Vector.X +
                            M01 * Vector.Y,

                            M10 * Vector.X +
                            M11 * Vector.Y
                        };
                    }

                    Matrix2 Matrix2::operator*(const Matrix2& Other) const noexcept
                    {
                        return {
                            M00 * Other.M00 +
                            M01 * Other.M10,

                            M00 * Other.M01 +
                            M01 * Other.M11,

                            M10 * Other.M00 +
                            M11 * Other.M10,

                            M10 * Other.M01 +
                            M11 * Other.M11
                        };
                    }

                    Matrix2 Matrix2::operator+(const Matrix2& Other) const noexcept
                    {
                        return {
                            M00 + Other.M00,
                            M01 + Other.M01,

                            M10 + Other.M10,
                            M11 + Other.M11
                        };
                    }

                    Matrix2 Matrix2::operator-(const Matrix2& Other) const noexcept
                    {
                        return {
                            M00 - Other.M00,
                            M01 - Other.M01,

                            M10 - Other.M10,
                            M11 - Other.M11
                        };
                    }

                    Matrix2 Matrix2::operator*(KernelTypes::Float Scalar) const noexcept
                    {
                        return {
                            M00 * Scalar,
                            M01 * Scalar,

                            M10 * Scalar,
                            M11 * Scalar
                        };
                    }

                    Matrix2 Matrix2::operator/(KernelTypes::Float Scalar) const noexcept
                    {
                        return {
                            M00 / Scalar,
                            M01 / Scalar,

                            M10 / Scalar,
                            M11 / Scalar
                        };
                    }

                    Matrix2& Matrix2::operator+=(const Matrix2& Other) noexcept
                    {
                        M00 += Other.M00;
                        M01 += Other.M01;

                        M10 += Other.M10;
                        M11 += Other.M11;

                        return *this;
                    }

                    Matrix2& Matrix2::operator-=(const Matrix2& Other) noexcept
                    {
                        M00 -= Other.M00;
                        M01 -= Other.M01;

                        M10 -= Other.M10;
                        M11 -= Other.M11;

                        return *this;
                    }

                    Matrix2& Matrix2::operator*=(KernelTypes::Float Scalar) noexcept
                    {
                        M00 *= Scalar;
                        M01 *= Scalar;

                        M10 *= Scalar;
                        M11 *= Scalar;

                        return *this;
                    }

                    Matrix2& Matrix2::operator/=(KernelTypes::Float Scalar) noexcept
                    {
                        M00 /= Scalar;
                        M01 /= Scalar;

                        M10 /= Scalar;
                        M11 /= Scalar;

                        return *this;
                    }

                    Matrix2& Matrix2::operator*=(const Matrix2& Other) noexcept
                    {
                        *this = *this * Other;

                        return *this;
                    }

                    KernelTypes::Bool Matrix2::operator==(const Matrix2& Other) const noexcept
                    {
                        return
                            M00 == Other.M00 &&
                            M01 == Other.M01 &&
                            M10 == Other.M10 &&
                            M11 == Other.M11;
                    }

                    KernelTypes::Bool Matrix2::operator!=(const Matrix2& Other) const noexcept
                    {
                        return !(*this == Other);
                    }

                    Matrix2 operator*(KernelTypes::Float Scalar, const Matrix2& Matrix) noexcept
                    {
                        return Matrix * Scalar;
                    }
                }
            }
        }
    }
}
