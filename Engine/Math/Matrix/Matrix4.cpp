#include "Matrix4.hpp"

#include <cmath>

namespace Math
{
    Matrix4::Matrix4() noexcept
        : M00(1.0f), M01(0.0f), M02(0.0f), M03(0.0f)
        , M10(0.0f), M11(1.0f), M12(0.0f), M13(0.0f)
        , M20(0.0f), M21(0.0f), M22(1.0f), M23(0.0f)
        , M30(0.0f), M31(0.0f), M32(0.0f), M33(1.0f)
    {
    }

    Matrix4::Matrix4(
        f32 M00, f32 M01, f32 M02, f32 M03,
        f32 M10, f32 M11, f32 M12, f32 M13,
        f32 M20, f32 M21, f32 M22, f32 M23,
        f32 M30, f32 M31, f32 M32, f32 M33
    ) noexcept
        : M00(M00), M01(M01), M02(M02), M03(M03)
        , M10(M10), M11(M11), M12(M12), M13(M13)
        , M20(M20), M21(M21), M22(M22), M23(M23)
        , M30(M30), M31(M31), M32(M32), M33(M33)
    {
    }

    Matrix4 Matrix4::Identity() noexcept
    {
        return Matrix4();
    }

    Matrix4 Matrix4::Zero() noexcept
    {
        return {
            0.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f
        };
    }

    Matrix4 Matrix4::Translation(
        const Vector3& Position
    ) noexcept
    {
        return {
            1.0f, 0.0f, 0.0f, Position.X,
            0.0f, 1.0f, 0.0f, Position.Y,
            0.0f, 0.0f, 1.0f, Position.Z,
            0.0f, 0.0f, 0.0f, 1.0f
        };
    }

    Matrix4 Matrix4::Scale(
        const Vector3& Scale
    ) noexcept
    {
        return {
            Scale.X, 0.0f, 0.0f, 0.0f,
            0.0f, Scale.Y, 0.0f, 0.0f,
            0.0f, 0.0f, Scale.Z, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        };
    }

    Matrix4 Matrix4::RotationX(
        f32 Angle
    ) noexcept
    {
        const f32 Cosine = std::cos(Angle);
        const f32 Sine = std::sin(Angle);

        return {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, Cosine, -Sine, 0.0f,
            0.0f, Sine, Cosine, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        };
    }

    Matrix4 Matrix4::RotationY(
        f32 Angle
    ) noexcept
    {
        const f32 Cosine = std::cos(Angle);
        const f32 Sine = std::sin(Angle);

        return {
            Cosine, 0.0f, Sine, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            -Sine, 0.0f, Cosine, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        };
    }

    Matrix4 Matrix4::RotationZ(
        f32 Angle
    ) noexcept
    {
        const f32 Cosine = std::cos(Angle);
        const f32 Sine = std::sin(Angle);

        return {
            Cosine, -Sine, 0.0f, 0.0f,
            Sine, Cosine, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        };
    }

    Matrix4 Matrix4::Rotation(
        const Vector3& Rotation
    ) noexcept
    {
        return
            RotationZ(Rotation.Z) *
            RotationY(Rotation.Y) *
            RotationX(Rotation.X);
    }

    Matrix4 Matrix4::Perspective(
    f32 FieldOfView,
    f32 AspectRatio,
    f32 NearPlane,
    f32 FarPlane
) noexcept
{
    const f32 HalfFieldOfView =
        FieldOfView * 0.5f;

    const f32 FocalLength =
        1.0f / std::tan(HalfFieldOfView);

    return {
        FocalLength / AspectRatio, 0.0f, 0.0f, 0.0f,

        0.0f, FocalLength, 0.0f, 0.0f,

        0.0f,
        0.0f,
        (FarPlane + NearPlane) /
            (NearPlane - FarPlane),
        (2.0f * FarPlane * NearPlane) /
            (NearPlane - FarPlane),

        0.0f,
        0.0f,
        -1.0f,
        0.0f
    };
}

    Matrix4 Matrix4::Orthographic(
        f32 Left,
        f32 Right,
        f32 Bottom,
        f32 Top,
        f32 NearPlane,
        f32 FarPlane
    ) noexcept
    {
        return {
            2.0f / (Right - Left),
            0.0f,
            0.0f,
            -(Right + Left) / (Right - Left),

            0.0f,
            2.0f / (Top - Bottom),
            0.0f,
            -(Top + Bottom) / (Top - Bottom),

            0.0f,
            0.0f,
            -2.0f / (FarPlane - NearPlane),
            -(FarPlane + NearPlane) /
                (FarPlane - NearPlane),

            0.0f,
            0.0f,
            0.0f,
            1.0f
        };
    }

