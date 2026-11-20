#include "Empire.Engine/Engine.Math/Math.Quaternion/Quaternion.hpp"
#include "Empire.Engine/Engine.Math/Math.Matrix/Matrix4.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireEngine
        {
            namespace EngineMath
            {
                namespace MathQuaternion
                {
                    Quaternion Quaternion::sRandomFromAngles(
                        KernelTypes::Float X0,
                        KernelTypes::Float Theta1,
                        KernelTypes::Float Theta2
                    ) noexcept
                    {
                        const KernelTypes::Float R1 = MathScalar::Sqrt(1.0f - X0);
                        const KernelTypes::Float R2 = MathScalar::Sqrt(X0);
                        Vector4 Sine;
                        Vector4 Cosine;
                        Vector4(Theta1, Theta2, 0.0f, 0.0f).SinCos(Sine, Cosine);
                        return Quaternion(
                            Sine.GetX() * R1,
                            Cosine.GetX() * R1,
                            Sine.GetY() * R2,
                            Cosine.GetY() * R2
                        );
                    }

                    Quaternion Quaternion::sRotation(const Vector3& Axis, KernelTypes::Float Angle) noexcept
                    {
                        KernelAssert::DebugCheck(Axis.IsNormalized(), "Rotation axis is normalized");
                        const KernelTypes::Float HalfAngle = 0.5f * Angle;
                        const KernelTypes::Float Sine = MathScalar::Sin(HalfAngle);
                        return Quaternion(Axis.GetX() * Sine, Axis.GetY() * Sine, Axis.GetZ() * Sine,
                            MathScalar::Cos(HalfAngle));
                    }

                    void Quaternion::GetAxisAngle(Vector3& Axis, KernelTypes::Float& Angle) const noexcept
                    {
                        KernelAssert::DebugCheck(IsNormalized(), "Quaternion is normalized");
                        const Quaternion PositiveW = EnsureWPositive();
                        const KernelTypes::Float AbsoluteW = PositiveW.GetW();
                        if (AbsoluteW >= 1.0f)
                        {
                            Axis = Vector3::sZero();
                            Angle = 0.0f;
                        }
                        else
                        {
                            Angle = 2.0f * MathScalar::ACos(AbsoluteW);
                            Axis = PositiveW.GetXYZ().NormalizedOr(Vector3::sZero());
                        }
                    }

                    Vector3 Quaternion::GetAngularVelocity(KernelTypes::Float DeltaTime) const noexcept
                    {
                        KernelAssert::DebugCheck(IsNormalized(), "Quaternion is normalized");
                        const Quaternion PositiveW = EnsureWPositive();
                        const Vector3 XYZ = PositiveW.GetXYZ();
                        const KernelTypes::Float LengthSquared = XYZ.LengthSq();
                        if (LengthSquared < 4.0e-4f)
                            return (2.0f / DeltaTime) * XYZ;
                        const KernelTypes::Float Angle = 2.0f * MathScalar::ACos(PositiveW.GetW());
                        return (XYZ / (MathScalar::Sqrt(LengthSquared) * DeltaTime)) * Angle;
                    }

                    Quaternion Quaternion::sFromTo(const Vector3& From, const Vector3& To) noexcept
                    {
                        const KernelTypes::Float LengthProduct = MathScalar::Sqrt(From.LengthSq() * To.LengthSq());
                        const KernelTypes::Float W = LengthProduct + From.Dot(To);
                        if (W == 0.0f)
                        {
                            if (LengthProduct == 0.0f)
                                return sIdentity();
                            return Quaternion(Vector4(From.GetNormalizedPerpendicular(), 0.0f));
                        }
                        return Quaternion(Vector4(From.Cross(To), W)).Normalized();
                    }

                    Quaternion Quaternion::sEulerAngles(const Vector3& Angles) noexcept
                    {
                        const Vector4 Half(0.5f * Angles.GetX(), 0.5f * Angles.GetY(), 0.5f * Angles.GetZ(), 0.0f);
                        Vector4 Sine;
                        Vector4 Cosine;
                        Half.SinCos(Sine, Cosine);
                        const KernelTypes::Float CX = Cosine.GetX();
                        const KernelTypes::Float SX = Sine.GetX();
                        const KernelTypes::Float CY = Cosine.GetY();
                        const KernelTypes::Float SY = Sine.GetY();
                        const KernelTypes::Float CZ = Cosine.GetZ();
                        const KernelTypes::Float SZ = Sine.GetZ();
                        return {
                            CZ * SX * CY - SZ * CX * SY,
                            CZ * CX * SY + SZ * SX * CY,
                            SZ * CX * CY - CZ * SX * SY,
                            CZ * CX * CY + SZ * SX * SY
                        };
                    }

                    Vector3 Quaternion::GetEulerAngles() const noexcept
                    {
                        const KernelTypes::Float X = GetX();
                        const KernelTypes::Float Y = GetY();
                        const KernelTypes::Float Z = GetZ();
                        const KernelTypes::Float W = GetW();
                        const KernelTypes::Float YSquared = Y * Y;
                        const KernelTypes::Float T0 = 2.0f * (W * X + Y * Z);
                        const KernelTypes::Float T1 = 1.0f - 2.0f * (X * X + YSquared);
                        const KernelTypes::Float T2 = MathScalar::Clamp(2.0f * (W * Y - Z * X), -1.0f, 1.0f);
                        const KernelTypes::Float T3 = 2.0f * (W * Z + X * Y);
                        const KernelTypes::Float T4 = 1.0f - 2.0f * (YSquared + Z * Z);
                        return {MathScalar::ATan2(T0, T1), MathScalar::ASin(T2), MathScalar::ATan2(T3, T4)};
                    }

                    Quaternion Quaternion::operator*(const Quaternion& Other) const noexcept
                    {
                        const KernelTypes::Float A = GetX();
                        const KernelTypes::Float B = GetY();
                        const KernelTypes::Float C = GetZ();
                        const KernelTypes::Float D = GetW();
                        const KernelTypes::Float X = Other.GetX();
                        const KernelTypes::Float Y = Other.GetY();
                        const KernelTypes::Float Z = Other.GetZ();
                        const KernelTypes::Float W = Other.GetW();
                        return {
                            (A * W + B * Z) + (D * X - C * Y),
                            (B * W + C * X) + (D * Y - A * Z),
                            (C * W + A * Y) + (D * Z - B * X),
                            -(A * X + B * Y) + (D * W - C * Z)
                        };
                    }

                    Quaternion Quaternion::sMultiplyImaginary(const Vector3& Left, const Quaternion& Right) noexcept
                    {
                        const KernelTypes::Float A = Left.GetX();
                        const KernelTypes::Float B = Left.GetY();
                        const KernelTypes::Float C = Left.GetZ();
                        const KernelTypes::Float X = Right.GetX();
                        const KernelTypes::Float Y = Right.GetY();
                        const KernelTypes::Float Z = Right.GetZ();
                        const KernelTypes::Float W = Right.GetW();
                        return {
                            (A * W + B * Z) - C * Y,
                            (B * W + C * X) - A * Z,
                            (C * W + A * Y) - B * X,
                            -(A * X + B * Y) - C * Z
                        };
                    }

                    Vector3 Quaternion::operator*(const Vector3& Value) const noexcept
                    {
                        KernelAssert::DebugCheck(IsNormalized(), "Quaternion is normalized");
                        const Vector3 Imaginary = GetXYZ();
                        const Vector3 Cross = Imaginary.Cross(Value);
                        const Vector3 DoubleCross = Imaginary.Cross(Cross);
                        return Value + (Cross * GetW() + DoubleCross) * 2.0f;
                    }

                    Vector3 Quaternion::InverseRotate(const Vector3& Value) const noexcept
                    {
                        KernelAssert::DebugCheck(IsNormalized(), "Quaternion is normalized");
                        const Vector3 Imaginary = -GetXYZ();
                        const Vector3 Cross = Imaginary.Cross(Value);
                        const Vector3 DoubleCross = Imaginary.Cross(Cross);
                        return Value + (Cross * GetW() + DoubleCross) * 2.0f;
                    }

                    Quaternion Quaternion::EnsureWPositive() const noexcept
                    {
                        return GetW() < 0.0f ? -(*this) : *this;
                    }

                    Quaternion Quaternion::GetPerpendicular() const noexcept
                    {
                        return Quaternion(mValue.Swizzle<1, 0, 3, 2>().FlipSign<1, -1, 1, -1>());
                    }

                    KernelTypes::Float Quaternion::GetRotationAngle(const Vector3& Axis) const noexcept
                    {
                        return GetW() == 0.0f ? MathScalar::Pi : 2.0f * MathScalar::ATan(GetXYZ().Dot(Axis) / GetW());
                    }

                    Quaternion Quaternion::GetTwist(const Vector3& Axis) const noexcept
                    {
                        const Vector3 Imaginary = GetXYZ().Dot(Axis) * Axis;
                        const Quaternion Twist(Imaginary.GetX(), Imaginary.GetY(), Imaginary.GetZ(), GetW());
                        const KernelTypes::Float LengthSquared = Twist.LengthSq();
                        return LengthSquared != 0.0f ? Twist / MathScalar::Sqrt(LengthSquared) : sIdentity();
                    }

                    void Quaternion::GetSwingTwist(Quaternion& Swing, Quaternion& Twist) const noexcept
                    {
                        const KernelTypes::Float X = GetX();
                        const KernelTypes::Float Y = GetY();
                        const KernelTypes::Float Z = GetZ();
                        const KernelTypes::Float W = GetW();
                        const KernelTypes::Float S = MathScalar::Sqrt(MathScalar::Square(W) + MathScalar::Square(X));
                        if (S != 0.0f)
                        {
                            Twist = Quaternion(X / S, 0, 0, W / S);
                            Swing = Quaternion(0, (W * Y - X * Z) / S, (W * Z + X * Y) / S, S);
                        }
                        else
                        {
                            Twist = sIdentity();
                            Swing = *this;
                        }
                    }

                    Quaternion Quaternion::LERP(const Quaternion& Destination, KernelTypes::Float Fraction) const noexcept
                    {
                        return (1.0f - Fraction) * (*this) + Fraction * Destination;
                    }

                    Quaternion Quaternion::SLERP(const Quaternion& Destination, KernelTypes::Float Fraction) const noexcept
                    {
                        constexpr KernelTypes::Float Delta = 0.0001f;
                        KernelTypes::Float SignScale1 = 1.0f;
                        KernelTypes::Float CosOmega = Dot(Destination);
                        if (CosOmega < 0.0f)
                        {
                            CosOmega = -CosOmega;
                            SignScale1 = -1.0f;
                        }
                        KernelTypes::Float Scale0;
                        KernelTypes::Float Scale1;
                        if (1.0f - CosOmega > Delta)
                        {
                            const KernelTypes::Float Omega = MathScalar::ACos(CosOmega);
                            const KernelTypes::Float SinOmega = MathScalar::Sin(Omega);
                            Scale0 = MathScalar::Sin((1.0f - Fraction) * Omega) / SinOmega;
                            Scale1 = SignScale1 * MathScalar::Sin(Fraction * Omega) / SinOmega;
                        }
                        else
                        {
                            Scale0 = 1.0f - Fraction;
                            Scale1 = SignScale1 * Fraction;
                        }
                        return (Scale0 * (*this) + Scale1 * Destination).Normalized();
                    }

                    Quaternion Quaternion::sLoadFloat3Unsafe(const Float3& Value) noexcept
                    {
                        const Vector3 Imaginary = Vector3::sLoadFloat3Unsafe(Value);
                        const KernelTypes::Float W = MathScalar::Sqrt(MathScalar::Max(1.0f - Imaginary.LengthSq(), 0.0f));
                        return Quaternion(Imaginary.GetX(), Imaginary.GetY(), Imaginary.GetZ(), W);
                    }

                    void Quaternion::StoreFloat3(Float3* Output) const noexcept
                    {
                        KernelAssert::DebugCheck(IsNormalized(), "Quaternion is normalized");
                        EnsureWPositive().GetXYZ().StoreFloat3(Output);
                    }
                }

                namespace MathMatrix
                {
                    using namespace MathVector;
                    using MathQuaternion::Quaternion;

                    Matrix4 Matrix4::sRotation(const Quaternion& Rotation) noexcept
                    {
                        KernelAssert::DebugCheck(Rotation.IsNormalized(), "Quaternion is normalized");
                        const KernelTypes::Float X = Rotation.GetX();
                        const KernelTypes::Float Y = Rotation.GetY();
                        const KernelTypes::Float Z = Rotation.GetZ();
                        const KernelTypes::Float W = Rotation.GetW();
                        const KernelTypes::Float TX = X + X;
                        const KernelTypes::Float TY = Y + Y;
                        const KernelTypes::Float TZ = Z + Z;
                        const KernelTypes::Float XX = TX * X;
                        const KernelTypes::Float YY = TY * Y;
                        const KernelTypes::Float ZZ = TZ * Z;
                        const KernelTypes::Float XY = TX * Y;
                        const KernelTypes::Float XZ = TX * Z;
                        const KernelTypes::Float XW = TX * W;
                        const KernelTypes::Float YZ = TY * Z;
                        const KernelTypes::Float YW = TY * W;
                        const KernelTypes::Float ZW = TZ * W;
                        return {
                            Vector4((1.0f - YY) - ZZ, XY + ZW, XZ - YW, 0),
                            Vector4(XY - ZW, (1.0f - ZZ) - XX, YZ + XW, 0),
                            Vector4(XZ + YW, YZ - XW, (1.0f - XX) - YY, 0),
                            Vector4(0, 0, 0, 1)
                        };
                    }

                    Matrix4 Matrix4::sRotation(const Vector3& Axis, KernelTypes::Float Angle) noexcept
                    {
                        return sRotation(Quaternion::sRotation(Axis, Angle));
                    }

                    Matrix4 Matrix4::sRotationTranslation(const Quaternion& Rotation, const Vector3& Translation) noexcept
                    {
                        Matrix4 Result = sRotation(Rotation);
                        Result.SetTranslation(Translation);
                        return Result;
                    }

                    Matrix4 Matrix4::sInverseRotationTranslation(const Quaternion& Rotation, const Vector3& Translation) noexcept
                    {
                        Matrix4 Result = sRotation(Rotation.Conjugated());
                        Result.SetTranslation(-Result.Multiply3x3(Translation));
                        return Result;
                    }

                    Matrix4 Matrix4::sQuatLeftMultiply(const Quaternion& Q) noexcept
                    {
                        const KernelTypes::Float X = Q.GetX(), Y = Q.GetY(), Z = Q.GetZ(), W = Q.GetW();
                        return {
                            Vector4(W, Z, -Y, -X), Vector4(-Z, W, X, -Y),
                            Vector4(Y, -X, W, -Z), Vector4(X, Y, Z, W)
                        };
                    }

                    Matrix4 Matrix4::sQuatRightMultiply(const Quaternion& Q) noexcept
                    {
                        const KernelTypes::Float X = Q.GetX(), Y = Q.GetY(), Z = Q.GetZ(), W = Q.GetW();
                        return {
                            Vector4(W, -Z, Y, -X), Vector4(Z, W, -X, -Y),
                            Vector4(-Y, X, W, -Z), Vector4(X, Y, Z, W)
                        };
                    }

                    Quaternion Matrix4::GetQuaternion() const noexcept
                    {
                        const KernelTypes::Float Trace = (*this)(0, 0) + (*this)(1, 1) + (*this)(2, 2);
                        KernelTypes::Float X, Y, Z, W;
                        if (Trace >= 0.0f)
                        {
                            const KernelTypes::Float S = MathScalar::Sqrt(Trace + 1.0f);
                            const KernelTypes::Float InverseS = 0.5f / S;
                            X = ((*this)(2, 1) - (*this)(1, 2)) * InverseS;
                            Y = ((*this)(0, 2) - (*this)(2, 0)) * InverseS;
                            Z = ((*this)(1, 0) - (*this)(0, 1)) * InverseS;
                            W = 0.5f * S;
                        }
                        else
                        {
                            KernelTypes::UInt32 Index = 0;
                            if ((*this)(1, 1) > (*this)(0, 0))
                                Index = 1;
                            if ((*this)(2, 2) > (*this)(Index, Index))
                                Index = 2;

                            if (Index == 0)
                            {
                                const KernelTypes::Float S = MathScalar::Sqrt((*this)(0, 0) - ((*this)(1, 1) + (*this)(2, 2)) + 1.0f);
                                const KernelTypes::Float InverseS = 0.5f / S;
                                X = 0.5f * S;
                                Y = ((*this)(0, 1) + (*this)(1, 0)) * InverseS;
                                Z = ((*this)(0, 2) + (*this)(2, 0)) * InverseS;
                                W = ((*this)(2, 1) - (*this)(1, 2)) * InverseS;
                            }
                            else if (Index == 1)
                            {
                                const KernelTypes::Float S = MathScalar::Sqrt((*this)(1, 1) - ((*this)(2, 2) + (*this)(0, 0)) + 1.0f);
                                const KernelTypes::Float InverseS = 0.5f / S;
                                X = ((*this)(0, 1) + (*this)(1, 0)) * InverseS;
                                Y = 0.5f * S;
                                Z = ((*this)(1, 2) + (*this)(2, 1)) * InverseS;
                                W = ((*this)(2, 0) - (*this)(0, 2)) * InverseS;
                            }
                            else
                            {
                                const KernelTypes::Float S = MathScalar::Sqrt((*this)(2, 2) - ((*this)(0, 0) + (*this)(1, 1)) + 1.0f);
                                const KernelTypes::Float InverseS = 0.5f / S;
                                X = ((*this)(0, 2) + (*this)(2, 0)) * InverseS;
                                Y = ((*this)(1, 2) + (*this)(2, 1)) * InverseS;
                                Z = 0.5f * S;
                                W = ((*this)(1, 0) - (*this)(0, 1)) * InverseS;
                            }
                        }
                        return Quaternion(X, Y, Z, W);
                    }
                }
            }
        }
    }
}
