#pragma once

#include "../../../Kernel/Types.hpp"
#include "Plane.hpp"
#include "../Empire.Engine/Engine/Math/Vector/Vector3.hpp"
#include "../Empire.Engine/Engine/Math/Matrix/Matrix4.hpp"

namespace Engine
{
    namespace Math
    {
        using namespace Kernel;
        struct Frustum
        {
            Plane Near;
            Plane Far;
            Plane Left;
            Plane Right;
            Plane Top;
            Plane Bottom;

            static Frustum FromMatrix(const Matrix4& ViewProjection) noexcept;

            Types::b8 ContainsPoint(const Vector3& Point) const noexcept;
        };
    }
}