    Matrix4 Matrix4::LookAt(
        const Vector3& Eye,
        const Vector3& Target,
        const Vector3& Up
    ) noexcept
    {
        const Vector3 Forward =
            (Target - Eye).Normalized();

        const Vector3 Right =
            Forward.Cross(Up).Normalized();

        const Vector3 CameraUp =
            Right.Cross(Forward);

        return {
            Right.X,
            Right.Y,
            Right.Z,
            -Right.Dot(Eye),

            CameraUp.X,
            CameraUp.Y,
            CameraUp.Z,
            -CameraUp.Dot(Eye),

            -Forward.X,
            -Forward.Y,
            -Forward.Z,
            Forward.Dot(Eye),

            0.0f,
            0.0f,
            0.0f,
            1.0f
        };
    }

    f32 Matrix4::Determinant() const noexcept
    {
        const f32 A =
            M22 * M33 -
            M23 * M32;

        const f32 B =
            M21 * M33 -
            M23 * M31;

        const f32 C =
            M21 * M32 -
            M22 * M31;

        const f32 D =
            M20 * M33 -
            M23 * M30;

        const f32 E =
            M20 * M32 -
            M22 * M30;

        const f32 F =
            M20 * M31 -
            M21 * M30;

        return
            M00 * (
                M11 * A -
                M12 * B +
                M13 * C
            )
            - M01 * (
                M10 * A -
                M12 * D +
                M13 * E
            )
            + M02 * (
                M10 * B -
                M11 * D +
                M13 * F
            )
            - M03 * (
                M10 * C -
                M11 * E +
                M12 * F
            );
    }

    Matrix4 Matrix4::Transposed() const noexcept
    {
        return {
            M00, M10, M20, M30,
            M01, M11, M21, M31,
            M02, M12, M22, M32,
            M03, M13, M23, M33
        };
    }

    Matrix4 Matrix4::Inversed() const noexcept
    {
        f32 Matrix[4][8] = {
            {
                M00, M01, M02, M03,
                1.0f, 0.0f, 0.0f, 0.0f
            },
            {
                M10, M11, M12, M13,
                0.0f, 1.0f, 0.0f, 0.0f
            },
            {
                M20, M21, M22, M23,
                0.0f, 0.0f, 1.0f, 0.0f
            },
            {
                M30, M31, M32, M33,
                0.0f, 0.0f, 0.0f, 1.0f
            }
        };

        constexpr f32 Epsilon = 0.000001f;

        for (i32 Column = 0; Column < 4; ++Column)
        {
            i32 PivotRow = Column;

            for (
                i32 Row = Column + 1;
                Row < 4;
                ++Row
            )
            {
                if (
                    std::fabs(Matrix[Row][Column]) >
                    std::fabs(Matrix[PivotRow][Column])
                )
                {
                    PivotRow = Row;
                }
            }

            if (
                std::fabs(
                    Matrix[PivotRow][Column]
                ) < Epsilon
            )
            {
                return Zero();
            }

            if (PivotRow != Column)
            {
                for (i32 Index = 0; Index < 8; ++Index)
                {
                    const f32 Temporary =
                        Matrix[Column][Index];

                    Matrix[Column][Index] =
                        Matrix[PivotRow][Index];

                    Matrix[PivotRow][Index] =
                        Temporary;
                }
            }

            const f32 Pivot =
                Matrix[Column][Column];

            for (i32 Index = 0; Index < 8; ++Index)
            {
                Matrix[Column][Index] /= Pivot;
            }

            for (i32 Row = 0; Row < 4; ++Row)
            {
                if (Row == Column)
                {
                    continue;
                }

                const f32 Factor =
                    Matrix[Row][Column];

                for (i32 Index = 0; Index < 8; ++Index)
                {
                    Matrix[Row][Index] -=
                        Factor * Matrix[Column][Index];
                }
            }
        }

        return {
            Matrix[0][4],
            Matrix[0][5],
            Matrix[0][6],
            Matrix[0][7],

            Matrix[1][4],
            Matrix[1][5],
            Matrix[1][6],
            Matrix[1][7],

            Matrix[2][4],
            Matrix[2][5],
            Matrix[2][6],
            Matrix[2][7],

            Matrix[3][4],
            Matrix[3][5],
            Matrix[3][6],
            Matrix[3][7]
        };
    }

