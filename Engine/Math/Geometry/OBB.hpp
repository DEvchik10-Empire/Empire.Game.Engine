#pragma once

#include "Aliases.hpp"
#include "Vector3.hpp"
#include "Quaternion.hpp"

using namespace Engine;

namespace Math
{
    struct OBB
    {
        Vector3 Center;
        Vector3 Extents;
        Quaternion Orientation;

        OBB() noexcept;
        OBB(const Vector3& Center, const Vector3& Extents, const Quaternion& Orientation) noexcept;

        b8 Contains(const Vector3& Point) const noexcept;

        b8 operator==(const OBB& Other) const noexcept;
        b8 operator!=(const OBB& Other) const noexcept;
    };
}