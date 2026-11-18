#include "Empire.Engine/Engine.Math/Math.Matrix/Matrix.Matrix3.hpp"
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

                    Matrix3::Matrix3()
                        : M00(1.0f), M01(0.0f), M02(0.0f)
                        , M10(0.0f), M11(1.0f), M12(0.0f)
                        , M20(0.0f), M21(0.0f), M22(1.0f)
                    {
                    }

                    Matrix3::Matrix3(KernelTypes::Float M00, KernelTypes::Float M01, KernelTypes::Float M02, KernelTypes::Float M10, KernelTypes::Float M11, KernelTypes::Float M12, KernelTypes::Float M20, KernelTypes::Float M21, KernelTypes::Float M22)
                        : M00(M00), M01(M01), M02(M02)
                        , M10(M10), M11(M11), M12(M12)
                        , M20(M20), M21(M21), M22(M22)
                    {
                    }

                    Matrix3 Matrix3::Identity()
                    {
                        return Matrix3();
                    }

                    Matrix3 Matrix3::Zero()
                    {
                        return {
                            0.0f, 0.0f, 0.0f,
                            0.0f, 0.0f, 0.0f,
                            0.0f, 0.0f, 0.0f
                        };
                    }

                    KernelTypes::Float Matrix3::Determinant() const noexcept
                    {
                        return
                            M00 * (M11 * M22 - M12 * M21) -
                            M01 * (M10 * M22 - M12 * M20) +
                            M02 * (M10 * M21 - M11 * M20);
                    }

                    Matrix3 Matrix3::Transposed() const noexcept
                    {
                        return {
                            M00, M10, M20,
                            M01, M11, M21,
                            M02, M12, M22
                        };
                    }

                    Matrix3 Matrix3::Inversed() const noexcept
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
                            (M11 * M22 - M12 * M21) *
                                InverseDeterminant,

                            (M02 * M21 - M01 * M22) *
                                InverseDeterminant,

                            (M01 * M12 - M02 * M11) *
                                InverseDeterminant,

                            (M12 * M20 - M10 * M22) *
                                InverseDeterminant,

                            (M00 * M22 - M02 * M20) *
                                InverseDeterminant,

                            (M02 * M10 - M00 * M12) *
                                InverseDeterminant,

                            (M10 * M21 - M11 * M20) *
                                InverseDeterminant,

                            (M01 * M20 - M00 * M21) *
                                InverseDeterminant,

                            (M00 * M11 - M01 * M10) *
                                InverseDeterminant
                        };
                    }

                    Vector3 Matrix3::operator*(const Vector3& Vector) const noexcept
                    {
                        return {
                            M00 * Vector.X +
                            M01 * Vector.Y +
                            M02 * Vector.Z,

                            M10 * Vector.X +
                            M11 * Vector.Y +
                            M12 * Vector.Z,

                            M20 * Vector.X +
                            M21 * Vector.Y +
                            M22 * Vector.Z
                        };
                    }

                    Matrix3 Matrix3::operator*(const Matrix3& Other) const noexcept
                    {
                        return {
                            M00 * Other.M00 + M01 * Other.M10 + M02 * Other.M20,
                            M00 * Other.M01 + M01 * Other.M11 + M02 * Other.M21,
                            M00 * Other.M02 + M01 * Other.M12 + M02 * Other.M22,

                            M10 * Other.M00 + M11 * Other.M10 + M12 * Other.M20,
                            M10 * Other.M01 + M11 * Other.M11 + M12 * Other.M21,
                            M10 * Other.M02 + M11 * Other.M12 + M12 * Other.M22,

                            M20 * Other.M00 + M21 * Other.M10 + M22 * Other.M20,
                            M20 * Other.M01 + M21 * Other.M11 + M22 * Other.M21,
                            M20 * Other.M02 + M21 * Other.M12 + M22 * Other.M22
                        };
                    }

                    Matrix3 Matrix3::operator+(const Matrix3& Other) const noexcept
                    {
                        return {
                            M00 + Other.M00, M01 + Other.M01, M02 + Other.M02,

                            M10 + Other.M10, M11 + Other.M11, M12 + Other.M12,

                            M20 + Other.M20, M21 + Other.M21, M22 + Other.M22
                        };
                    }

                    Matrix3 Matrix3::operator-(const Matrix3& Other) const noexcept
                    {
                        return {
                            M00 - Other.M00, M01 - Other.M01, M02 - Other.M02,

                            M10 - Other.M10, M11 - Other.M11, M12 - Other.M12,

                            M20 - Other.M20, M21 - Other.M21, M22 - Other.M22
                        };
                    }

                    Matrix3 Matrix3::operator*(KernelTypes::Float Scalar) const noexcept
                    {
                        return {
                            M00 * Scalar, M01 * Scalar, M02 * Scalar,

                            M10 * Scalar, M11 * Scalar, M12 * Scalar,

                            M20 * Scalar, M21 * Scalar, M22 * Scalar
                        };
                    }

                    Matrix3 Matrix3::operator/(KernelTypes::Float Scalar) const noexcept
                    {
                        return {
                            M00 / Scalar, M01 / Scalar, M02 / Scalar,

                            M10 / Scalar, M11 / Scalar, M12 / Scalar,

                            M20 / Scalar, M21 / Scalar, M22 / Scalar
                        };
                    }

                    Matrix3& Matrix3::operator+=(const Matrix3& Other) noexcept
                    {
                        M00 += Other.M00; M01 += Other.M01; M02 += Other.M02;

                        M10 += Other.M10; M11 += Other.M11; M12 += Other.M12;

                        M20 += Other.M20; M21 += Other.M21; M22 += Other.M22;

                        return *this;
                    }

                    Matrix3& Matrix3::operator-=(const Matrix3& Other) noexcept
                    {
                        M00 -= Other.M00; M01 -= Other.M01; M02 -= Other.M02;

                        M10 -= Other.M10; M11 -= Other.M11; M12 -= Other.M12;

                        M20 -= Other.M20; M21 -= Other.M21; M22 -= Other.M22;

                        return *this;
                    }

                    Matrix3& Matrix3::operator*=(KernelTypes::Float Scalar) noexcept
                    {
                        M00 *= Scalar; M01 *= Scalar; M02 *= Scalar;

                        M10 *= Scalar; M11 *= Scalar; M12 *= Scalar;

                        M20 *= Scalar; M21 *= Scalar; M22 *= Scalar;

                        return *this;
                    }

                    Matrix3& Matrix3::operator/=(KernelTypes::Float Scalar) noexcept
                    {
                        M00 /= Scalar; M01 /= Scalar; M02 /= Scalar;

                        M10 /= Scalar; M11 /= Scalar; M12 /= Scalar;

                        M20 /= Scalar; M21 /= Scalar; M22 /= Scalar;

                        return *this;
                    }

                    Matrix3& Matrix3::operator*=(const Matrix3& Other) noexcept
                    {
                        *this = *this * Other;

                        return *this;
                    }

                    KernelTypes::Bool Matrix3::operator==(const Matrix3& Other) const noexcept
                    {
                        return
                            M00 == Other.M00 &&
                            M01 == Other.M01 &&
                            M02 == Other.M02 &&
                            M10 == Other.M10 &&
                            M11 == Other.M11 &&
                            M12 == Other.M12 &&
                            M20 == Other.M20 &&
                            M21 == Other.M21 &&
                            M22 == Other.M22;
                    }

                    KernelTypes::Bool Matrix3::operator!=(const Matrix3& Other) const noexcept
                    {
                        return !(*this == Other);
                    }

                    Matrix3 operator*(KernelTypes::Float Scalar, const Matrix3& Matrix) noexcept
                    {
                        return Matrix * Scalar;
                    }
                }
            }
        }
    }
}
