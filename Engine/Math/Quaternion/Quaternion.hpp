#pragma once

#include "Aliases.hpp"
#include "Vector3.hpp"
#include "Matrix4.hpp"

using namespace Engine;

namespace Math
{
    struct Quaternion
    {
        f32 X;
        f32 Y;
        f32 Z;
        f32 W;

        Quaternion();
        Quaternion(f32 X, f32 Y, f32 Z, f32 W);

        static Quaternion Identity();

        static Quaternion FromAxisAngle(
            const Vector3& Axis,
            f32 Angle
        );

        static Quaternion FromEulerAngles(
            const Vector3& EulerAngles
        );

        f32 Length() const;
        f32 LengthSquared() const;

        Quaternion Normalized() const;
        void Normalize();

        Quaternion Conjugated() const;
        Quaternion Inversed() const;

        f32 Dot(const Quaternion& Other) const;

        Vector3 Rotate(const Vector3& Vector) const;

        Matrix4 ToMatrix4() const;

        Vector3 ToEulerAngles() const;

        Quaternion operator+(const Quaternion& Other) const;
        Quaternion operator-(const Quaternion& Other) const;

        Quaternion operator*(const Quaternion& Other) const;
        Quaternion operator*(f32 Scalar) const;
        Quaternion operator/(f32 Scalar) const;

        Quaternion& operator+=(const Quaternion& Other);
        Quaternion& operator-=(const Quaternion& Other);

        Quaternion& operator*=(const Quaternion& Other);
        Quaternion& operator*=(f32 Scalar);
        Quaternion& operator/=(f32 Scalar);

        Quaternion operator-() const;

        b8 operator==(const Quaternion& Other) const;
        b8 operator!=(const Quaternion& Other) const;
    };

    Quaternion operator*(f32 Scalar, const Quaternion& QuaternionValue);
}