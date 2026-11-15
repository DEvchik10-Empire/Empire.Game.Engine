#include "Plane.hpp"

namespace Engine
{
    namespace Math
    {
        using namespace Kernel;
        Plane::Plane() noexcept
            : Normal(0.0f, 1.0f, 0.0f), Distance(0.0f)
        {
        }

        Plane::Plane(const Vector3& Normal, Types::f32 Distance) noexcept
            : Normal(Normal.Normalized()), Distance(Distance)
        {
        }

        Plane Plane::FromPointNormal(const Vector3& Point, const Vector3& Normal) noexcept
        {
            const Vector3 NormalizedNormal = Normal.Normalized();
            const Types::f32 PlaneDistance = NormalizedNormal.Dot(Point);

            return Plane(NormalizedNormal, PlaneDistance);
        }

        Types::f32 Plane::DistanceToPoint(const Vector3& Point) const noexcept
        {
            return Normal.Dot(Point) - Distance;
        }

        Types::b8 Plane::operator==(const Plane& Other) const noexcept
        {
            return Normal == Other.Normal && Distance == Other.Distance;
        }

        Types::b8 Plane::operator!=(const Plane& Other) const noexcept
        {
            return !(*this == Other);
        }
    }
}
