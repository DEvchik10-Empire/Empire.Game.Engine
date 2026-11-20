#include "Empire.Engine/Engine.Math/Math.Matrix/Matrix4.hpp"
#include "Empire.Engine/Engine.Math/Math.Vector/Trigonometry.hpp"

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
                    Matrix4 Matrix4::sNaN() noexcept
                    {
                        const Vector4 NaN = Vector4::sNaN();
                        return {NaN, NaN, NaN, NaN};
                    }

                    Matrix4 Matrix4::sLoadFloat4x4(const Float4* Columns) noexcept
                    {
                        return {
                            Vector4::sLoadFloat4(Columns),
                            Vector4::sLoadFloat4(Columns + 1),
                            Vector4::sLoadFloat4(Columns + 2),
                            Vector4::sLoadFloat4(Columns + 3)
                        };
                    }

                    Matrix4 Matrix4::sLoadFloat4x4Aligned(const Float4* Columns) noexcept
                    {
                        for (KernelTypes::UInt32 Column = 0; Column < 4; ++Column)
                            KernelAssert::DebugCheck(
                                (reinterpret_cast<KernelTypes::UIntPtr>(Columns + Column) & 15u) == 0,
                                "Matrix4 column is 16-byte aligned"
                            );
                        return sLoadFloat4x4(Columns);
                    }

                    Matrix4 Matrix4::sRotationX(KernelTypes::Float Angle) noexcept
                    {
                        const KernelTypes::Float Sine = MathScalar::Sin(Angle);
                        const KernelTypes::Float Cosine = MathScalar::Cos(Angle);
                        return {Vector4(1, 0, 0, 0), Vector4(0, Cosine, Sine, 0),
                            Vector4(0, -Sine, Cosine, 0), Vector4(0, 0, 0, 1)};
                    }

                    Matrix4 Matrix4::sRotationY(KernelTypes::Float Angle) noexcept
                    {
                        const KernelTypes::Float Sine = MathScalar::Sin(Angle);
                        const KernelTypes::Float Cosine = MathScalar::Cos(Angle);
                        return {Vector4(Cosine, 0, -Sine, 0), Vector4(0, 1, 0, 0),
                            Vector4(Sine, 0, Cosine, 0), Vector4(0, 0, 0, 1)};
                    }

                    Matrix4 Matrix4::sRotationZ(KernelTypes::Float Angle) noexcept
                    {
                        const KernelTypes::Float Sine = MathScalar::Sin(Angle);
                        const KernelTypes::Float Cosine = MathScalar::Cos(Angle);
                        return {Vector4(Cosine, Sine, 0, 0), Vector4(-Sine, Cosine, 0, 0),
                            Vector4(0, 0, 1, 0), Vector4(0, 0, 0, 1)};
                    }

                    Matrix4 Matrix4::sTranslation(const Vector3& Translation) noexcept
                    {
                        Matrix4 Result = sIdentity();
                        Result.SetTranslation(Translation);
                        return Result;
                    }

                    Matrix4 Matrix4::sScale(KernelTypes::Float Scale) noexcept
                    {
                        return sScale(Vector3::sReplicate(Scale));
                    }

                    Matrix4 Matrix4::sScale(const Vector3& Scale) noexcept
                    {
                        return {Vector4(Scale.GetX(), 0, 0, 0), Vector4(0, Scale.GetY(), 0, 0),
                            Vector4(0, 0, Scale.GetZ(), 0), Vector4(0, 0, 0, 1)};
                    }

                    Matrix4 Matrix4::sOuterProduct(const Vector3& A, const Vector3& B) noexcept
                    {
                        const Vector4 Extended(A, 0.0f);
                        return {Extended * B.GetX(), Extended * B.GetY(), Extended * B.GetZ(), Vector4(0, 0, 0, 1)};
                    }

                    Matrix4 Matrix4::sCrossProduct(const Vector3& Value) noexcept
                    {
                        const KernelTypes::Float X = Value.GetX();
                        const KernelTypes::Float Y = Value.GetY();
                        const KernelTypes::Float Z = Value.GetZ();
                        return {Vector4(0, Z, -Y, 0), Vector4(-Z, 0, X, 0),
                            Vector4(Y, -X, 0, 0), Vector4(0, 0, 0, 1)};
                    }

                    Matrix4 Matrix4::sLookAt(const Vector3& Position, const Vector3& Target, const Vector3& Up) noexcept
                    {
                        const Vector3 Direction = (Target - Position).NormalizedOr(-Vector3::sAxisZ());
                        const Vector3 Right = Direction.Cross(Up).NormalizedOr(Vector3::sAxisX());
                        const Vector3 ActualUp = Right.Cross(Direction);
                        return Matrix4(Vector4(Right, 0), Vector4(ActualUp, 0), Vector4(-Direction, 0),
                            Vector4(Position, 1)).InversedRotationTranslation();
                    }

                    Matrix4 Matrix4::sPerspective(KernelTypes::Float FovY, KernelTypes::Float Aspect,
                        KernelTypes::Float Near, KernelTypes::Float Far) noexcept
                    {
                        const KernelTypes::Float Height = 1.0f / MathScalar::Tan(0.5f * FovY);
                        const KernelTypes::Float Width = Height / Aspect;
                        const KernelTypes::Float Range = Far / (Near - Far);
                        return {Vector4(Width, 0, 0, 0), Vector4(0, Height, 0, 0),
                            Vector4(0, 0, Range, -1), Vector4(0, 0, Range * Near, 0)};
                    }

                    Matrix4 Matrix4::sOrthographic(KernelTypes::Float Left, KernelTypes::Float Right,
                        KernelTypes::Float Bottom, KernelTypes::Float Top, KernelTypes::Float Near, KernelTypes::Float Far) noexcept
                    {
                        const KernelTypes::Float Width = Right - Left;
                        const KernelTypes::Float Height = Top - Bottom;
                        const KernelTypes::Float Depth = Far - Near;
                        return {Vector4(2.0f / Width, 0, 0, 0), Vector4(0, 2.0f / Height, 0, 0),
                            Vector4(0, 0, -1.0f / Depth, 0),
                            Vector4(-(Right + Left) / Width, -(Top + Bottom) / Height, -Near / Depth, 1)};
                    }

                    KernelTypes::Bool Matrix4::operator==(const Matrix4& Other) const noexcept
                    {
                        for (KernelTypes::UInt32 Column = 0; Column < 4; ++Column)
                            if (mColumns[Column] != Other.mColumns[Column])
                                return false;
                        return true;
                    }

                    KernelTypes::Bool Matrix4::IsClose(const Matrix4& Other, KernelTypes::Float MaxDistanceSquared) const noexcept
                    {
                        for (KernelTypes::UInt32 Column = 0; Column < 4; ++Column)
                            if (!mColumns[Column].IsClose(Other.mColumns[Column], MaxDistanceSquared))
                                return false;
                        return true;
                    }

                    Matrix4 Matrix4::operator*(const Matrix4& Other) const noexcept
                    {
                        Matrix4 Result;
                        for (KernelTypes::UInt32 Column = 0; Column < 4; ++Column)
                            Result.mColumns[Column] = *this * Other.mColumns[Column];
                        return Result;
                    }

                    Vector3 Matrix4::operator*(const Vector3& Value) const noexcept
                    {
                        return Multiply3x3(Value) + GetTranslation();
                    }

                    Vector4 Matrix4::operator*(const Vector4& Value) const noexcept
                    {
                        return mColumns[0] * Value.GetX() + mColumns[1] * Value.GetY()
                            + mColumns[2] * Value.GetZ() + mColumns[3] * Value.GetW();
                    }

                    Vector3 Matrix4::Multiply3x3(const Vector3& Value) const noexcept
                    {
                        return Vector3(
                            mColumns[0].GetX() * Value.GetX() + mColumns[1].GetX() * Value.GetY() + mColumns[2].GetX() * Value.GetZ(),
                            mColumns[0].GetY() * Value.GetX() + mColumns[1].GetY() * Value.GetY() + mColumns[2].GetY() * Value.GetZ(),
                            mColumns[0].GetZ() * Value.GetX() + mColumns[1].GetZ() * Value.GetY() + mColumns[2].GetZ() * Value.GetZ()
                        );
                    }

                    Vector3 Matrix4::Multiply3x3Transposed(const Vector3& Value) const noexcept
                    {
                        return Transposed3x3().Multiply3x3(Value);
                    }

                    Matrix4 Matrix4::Multiply3x3(const Matrix4& Other) const noexcept
                    {
                        Matrix4 Result = sIdentity();
                        for (KernelTypes::UInt32 Column = 0; Column < 3; ++Column)
                        {
                            const Vector3 Product = Multiply3x3(Other.GetColumn3(Column));
                            Result.SetColumn3(Column, Product);
                        }
                        return Result;
                    }

                    Matrix4 Matrix4::Multiply3x3LeftTransposed(const Matrix4& Other) const noexcept
                    {
                        return Transposed3x3().Multiply3x3(Other);
                    }

                    Matrix4 Matrix4::Multiply3x3RightTransposed(const Matrix4& Other) const noexcept
                    {
                        Matrix4 Result = sIdentity();
                        for (KernelTypes::UInt32 Column = 0; Column < 3; ++Column)
                            for (KernelTypes::UInt32 Row = 0; Row < 3; ++Row)
                            {
                                KernelTypes::Float Value = 0.0f;
                                for (KernelTypes::UInt32 Index = 0; Index < 3; ++Index)
                                    Value += (*this)(Row, Index) * Other(Column, Index);
                                Result(Row, Column) = Value;
                            }
                        return Result;
                    }

                    Matrix4 Matrix4::operator*(KernelTypes::Float Scale) const noexcept
                    {
                        return {mColumns[0] * Scale, mColumns[1] * Scale, mColumns[2] * Scale, mColumns[3] * Scale};
                    }

                    Matrix4 Matrix4::operator+(const Matrix4& Other) const noexcept
                    {
                        return {mColumns[0] + Other.mColumns[0], mColumns[1] + Other.mColumns[1],
                            mColumns[2] + Other.mColumns[2], mColumns[3] + Other.mColumns[3]};
                    }

                    Matrix4 Matrix4::operator-() const noexcept
                    {
                        return {-mColumns[0], -mColumns[1], -mColumns[2], -mColumns[3]};
                    }

                    Matrix4 Matrix4::operator-(const Matrix4& Other) const noexcept
                    {
                        return {mColumns[0] - Other.mColumns[0], mColumns[1] - Other.mColumns[1],
                            mColumns[2] - Other.mColumns[2], mColumns[3] - Other.mColumns[3]};
                    }

                    Matrix4& Matrix4::operator+=(const Matrix4& Other) noexcept
                    {
                        for (KernelTypes::UInt32 Column = 0; Column < 4; ++Column)
                            mColumns[Column] += Other.mColumns[Column];
                        return *this;
                    }

                    Matrix4& Matrix4::operator*=(KernelTypes::Float Scale) noexcept
                    {
                        for (Vector4& Column : mColumns)
                            Column *= Scale;
                        return *this;
                    }

                    Vector3 Matrix4::GetColumn3(KernelTypes::UInt32 Column) const noexcept
                    {
                        KernelAssert::DebugCheck(Column < 4, "Matrix4 column index < 4");
                        return Vector3(mColumns[Column]);
                    }

                    void Matrix4::SetColumn3(KernelTypes::UInt32 Column, const Vector3& Value) noexcept
                    {
                        KernelAssert::DebugCheck(Column < 4, "Matrix4 column index < 4");
                        mColumns[Column] = Vector4(Value, Column == 3 ? 1.0f : 0.0f);
                    }

                    Vector4 Matrix4::GetColumn4(KernelTypes::UInt32 Column) const noexcept
                    {
                        KernelAssert::DebugCheck(Column < 4, "Matrix4 column index < 4");
                        return mColumns[Column];
                    }

                    void Matrix4::SetColumn4(KernelTypes::UInt32 Column, const Vector4& Value) noexcept
                    {
                        KernelAssert::DebugCheck(Column < 4, "Matrix4 column index < 4");
                        mColumns[Column] = Value;
                    }

                    void Matrix4::StoreFloat4x4(Float4* Output) const noexcept
                    {
                        for (KernelTypes::UInt32 Column = 0; Column < 4; ++Column)
                            mColumns[Column].StoreFloat4(Output + Column);
                    }

                    Matrix4 Matrix4::Transposed() const noexcept
                    {
                        Matrix4 Result;
                        for (KernelTypes::UInt32 Column = 0; Column < 4; ++Column)
                            for (KernelTypes::UInt32 Row = 0; Row < 4; ++Row)
                                Result(Row, Column) = (*this)(Column, Row);
                        return Result;
                    }

                    Matrix4 Matrix4::Transposed3x3() const noexcept
                    {
                        Matrix4 Result = sIdentity();
                        for (KernelTypes::UInt32 Column = 0; Column < 3; ++Column)
                            for (KernelTypes::UInt32 Row = 0; Row < 3; ++Row)
                                Result(Row, Column) = (*this)(Column, Row);
                        return Result;
                    }

                    Matrix4 Matrix4::Inversed() const noexcept
                    {
                        const KernelTypes::Float M00 = (*this)(0, 0), M10 = (*this)(1, 0), M20 = (*this)(2, 0), M30 = (*this)(3, 0);
                        const KernelTypes::Float M01 = (*this)(0, 1), M11 = (*this)(1, 1), M21 = (*this)(2, 1), M31 = (*this)(3, 1);
                        const KernelTypes::Float M02 = (*this)(0, 2), M12 = (*this)(1, 2), M22 = (*this)(2, 2), M32 = (*this)(3, 2);
                        const KernelTypes::Float M03 = (*this)(0, 3), M13 = (*this)(1, 3), M23 = (*this)(2, 3), M33 = (*this)(3, 3);

                        const KernelTypes::Float M10211120 = M10 * M21 - M11 * M20;
                        const KernelTypes::Float M10221220 = M10 * M22 - M12 * M20;
                        const KernelTypes::Float M10231320 = M10 * M23 - M13 * M20;
                        const KernelTypes::Float M10311130 = M10 * M31 - M11 * M30;
                        const KernelTypes::Float M10321230 = M10 * M32 - M12 * M30;
                        const KernelTypes::Float M10331330 = M10 * M33 - M13 * M30;
                        const KernelTypes::Float M11221221 = M11 * M22 - M12 * M21;
                        const KernelTypes::Float M11231321 = M11 * M23 - M13 * M21;
                        const KernelTypes::Float M11321231 = M11 * M32 - M12 * M31;
                        const KernelTypes::Float M11331331 = M11 * M33 - M13 * M31;
                        const KernelTypes::Float M12231322 = M12 * M23 - M13 * M22;
                        const KernelTypes::Float M12331332 = M12 * M33 - M13 * M32;
                        const KernelTypes::Float M20312130 = M20 * M31 - M21 * M30;
                        const KernelTypes::Float M20322230 = M20 * M32 - M22 * M30;
                        const KernelTypes::Float M20332330 = M20 * M33 - M23 * M30;
                        const KernelTypes::Float M21322231 = M21 * M32 - M22 * M31;
                        const KernelTypes::Float M21332331 = M21 * M33 - M23 * M31;
                        const KernelTypes::Float M22332332 = M22 * M33 - M23 * M32;

                        const Vector4 Column0(
                            M11 * M22332332 - M12 * M21332331 + M13 * M21322231,
                            -M10 * M22332332 + M12 * M20332330 - M13 * M20322230,
                            M10 * M21332331 - M11 * M20332330 + M13 * M20312130,
                            -M10 * M21322231 + M11 * M20322230 - M12 * M20312130);
                        const Vector4 Column1(
                            -M01 * M22332332 + M02 * M21332331 - M03 * M21322231,
                            M00 * M22332332 - M02 * M20332330 + M03 * M20322230,
                            -M00 * M21332331 + M01 * M20332330 - M03 * M20312130,
                            M00 * M21322231 - M01 * M20322230 + M02 * M20312130);
                        const Vector4 Column2(
                            M01 * M12331332 - M02 * M11331331 + M03 * M11321231,
                            -M00 * M12331332 + M02 * M10331330 - M03 * M10321230,
                            M00 * M11331331 - M01 * M10331330 + M03 * M10311130,
                            -M00 * M11321231 + M01 * M10321230 - M02 * M10311130);
                        const Vector4 Column3(
                            -M01 * M12231322 + M02 * M11231321 - M03 * M11221221,
                            M00 * M12231322 - M02 * M10231320 + M03 * M10221220,
                            -M00 * M11231321 + M01 * M10231320 - M03 * M10211120,
                            M00 * M11221221 - M01 * M10221220 + M02 * M10211120);
                        const KernelTypes::Float Determinant = M00 * Column0.GetX() + M01 * Column0.GetY()
                            + M02 * Column0.GetZ() + M03 * Column0.GetW();
                        return {Column0 / Determinant, Column1 / Determinant, Column2 / Determinant, Column3 / Determinant};
                    }

                    Matrix4 Matrix4::InversedRotationTranslation() const noexcept
                    {
                        Matrix4 Result = Transposed3x3();
                        Result.SetTranslation(-Result.Multiply3x3(GetTranslation()));
                        return Result;
                    }

                    KernelTypes::Float Matrix4::GetDeterminant3x3() const noexcept
                    {
                        return GetAxisX().Dot(GetAxisY().CrossPrecise(GetAxisZ()));
                    }

                    Matrix4 Matrix4::Adjointed3x3() const noexcept
                    {
                        return {Vector4(GetAxisY().CrossPrecise(GetAxisZ()), 0),
                            Vector4(GetAxisZ().CrossPrecise(GetAxisX()), 0),
                            Vector4(GetAxisX().CrossPrecise(GetAxisY()), 0), Vector4(0, 0, 0, 1)};
                    }

                    Matrix4 Matrix4::Inversed3x3() const noexcept
                    {
                        const KernelTypes::Float Determinant = GetDeterminant3x3();
                        const Matrix4 Adjoint = Adjointed3x3();
                        return {
                            Adjoint.mColumns[0] / Determinant,
                            Adjoint.mColumns[1] / Determinant,
                            Adjoint.mColumns[2] / Determinant,
                            Vector4(0, 0, 0, 1)
                        };
                    }

                    KernelTypes::Bool Matrix4::SetInversed3x3(const Matrix4& Value) noexcept
                    {
                        const KernelTypes::Float Determinant = Value.GetDeterminant3x3();
                        if (Determinant == 0.0f)
                            return false;
                        *this = Value.Adjointed3x3();
                        for (KernelTypes::UInt32 Column = 0; Column < 3; ++Column)
                            mColumns[Column] /= Determinant;
                        return true;
                    }

                    Matrix4 Matrix4::GetRotation() const noexcept
                    {
                        KernelAssert::DebugCheck(mColumns[0].GetW() == 0.0f
                            && mColumns[1].GetW() == 0.0f
                            && mColumns[2].GetW() == 0.0f,
                            "Matrix4 rotation has a zero bottom row");
                        return {
                            Vector4(mColumns[0].GetX(), mColumns[0].GetY(), mColumns[0].GetZ(), 0.0f),
                            Vector4(mColumns[1].GetX(), mColumns[1].GetY(), mColumns[1].GetZ(), 0.0f),
                            Vector4(mColumns[2].GetX(), mColumns[2].GetY(), mColumns[2].GetZ(), 0.0f),
                            Vector4(0, 0, 0, 1)
                        };
                    }

                    Matrix4 Matrix4::GetRotationSafe() const noexcept
                    {
                        return {
                            Vector4(mColumns[0].GetX(), mColumns[0].GetY(), mColumns[0].GetZ(), 0.0f),
                            Vector4(mColumns[1].GetX(), mColumns[1].GetY(), mColumns[1].GetZ(), 0.0f),
                            Vector4(mColumns[2].GetX(), mColumns[2].GetY(), mColumns[2].GetZ(), 0.0f),
                            Vector4(0, 0, 0, 1)
                        };
                    }

                    void Matrix4::SetRotation(const Matrix4& Rotation) noexcept
                    {
                        for (KernelTypes::UInt32 Column = 0; Column < 3; ++Column)
                            mColumns[Column] = Rotation.mColumns[Column];
                    }

                    Matrix4 Matrix4::GetDirectionPreservingMatrix() const noexcept
                    {
                        return GetRotation().Inversed3x3().Transposed3x3();
                    }

                    Matrix4 Matrix4::PreTranslated(const Vector3& Translation) const noexcept
                    {
                        Matrix4 Result = *this;
                        Result.mColumns[3] = Result.mColumns[3] + mColumns[0] * Translation.GetX()
                            + mColumns[1] * Translation.GetY() + mColumns[2] * Translation.GetZ();
                        return Result;
                    }

                    Matrix4 Matrix4::PostTranslated(const Vector3& Translation) const noexcept
                    {
                        Matrix4 Result = *this;
                        Result.SetTranslation(GetTranslation() + Translation);
                        return Result;
                    }

                    Matrix4 Matrix4::PreScaled(const Vector3& Scale) const noexcept
                    {
                        return *this * sScale(Scale);
                    }

                    Matrix4 Matrix4::PostScaled(const Vector3& Scale) const noexcept
                    {
                        Matrix4 Result = *this;
                        for (KernelTypes::UInt32 Column = 0; Column < 4; ++Column)
                        {
                            Result.mColumns[Column].SetX(mColumns[Column].GetX() * Scale.GetX());
                            Result.mColumns[Column].SetY(mColumns[Column].GetY() * Scale.GetY());
                            Result.mColumns[Column].SetZ(mColumns[Column].GetZ() * Scale.GetZ());
                        }
                        return Result;
                    }

                    Matrix4 Matrix4::Decompose(Vector3& Scale) const noexcept
                    {
                        Vector3 X = GetAxisX();
                        Vector3 Y = GetAxisY();
                        Vector3 Z = GetAxisZ();
                        const KernelTypes::Float XDotX = X.LengthSq();
                        Y -= (X.Dot(Y) / XDotX) * X;
                        Z -= (X.Dot(Z) / XDotX) * X;
                        const KernelTypes::Float YDotY = Y.LengthSq();
                        Z -= (Y.Dot(Z) / YDotY) * Y;
                        const KernelTypes::Float ZDotZ = Z.LengthSq();
                        Scale = Vector3(MathScalar::Sqrt(XDotX), MathScalar::Sqrt(YDotY), MathScalar::Sqrt(ZDotZ));
                        if (X.Cross(Y).Dot(Z) < 0.0f)
                            Scale.SetZ(-Scale.GetZ());
                        X /= Scale.GetX();
                        Y /= Scale.GetY();
                        Z /= Scale.GetZ();
                        return {Vector4(X, 0), Vector4(Y, 0), Vector4(Z, 0), mColumns[3]};
                    }
                }
            }
        }
    }
}
