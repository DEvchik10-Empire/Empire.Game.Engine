#pragma once

#include "../../../Kernel/Types.hpp"
#include "../Empire.Engine/Engine/Math/Vector/Vector3.hpp"
#include "../Empire.Engine/Engine/Math//Quaternion/Quaternion.hpp"
#include "../Empire.Engine/Engine/Math/Matrix/Matrix4.hpp"

namespace Engine
{
    namespace Math
    {
        using namespace Kernel;

        struct Transform
        {
            Vector3 Position;
            Quaternion Rotation;
            Vector3 Scale;

            Transform();

            Transform(const Vector3& Position, const Quaternion& Rotation, const Vector3& Scale);

            static Transform Identity();

            Matrix4 ToMatrix4() const;

            Vector3 TransformPoint(const Vector3& Point) const;
            Vector3 TransformDirection(const Vector3& Direction) const;

            Transform Inversed() const;

            Transform operator*(const Transform& Other) const;

            Transform& operator*=(const Transform& Other);

            Types::b8 operator==(const Transform& Other) const;
            Types::b8 operator!=(const Transform& Other) const;
        };
    }
}
