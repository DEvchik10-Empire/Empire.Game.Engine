#pragma once

#include "Aliases.hpp"
#include "Vector3.hpp"

using namespace Engine;

namespace Math
{
    struct CatmullRom
    {
        static Vector3 Interpolate(
            const Vector3& P0,
            const Vector3& P1,
            const Vector3& P2,
            const Vector3& P3,
            f32 T
        ) noexcept;
    };
}