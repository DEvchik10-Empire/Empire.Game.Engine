#pragma once

#include "Aliases.hpp"
#include "Vector3.hpp"

using namespace Engine;

namespace Math
{
    struct Sphere
    {
        Vector3 Center;
        f32 Radius;

        Sphere() noexcept;
        Sphere(const Vector3& Center, f32 Radius) noexcept;

        b8 Contains(const Vector3& Point) const noexcept;
        b8 Intersects(const Sphere& Other) const noexcept;

        b8 operator==(const Sphere& Other) const noexcept;
        b8 operator!=(const Sphere& Other) const noexcept;
    };
}