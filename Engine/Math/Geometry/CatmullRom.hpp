#pragma once

#include "../../../Kernel/Types.hpp"

#include "../Empire.Engine/Engine/Math/Vector/Vector3.hpp"

namespace Engine
{
    namespace Math
    {
        using namespace Kernel;
        struct CatmullRom
        {
            static Vector3 Interpolate(const Vector3& P0, const Vector3& P1, const Vector3& P2, const Vector3& P3, Types::f32 T) noexcept;
        };
    }
}
