#include "Empire.Engine/Engine.Math/Math.Quaternion/Quaternion.Quaternion.hpp"

#include <cmath>

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
                    using namespace EngineKernel;

                    Quaternion::Quaternion()
                        : X(0.0f)
                        , Y(0.0f)
                        , Z(0.0f)
                        , W(1.0f)
                    {
                    }

                    Quaternion::Quaternion(KernelTypes::Float X, KernelTypes::Float Y, KernelTypes::Float Z, KernelTypes::Float W)
                        : X(X)
                        , Y(Y)
                        , Z(Z)
                        , W(W)
                    {
                    }

                    Quaternion Quaternion::Identity()
                    {
                        return {
                            0.0f,
                            0.0f,
                            0.0f,
                            1.0f
                        };
                    }

                    Quaternion Quaternion::FromAxisAngle(const Vector3& Axis, KernelTypes::Float Angle)
                    {
                        const Vector3 NormalizedAxis = Axis.Normalized();

                        const KernelTypes::Float HalfAngle = Angle * 0.5f;
                        const KernelTypes::Float SinHalfAngle = std::sin(HalfAngle);
                        const KernelTypes::Float CosHalfAngle = std::cos(HalfAngle);

                        return {
                            NormalizedAxis.X * SinHalfAngle,
                            NormalizedAxis.Y * SinHalfAngle,
                            NormalizedAxis.Z * SinHalfAngle,
                            CosHalfAngle
                        };
                    }

                    Quaternion Quaternion::FromEulerAngles(const Vector3& EulerAngles)
                    {
                        const KernelTypes::Float HalfX = EulerAngles.X * 0.5f;
                        const KernelTypes::Float HalfY = EulerAngles.Y * 0.5f;
                        const KernelTypes::Float HalfZ = EulerAngles.Z * 0.5f;

                        const KernelTypes::Float CosX = std::cos(HalfX);
                        const KernelTypes::Float SinX = std::sin(HalfX);

                        const KernelTypes::Float CosY = std::cos(HalfY);
                        const KernelTypes::Float SinY = std::sin(HalfY);

                        const KernelTypes::Float CosZ = std::cos(HalfZ);
                        const KernelTypes::Float SinZ = std::sin(HalfZ);

                        return {
                            SinX * CosY * CosZ - CosX * SinY * SinZ,
                            CosX * SinY * CosZ + SinX * CosY * SinZ,
                            CosX * CosY * SinZ - SinX * SinY * CosZ,
                            CosX * CosY * CosZ + SinX * SinY * SinZ
                        };
                    }

                    KernelTypes::Float Quaternion::Length() const
                    {
                        return std::sqrt(
                            X * X +
                            Y * Y +
                            Z * Z +
                            W * W
                        );
                    }

                    KernelTypes::Float Quaternion::LengthSquared() const
                    {
                        return
                            X * X +
                            Y * Y +
                            Z * Z +
                            W * W;
                    }

                    Quaternion Quaternion::Normalized() const
                    {
                        const KernelTypes::Float QuaternionLength = Length();

                        if (QuaternionLength == 0.0f)
                        {
                            return Identity();
                        }

                        return *this / QuaternionLength;
                    }

                    void Quaternion::Normalize()
                    {
                        const KernelTypes::Float QuaternionLength = Length();

                        if (QuaternionLength == 0.0f)
                        {
                            *this = Identity();
                            return;
                        }

                        *this /= QuaternionLength;
                    }

                    Quaternion Quaternion::Conjugated() const
                    {
                        return {
                            -X,
                            -Y,
                            -Z,
                            W
                        };
                    }

                    Quaternion Quaternion::Inversed() const
                    {
                        const KernelTypes::Float LengthSquaredValue = LengthSquared();

                        if (LengthSquaredValue == 0.0f)
                        {
                            return Identity();
                        }

                        return Conjugated() / LengthSquaredValue;
                    }

                    KernelTypes::Float Quaternion::Dot(const Quaternion& Other) const
                    {
                        return
                            X * Other.X +
                            Y * Other.Y +
                            Z * Other.Z +
                            W * Other.W;
                    }

                    Vector3 Quaternion::Rotate(const Vector3& Vector) const
                    {
                        const Quaternion VectorQuaternion(
                            Vector.X,
                            Vector.Y,
                            Vector.Z,
                            0.0f
                        );

                        const Quaternion Result =
                            *this *
                            VectorQuaternion *
                            Inversed();

                        return {
                            Result.X,
                            Result.Y,
                            Result.Z
                        };
                    }

                    Matrix4 Quaternion::ToMatrix4() const
                    {
                        const Quaternion NormalizedQuaternion = Normalized();

                        const KernelTypes::Float XX = NormalizedQuaternion.X * NormalizedQuaternion.X;
                        const KernelTypes::Float YY = NormalizedQuaternion.Y * NormalizedQuaternion.Y;
                        const KernelTypes::Float ZZ = NormalizedQuaternion.Z * NormalizedQuaternion.Z;

                        const KernelTypes::Float XY = NormalizedQuaternion.X * NormalizedQuaternion.Y;
                        const KernelTypes::Float XZ = NormalizedQuaternion.X * NormalizedQuaternion.Z;
                        const KernelTypes::Float YZ = NormalizedQuaternion.Y * NormalizedQuaternion.Z;

                        const KernelTypes::Float WX = NormalizedQuaternion.W * NormalizedQuaternion.X;
                        const KernelTypes::Float WY = NormalizedQuaternion.W * NormalizedQuaternion.Y;
                        const KernelTypes::Float WZ = NormalizedQuaternion.W * NormalizedQuaternion.Z;

                        return {
                            1.0f - 2.0f * (YY + ZZ),
                            2.0f * (XY - WZ),
                            2.0f * (XZ + WY),
                            0.0f,

                            2.0f * (XY + WZ),
                            1.0f - 2.0f * (XX + ZZ),
                            2.0f * (YZ - WX),
                            0.0f,

                            2.0f * (XZ - WY),
                            2.0f * (YZ + WX),
                            1.0f - 2.0f * (XX + YY),
                            0.0f,

                            0.0f,
                            0.0f,
                            0.0f,
                            1.0f
                        };
                    }

                    Vector3 Quaternion::ToEulerAngles() const
                    {
                        const Quaternion NormalizedQuaternion = Normalized();

                        const KernelTypes::Float SinX =
                            2.0f *
                            (
                                NormalizedQuaternion.W * NormalizedQuaternion.X +
                                NormalizedQuaternion.Y * NormalizedQuaternion.Z
                            );

                        const KernelTypes::Float CosX =
                            1.0f -
                            2.0f *
                            (
                                NormalizedQuaternion.X * NormalizedQuaternion.X +
                                NormalizedQuaternion.Y * NormalizedQuaternion.Y
                            );

                        const KernelTypes::Float XAngle = std::atan2(SinX, CosX);

                        const KernelTypes::Float SinY =
                            2.0f *
                            (
                                NormalizedQuaternion.W * NormalizedQuaternion.Y -
                                NormalizedQuaternion.Z * NormalizedQuaternion.X
                            );

                        const KernelTypes::Float ClampedSinY =
                            std::fmax(-1.0f, std::fmin(1.0f, SinY));

                        const KernelTypes::Float YAngle = std::asin(ClampedSinY);

                        const KernelTypes::Float SinZ =
                            2.0f *
                            (
                                NormalizedQuaternion.W * NormalizedQuaternion.Z +
                                NormalizedQuaternion.X * NormalizedQuaternion.Y
                            );

                        const KernelTypes::Float CosZ =
                            1.0f -
                            2.0f *
                            (
                                NormalizedQuaternion.Y * NormalizedQuaternion.Y +
                                NormalizedQuaternion.Z * NormalizedQuaternion.Z
                            );

                        const KernelTypes::Float ZAngle = std::atan2(SinZ, CosZ);

                        return {
                            XAngle,
                            YAngle,
                            ZAngle
                        };
                    }

                    Quaternion Quaternion::operator+(const Quaternion& Other) const
                    {
                        return {
                            X + Other.X,
                            Y + Other.Y,
                            Z + Other.Z,
                            W + Other.W
                        };
                    }

                    Quaternion Quaternion::operator-(const Quaternion& Other) const
                    {
                        return {
                            X - Other.X,
                            Y - Other.Y,
                            Z - Other.Z,
                            W - Other.W
                        };
                    }

                    Quaternion Quaternion::operator*(const Quaternion& Other) const
                    {
                        return {
                            W * Other.X + X * Other.W + Y * Other.Z - Z * Other.Y,
                            W * Other.Y - X * Other.Z + Y * Other.W + Z * Other.X,
                            W * Other.Z + X * Other.Y - Y * Other.X + Z * Other.W,
                            W * Other.W - X * Other.X - Y * Other.Y - Z * Other.Z
                        };
                    }

                    Quaternion Quaternion::operator*(KernelTypes::Float Scalar) const
                    {
                        return {
                            X * Scalar,
                            Y * Scalar,
                            Z * Scalar,
                            W * Scalar
                        };
                    }

                    Quaternion Quaternion::operator/(KernelTypes::Float Scalar) const
                    {
                        return {
                            X / Scalar,
                            Y / Scalar,
                            Z / Scalar,
                            W / Scalar
                        };
                    }

                    Quaternion& Quaternion::operator+=(const Quaternion& Other)
                    {
                        X += Other.X;
                        Y += Other.Y;
                        Z += Other.Z;
                        W += Other.W;

                        return *this;
                    }

                    Quaternion& Quaternion::operator-=(const Quaternion& Other)
                    {
                        X -= Other.X;
                        Y -= Other.Y;
                        Z -= Other.Z;
                        W -= Other.W;

                        return *this;
                    }

                    Quaternion& Quaternion::operator*=(const Quaternion& Other)
                    {
                        *this = *this * Other;

                        return *this;
                    }

                    Quaternion& Quaternion::operator*=(KernelTypes::Float Scalar)
                    {
                        X *= Scalar;
                        Y *= Scalar;
                        Z *= Scalar;
                        W *= Scalar;

                        return *this;
                    }

                    Quaternion& Quaternion::operator/=(KernelTypes::Float Scalar)
                    {
                        X /= Scalar;
                        Y /= Scalar;
                        Z /= Scalar;
                        W /= Scalar;

                        return *this;
                    }

                    Quaternion Quaternion::operator-() const
                    {
                        return {
                            -X,
                            -Y,
                            -Z,
                            -W
                        };
                    }

                    KernelTypes::Bool Quaternion::operator==(const Quaternion& Other) const
                    {
                        return
                            X == Other.X &&
                            Y == Other.Y &&
                            Z == Other.Z &&
                            W == Other.W;
                    }

                    KernelTypes::Bool Quaternion::operator!=(const Quaternion& Other) const
                    {
                        return !(*this == Other);
                    }

                    Quaternion operator*(KernelTypes::Float Scalar, const Quaternion& QuaternionValue)
                    {
                        return QuaternionValue * Scalar;
                    }
                }
            }
        }
    }
}
