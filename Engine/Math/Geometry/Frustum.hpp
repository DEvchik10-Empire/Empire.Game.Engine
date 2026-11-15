#pragma once

#include "Aliases.hpp"
#include "Plane.hpp"
#include "Vector3.hpp"
#include "Matrix4.hpp"

using namespace Engine;

namespace Math
{
    struct Frustum
    {
        Plane Near;
        Plane Far;
        Plane Left;
        Plane Right;
        Plane Top;
        Plane Bottom;

        static Frustum FromMatrix(const Matrix4& ViewProjection) noexcept;

        b8 ContainsPoint(const Vector3& Point) const noexcept;
    };
}