    Vector4 Matrix4::operator*(
        const Vector4& Vector
    ) const noexcept
    {
        return {
            M00 * Vector.X +
            M01 * Vector.Y +
            M02 * Vector.Z +
            M03 * Vector.W,

            M10 * Vector.X +
            M11 * Vector.Y +
            M12 * Vector.Z +
            M13 * Vector.W,

            M20 * Vector.X +
            M21 * Vector.Y +
            M22 * Vector.Z +
            M23 * Vector.W,

            M30 * Vector.X +
            M31 * Vector.Y +
            M32 * Vector.Z +
            M33 * Vector.W
        };
    }

    Matrix4 Matrix4::operator*(
        const Matrix4& Other
    ) const noexcept
    {
        return {
            M00 * Other.M00 +
            M01 * Other.M10 +
            M02 * Other.M20 +
            M03 * Other.M30,

            M00 * Other.M01 +
            M01 * Other.M11 +
            M02 * Other.M21 +
            M03 * Other.M31,

            M00 * Other.M02 +
            M01 * Other.M12 +
            M02 * Other.M22 +
            M03 * Other.M32,

            M00 * Other.M03 +
            M01 * Other.M13 +
            M02 * Other.M23 +
            M03 * Other.M33,

            M10 * Other.M00 +
            M11 * Other.M10 +
            M12 * Other.M20 +
            M13 * Other.M30,

            M10 * Other.M01 +
            M11 * Other.M11 +
            M12 * Other.M21 +
            M13 * Other.M31,

            M10 * Other.M02 +
            M11 * Other.M12 +
            M12 * Other.M22 +
            M13 * Other.M32,

            M10 * Other.M03 +
            M11 * Other.M13 +
            M12 * Other.M23 +
            M13 * Other.M33,

            M20 * Other.M00 +
            M21 * Other.M10 +
            M22 * Other.M20 +
            M23 * Other.M30,

            M20 * Other.M01 +
            M21 * Other.M11 +
            M22 * Other.M21 +
            M23 * Other.M31,

            M20 * Other.M02 +
            M21 * Other.M12 +
            M22 * Other.M22 +
            M23 * Other.M32,

            M20 * Other.M03 +
            M21 * Other.M13 +
            M22 * Other.M23 +
            M23 * Other.M33,

            M30 * Other.M00 +
            M31 * Other.M10 +
            M32 * Other.M20 +
            M33 * Other.M30,

            M30 * Other.M01 +
            M31 * Other.M11 +
            M32 * Other.M21 +
            M33 * Other.M31,

            M30 * Other.M02 +
            M31 * Other.M12 +
            M32 * Other.M22 +
            M33 * Other.M32,

            M30 * Other.M03 +
            M31 * Other.M13 +
            M32 * Other.M23 +
            M33 * Other.M33
        };
    }

    Matrix4 Matrix4::operator+(
        const Matrix4& Other
    ) const noexcept
    {
        return {
            M00 + Other.M00,
            M01 + Other.M01,
            M02 + Other.M02,
            M03 + Other.M03,

            M10 + Other.M10,
            M11 + Other.M11,
            M12 + Other.M12,
            M13 + Other.M13,

            M20 + Other.M20,
            M21 + Other.M21,
            M22 + Other.M22,
            M23 + Other.M23,

            M30 + Other.M30,
            M31 + Other.M31,
            M32 + Other.M32,
            M33 + Other.M33
        };
    }

    Matrix4 Matrix4::operator-(
        const Matrix4& Other
    ) const noexcept
    {
        return {
            M00 - Other.M00,
            M01 - Other.M01,
            M02 - Other.M02,
            M03 - Other.M03,

            M10 - Other.M10,
            M11 - Other.M11,
            M12 - Other.M12,
            M13 - Other.M13,

            M20 - Other.M20,
            M21 - Other.M21,
            M22 - Other.M22,
            M23 - Other.M23,

            M30 - Other.M30,
            M31 - Other.M31,
            M32 - Other.M32,
            M33 - Other.M33
        };
    }

