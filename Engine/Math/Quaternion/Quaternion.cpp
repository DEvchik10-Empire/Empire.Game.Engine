#include "Quaternion.hpp"

#include <cmath>

namespace Engine
{
    namespace Math
    {
        using namespace Kernel;

        Quaternion::Quaternion()
            : X(0.0f)
            , Y(0.0f)
            , Z(0.0f)
            , W(1.0f)
        {
        }

        Quaternion::Quaternion(Types::f32 X, Types::f32 Y, Types::f32 Z, Types::f32 W)
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

        Quaternion Quaternion::FromAxisAngle(const Vector3& Axis, Types::f32 Angle)
        {
            const Vector3 NormalizedAxis = Axis.Normalized();

            const Types::f32 HalfAngle = Angle * 0.5f;
            const Types::f32 SinHalfAngle = std::sin(HalfAngle);
            const Types::f32 CosHalfAngle = std::cos(HalfAngle);

            return {
                NormalizedAxis.X * SinHalfAngle,
                NormalizedAxis.Y * SinHalfAngle,
                NormalizedAxis.Z * SinHalfAngle,
                CosHalfAngle
            };
        }

        Quaternion Quaternion::FromEulerAngles(const Vector3& EulerAngles)
        {
            const Types::f32 HalfX = EulerAngles.X * 0.5f;
            const Types::f32 HalfY = EulerAngles.Y * 0.5f;
            const Types::f32 HalfZ = EulerAngles.Z * 0.5f;

            const Types::f32 CosX = std::cos(HalfX);
            const Types::f32 SinX = std::sin(HalfX);

            const Types::f32 CosY = std::cos(HalfY);
            const Types::f32 SinY = std::sin(HalfY);

            const Types::f32 CosZ = std::cos(HalfZ);
            const Types::f32 SinZ = std::sin(HalfZ);

            return {
                SinX * CosY * CosZ - CosX * SinY * SinZ,
                CosX * SinY * CosZ + SinX * CosY * SinZ,
                CosX * CosY * SinZ - SinX * SinY * CosZ,
                CosX * CosY * CosZ + SinX * SinY * SinZ
            };
        }

        Types::f32 Quaternion::Length() const
        {
            return std::sqrt(
                X * X +
                Y * Y +
                Z * Z +
                W * W
            );
        }

        Types::f32 Quaternion::LengthSquared() const
        {
            return
                X * X +
                Y * Y +
                Z * Z +
                W * W;
        }

        Quaternion Quaternion::Normalized() const
        {
            const Types::f32 QuaternionLength = Length();

            if (QuaternionLength == 0.0f)
            {
                return Identity();
            }

            return *this / QuaternionLength;
        }

        void Quaternion::Normalize()
        {
            const Types::f32 QuaternionLength = Length();

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
            const Types::f32 LengthSquaredValue = LengthSquared();

            if (LengthSquaredValue == 0.0f)
            {
                return Identity();
            }

            return Conjugated() / LengthSquaredValue;
        }

        Types::f32 Quaternion::Dot(const Quaternion& Other) const
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

            const Types::f32 XX = NormalizedQuaternion.X * NormalizedQuaternion.X;
            const Types::f32 YY = NormalizedQuaternion.Y * NormalizedQuaternion.Y;
            const Types::f32 ZZ = NormalizedQuaternion.Z * NormalizedQuaternion.Z;

            const Types::f32 XY = NormalizedQuaternion.X * NormalizedQuaternion.Y;
            const Types::f32 XZ = NormalizedQuaternion.X * NormalizedQuaternion.Z;
            const Types::f32 YZ = NormalizedQuaternion.Y * NormalizedQuaternion.Z;

            const Types::f32 WX = NormalizedQuaternion.W * NormalizedQuaternion.X;
            const Types::f32 WY = NormalizedQuaternion.W * NormalizedQuaternion.Y;
            const Types::f32 WZ = NormalizedQuaternion.W * NormalizedQuaternion.Z;

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

            const Types::f32 SinX =
                2.0f *
                (
                    NormalizedQuaternion.W * NormalizedQuaternion.X +
                    NormalizedQuaternion.Y * NormalizedQuaternion.Z
                );

            const Types::f32 CosX =
                1.0f -
                2.0f *
                (
                    NormalizedQuaternion.X * NormalizedQuaternion.X +
                    NormalizedQuaternion.Y * NormalizedQuaternion.Y
                );

            const Types::f32 XAngle = std::atan2(SinX, CosX);

            const Types::f32 SinY =
                2.0f *
                (
                    NormalizedQuaternion.W * NormalizedQuaternion.Y -
                    NormalizedQuaternion.Z * NormalizedQuaternion.X
                );

            const Types::f32 ClampedSinY =
                std::fmax(-1.0f, std::fmin(1.0f, SinY));

            const Types::f32 YAngle = std::asin(ClampedSinY);

            const Types::f32 SinZ =
                2.0f *
                (
                    NormalizedQuaternion.W * NormalizedQuaternion.Z +
                    NormalizedQuaternion.X * NormalizedQuaternion.Y
                );

            const Types::f32 CosZ =
                1.0f -
                2.0f *
                (
                    NormalizedQuaternion.Y * NormalizedQuaternion.Y +
                    NormalizedQuaternion.Z * NormalizedQuaternion.Z
                );

            const Types::f32 ZAngle = std::atan2(SinZ, CosZ);

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

        Quaternion Quaternion::operator*(Types::f32 Scalar) const
        {
            return {
                X * Scalar,
                Y * Scalar,
                Z * Scalar,
                W * Scalar
            };
        }

        Quaternion Quaternion::operator/(Types::f32 Scalar) const
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

        Quaternion& Quaternion::operator*=(Types::f32 Scalar)
        {
            X *= Scalar;
            Y *= Scalar;
            Z *= Scalar;
            W *= Scalar;

            return *this;
        }

        Quaternion& Quaternion::operator/=(Types::f32 Scalar)
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

        Types::b8 Quaternion::operator==(const Quaternion& Other) const
        {
            return
                X == Other.X &&
                Y == Other.Y &&
                Z == Other.Z &&
                W == Other.W;
        }

        Types::b8 Quaternion::operator!=(const Quaternion& Other) const
        {
            return !(*this == Other);
        }

        Quaternion operator*(Types::f32 Scalar, const Quaternion& QuaternionValue)
        {
            return QuaternionValue * Scalar;
        }
    }
}
