#pragma once

#include "Aliases.hpp"
#include "Vector3.hpp"
#include "Quaternion.hpp"
#include "Matrix4.hpp"

using namespace Engine;

namespace Math
{
    struct Transform
    {
        Vector3 Position;
        Quaternion Rotation;
        Vector3 Scale;

        Transform();

        Transform(
            const Vector3& Position,
            const Quaternion& Rotation,
            const Vector3& Scale
        );

        static Transform Identity();

        Matrix4 ToMatrix4() const;

        Vector3 TransformPoint(const Vector3& Point) const;
        Vector3 TransformDirection(const Vector3& Direction) const;

        Transform Inversed() const;

        Transform operator*(const Transform& Other) const;

        Transform& operator*=(const Transform& Other);

        b8 operator==(const Transform& Other) const;
        b8 operator!=(const Transform& Other) const;
    };
}