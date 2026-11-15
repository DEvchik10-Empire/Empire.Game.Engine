#pragma once

#include "../../../Kernel/Types.hpp"
#include "../Empire.Engine/Engine/Math/Vector/Vector3.hpp"

namespace Engine
{
    namespace Math
    {
        using namespace Kernel;
        struct Ray
        {
            Vector3 Origin;
            Vector3 Direction;

            Ray() noexcept;
            Ray(const Vector3& Origin, const Vector3& Direction) noexcept;

            Vector3 GetPoint(Types::f32 Distance) const noexcept;

            Types::b8 operator==(const Ray& Other) const noexcept;
            Types::b8 operator!=(const Ray& Other) const noexcept;
        };
    }
}
