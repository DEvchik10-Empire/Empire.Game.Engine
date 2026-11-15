#include "OBB.hpp"

namespace Math
{
    OBB::OBB() noexcept
        : Center(0.0f, 0.0f, 0.0f), Extents(1.0f, 1.0f, 1.0f), Orientation(Quaternion::Identity())
    {
    }

    OBB::OBB(const Vector3& Center, const Vector3& Extents, const Quaternion& Orientation) noexcept
        : Center(Center), Extents(Extents), Orientation(Orientation)
    {
    }

    b8 OBB::Contains(const Vector3& Point) const noexcept
    {
        Vector3 LocalPoint = Point - Center;
        LocalPoint = Orientation.Inversed().Rotate(LocalPoint);

        return LocalPoint.X >= -Extents.X && LocalPoint.X <= Extents.X &&
            LocalPoint.Y >= -Extents.Y && LocalPoint.Y <= Extents.Y &&
            LocalPoint.Z >= -Extents.Z && LocalPoint.Z <= Extents.Z;
    }

    b8 OBB::operator==(const OBB& Other) const noexcept
    {
        return Center == Other.Center &&
               Extents == Other.Extents &&
               Orientation == Other.Orientation;
    }

    b8 OBB::operator!=(const OBB& Other) const noexcept
    {
        return !(*this == Other);
    }

    
}