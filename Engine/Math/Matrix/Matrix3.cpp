#include "Matrix3.hpp"
#include "Math.hpp"

namespace Math
{
    // ... (Matrix3(), Matrix3(f32...), Identity(), Zero(), Determinant(), Transposed() — без изменений)

    Matrix3 Matrix3::Inversed() const noexcept
    {
        const f32 DeterminantValue =
            Determinant();

        if (Math::IsZero(DeterminantValue))
        {
            return Zero();
        }

        const f32 InverseDeterminant =
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

    // ... (остальное без изменений — operator*, operator+, operator-, operator+=, operator-=, operator*=, operator/=, operator==, operator!=)
}