#include "Matrix2.hpp"
#include "../Empire.Engine/Engine/Math/Math.hpp"

namespace Engine
{
    namespace Math
    {
        using namespace Kernel;
        Matrix2::Matrix2()
            : M00(1.0f), M01(0.0f)
            , M10(0.0f), M11(1.0f)
        {
        }

        Matrix2::Matrix2(Types::f32 M00, Types::f32 M01, Types::f32 M10, Types::f32 M11)
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

        Types::f32 Matrix2::Determinant() const noexcept
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
            const Types::f32 DeterminantValue =
                Determinant();

            if (Math::IsZero(DeterminantValue))
            {
                return Zero();
            }

            const Types::f32 InverseDeterminant =
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

        Matrix2 Matrix2::operator*(Types::f32 Scalar) const noexcept
        {
            return {
                M00 * Scalar,
                M01 * Scalar,

                M10 * Scalar,
                M11 * Scalar
            };
        }

        Matrix2 Matrix2::operator/(Types::f32 Scalar) const noexcept
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

        Matrix2& Matrix2::operator*=(Types::f32 Scalar) noexcept
        {
            M00 *= Scalar;
            M01 *= Scalar;

            M10 *= Scalar;
            M11 *= Scalar;

            return *this;
        }

        Matrix2& Matrix2::operator/=(Types::f32 Scalar) noexcept
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

        Types::b8 Matrix2::operator==(const Matrix2& Other) const noexcept
        {
            return
                M00 == Other.M00 &&
                M01 == Other.M01 &&
                M10 == Other.M10 &&
                M11 == Other.M11;
        }

        Types::b8 Matrix2::operator!=(const Matrix2& Other) const noexcept
        {
            return !(*this == Other);
        }

        Matrix2 operator*(Types::f32 Scalar, const Matrix2& Matrix) noexcept
        {
            return Matrix * Scalar;
        }
    }
}
