#pragma once

#include "../../../Kernel/Types.hpp"
#include "../Empire.Engine/Engine/Math/Vector/Vector3.hpp"

namespace Engine
{
    namespace Math
    {
        using namespace Kernel;
        struct Plane
        {
            Vector3 Normal;
            Types::f32 Distance;

            Plane() noexcept;
            Plane(const Vector3& Normal, Types::f32 Distance) noexcept;

            static Plane FromPointNormal(const Vector3& Point, const Vector3& Normal) noexcept;

            Types::f32 DistanceToPoint(const Vector3& Point) const noexcept;

            Types::b8 operator==(const Plane& Other) const noexcept;
            Types::b8 operator!=(const Plane& Other) const noexcept;
        };
    }
}
