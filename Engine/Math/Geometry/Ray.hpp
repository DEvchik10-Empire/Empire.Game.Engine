#pragma once

#include "Aliases.hpp"
#include "Vector3.hpp"

using namespace Engine;

namespace Math
{
    struct Ray
    {
        Vector3 Origin;
        Vector3 Direction;

        Ray() noexcept;
        Ray(const Vector3& Origin, const Vector3& Direction) noexcept;

        Vector3 GetPoint(f32 Distance) const noexcept;

        b8 operator==(const Ray& Other) const noexcept;
        b8 operator!=(const Ray& Other) const noexcept;
    };
}