    Matrix4 Matrix4::operator*(
        f32 Scalar
    ) const noexcept
    {
        return {
            M00 * Scalar,
            M01 * Scalar,
            M02 * Scalar,
            M03 * Scalar,

            M10 * Scalar,
            M11 * Scalar,
            M12 * Scalar,
            M13 * Scalar,

            M20 * Scalar,
            M21 * Scalar,
            M22 * Scalar,
            M23 * Scalar,

            M30 * Scalar,
            M31 * Scalar,
            M32 * Scalar,
            M33 * Scalar
        };
    }

    Matrix4 Matrix4::operator/(
        f32 Scalar
    ) const noexcept
    {
        return {
            M00 / Scalar,
            M01 / Scalar,
            M02 / Scalar,
            M03 / Scalar,

            M10 / Scalar,
            M11 / Scalar,
            M12 / Scalar,
            M13 / Scalar,

            M20 / Scalar,
            M21 / Scalar,
            M22 / Scalar,
            M23 / Scalar,

            M30 / Scalar,
            M31 / Scalar,
            M32 / Scalar,
            M33 / Scalar
        };
    }

    Matrix4& Matrix4::operator+=(
        const Matrix4& Other
    ) noexcept
    {
        M00 += Other.M00;
        M01 += Other.M01;
        M02 += Other.M02;
        M03 += Other.M03;

        M10 += Other.M10;
        M11 += Other.M11;
        M12 += Other.M12;
        M13 += Other.M13;

        M20 += Other.M20;
        M21 += Other.M21;
        M22 += Other.M22;
        M23 += Other.M23;

        M30 += Other.M30;
        M31 += Other.M31;
        M32 += Other.M32;
        M33 += Other.M33;

        return *this;
    }

    Matrix4& Matrix4::operator-=(
        const Matrix4& Other
    ) noexcept
    {
        M00 -= Other.M00;
        M01 -= Other.M01;
        M02 -= Other.M02;
        M03 -= Other.M03;

        M10 -= Other.M10;
        M11 -= Other.M11;
        M12 -= Other.M12;
        M13 -= Other.M13;

        M20 -= Other.M20;
        M21 -= Other.M21;
        M22 -= Other.M22;
        M23 -= Other.M23;

        M30 -= Other.M30;
        M31 -= Other.M31;
        M32 -= Other.M32;
        M33 -= Other.M33;

        return *this;
    }

    Matrix4& Matrix4::operator*=(
        f32 Scalar
    ) noexcept
    {
        M00 *= Scalar;
        M01 *= Scalar;
        M02 *= Scalar;
        M03 *= Scalar;

        M10 *= Scalar;
        M11 *= Scalar;
        M12 *= Scalar;
        M13 *= Scalar;

        M20 *= Scalar;
        M21 *= Scalar;
        M22 *= Scalar;
        M23 *= Scalar;

        M30 *= Scalar;
        M31 *= Scalar;
        M32 *= Scalar;
        M33 *= Scalar;

        return *this;
    }

    Matrix4& Matrix4::operator/=(
        f32 Scalar
    ) noexcept
    {
        M00 /= Scalar;
        M01 /= Scalar;
        M02 /= Scalar;
        M03 /= Scalar;

        M10 /= Scalar;
        M11 /= Scalar;
        M12 /= Scalar;
        M13 /= Scalar;

        M20 /= Scalar;
        M21 /= Scalar;
        M22 /= Scalar;
        M23 /= Scalar;

        M30 /= Scalar;
        M31 /= Scalar;
        M32 /= Scalar;
        M33 /= Scalar;

        return *this;
    }

    Matrix4& Matrix4::operator*=(
        const Matrix4& Other
    ) noexcept
    {
        *this = *this * Other;

        return *this;
    }

    b8 Matrix4::operator==(
        const Matrix4& Other
    ) const noexcept
    {
        return
            M00 == Other.M00 &&
            M01 == Other.M01 &&
            M02 == Other.M02 &&
            M03 == Other.M03 &&

            M10 == Other.M10 &&
            M11 == Other.M11 &&
            M12 == Other.M12 &&
            M13 == Other.M13 &&

            M20 == Other.M20 &&
            M21 == Other.M21 &&
            M22 == Other.M22 &&
            M23 == Other.M23 &&

            M30 == Other.M30 &&
            M31 == Other.M31 &&
            M32 == Other.M32 &&
            M33 == Other.M33;
    }

    b8 Matrix4::operator!=(
        const Matrix4& Other
    ) const noexcept
    {
        return !(*this == Other);
    }

    Matrix4 operator*(
        f32 Scalar,
        const Matrix4& Matrix
    ) noexcept
    {
        return Matrix * Scalar;
    }
}