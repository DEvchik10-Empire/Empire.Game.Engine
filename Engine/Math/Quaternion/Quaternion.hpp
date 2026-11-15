#pragma once

#include "../../../Kernel/Types.hpp"
#include "../Empire.Engine/Engine/Math/Vector/Vector3.hpp"
#include "../Empire.Engine/Engine/Math/Matrix/Matrix4.hpp"

namespace Engine
{
    namespace Math
    {
        using namespace Kernel;

        struct Quaternion
        {
            Types::f32 X;
            Types::f32 Y;
            Types::f32 Z;
            Types::f32 W;

            Quaternion();
            Quaternion(Types::f32 X, Types::f32 Y, Types::f32 Z, Types::f32 W);

            static Quaternion Identity();

            static Quaternion FromAxisAngle(const Vector3& Axis, Types::f32 Angle);

            static Quaternion FromEulerAngles(const Vector3& EulerAngles);

            Types::f32 Length() const;
            Types::f32 LengthSquared() const;

            Quaternion Normalized() const;
            void Normalize();

            Quaternion Conjugated() const;
            Quaternion Inversed() const;

            Types::f32 Dot(const Quaternion& Other) const;

            Vector3 Rotate(const Vector3& Vector) const;

            Matrix4 ToMatrix4() const;

            Vector3 ToEulerAngles() const;

            Quaternion operator+(const Quaternion& Other) const;
            Quaternion operator-(const Quaternion& Other) const;

            Quaternion operator*(const Quaternion& Other) const;
            Quaternion operator*(Types::f32 Scalar) const;
            Quaternion operator/(Types::f32 Scalar) const;

            Quaternion& operator+=(const Quaternion& Other);
            Quaternion& operator-=(const Quaternion& Other);

            Quaternion& operator*=(const Quaternion& Other);
            Quaternion& operator*=(Types::f32 Scalar);
            Quaternion& operator/=(Types::f32 Scalar);

            Quaternion operator-() const;

            Types::b8 operator==(const Quaternion& Other) const;
            Types::b8 operator!=(const Quaternion& Other) const;
        };

        Quaternion operator*(Types::f32 Scalar, const Quaternion& QuaternionValue);
    }
}
