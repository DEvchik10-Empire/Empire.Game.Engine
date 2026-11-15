#pragma once

#include "../../../Kernel/Types.hpp"
#include "../Empire.Engine/Engine/Math/Vector/Vector3.hpp"

namespace Engine
{
    namespace Math
    {
        using namespace Kernel;
        struct Sphere
        {
            Vector3 Center;
            Types::f32 Radius;

            Sphere() noexcept;
            Sphere(const Vector3& Center, Types::f32 Radius) noexcept;

            Types::b8 Contains(const Vector3& Point) const noexcept;
            Types::b8 Intersects(const Sphere& Other) const noexcept;

            Types::b8 operator==(const Sphere& Other) const noexcept;
            Types::b8 operator!=(const Sphere& Other) const noexcept;
        };
    }
}
