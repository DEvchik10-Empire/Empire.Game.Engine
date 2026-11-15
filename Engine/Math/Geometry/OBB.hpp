#pragma once

#include "../../../Kernel/Types.hpp"
#include "../Empire.Engine/Engine/Math/Vector/Vector3.hpp"
#include "../Empire.Engine/Engine/Math/Quaternion/Quaternion.hpp"

namespace Engine
{
    namespace Math
    {
        using namespace Kernel;
        struct OBB
        {
            Vector3 Center;
            Vector3 Extents;
            Quaternion Orientation;

            OBB() noexcept;
            OBB(const Vector3& Center, const Vector3& Extents, const Quaternion& Orientation) noexcept;

            Types::b8 Contains(const Vector3& Point) const noexcept;

            Types::b8 operator==(const OBB& Other) const noexcept;
            Types::b8 operator!=(const OBB& Other) const noexcept;
        };
    }
}
