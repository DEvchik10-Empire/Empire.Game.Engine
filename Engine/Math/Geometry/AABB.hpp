#pragma once

#include "../../../Kernel/Types.hpp"
#include "../Empire.Engine/Engine/Math/Vector/Vector3.hpp"

namespace Engine
{
    namespace Math
    {
        using namespace Kernel;
        struct AABB
        {
            Vector3 Min;
            Vector3 Max;

            AABB() noexcept;
            AABB(const Vector3& Min, const Vector3& Max) noexcept;

            static AABB FromCenterExtents(const Vector3& Center, const Vector3& Extents) noexcept;

            Vector3 Center() const noexcept;
            Vector3 Extents() const noexcept;
            Vector3 Size() const noexcept;

            Types::b8 Contains(const Vector3& Point) const noexcept;
            Types::b8 Intersects(const AABB& Other) const noexcept;

            AABB Union(const AABB& Other) const noexcept;

            Types::b8 operator==(const AABB& Other) const noexcept;
            Types::b8 operator!=(const AABB& Other) const noexcept;
        };
    }
}
