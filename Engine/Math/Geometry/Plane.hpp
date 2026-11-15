#pragma once

#include "Aliases.hpp"
#include "Vector3.hpp"

using namespace Engine;

namespace Math
{
    struct Plane
    {
        Vector3 Normal;
        f32 Distance;

        Plane() noexcept;
        Plane(const Vector3& Normal, f32 Distance) noexcept;

        static Plane FromPointNormal(
            const Vector3& Point,
            const Vector3& Normal
        ) noexcept;

        f32 DistanceToPoint(const Vector3& Point) const noexcept;

        b8 operator==(const Plane& Other) const noexcept;
        b8 operator!=(const Plane& Other) const noexcept;
    };
}