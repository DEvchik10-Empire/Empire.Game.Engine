#include "Sphere.hpp"

namespace Math
{
    Sphere::Sphere() noexcept
        : Center(0.0f, 0.0f, 0.0f), Radius(1.0f)
    {
    }

    Sphere::Sphere(const Vector3& Center, f32 Radius) noexcept
        : Center(Center), Radius(Radius)
    {
    }

    b8 Sphere::Contains(const Vector3& Point) const noexcept
    {
        return Center.DistanceSquared(Point) <= Radius * Radius;
    }

    b8 Sphere::Intersects(const Sphere& Other) const noexcept
    {
        const f32 RadiusSum = Radius + Other.Radius;
        return Center.DistanceSquared(Other.Center) <= RadiusSum * RadiusSum;
    }

    b8 Sphere::operator==(const Sphere& Other) const noexcept
    {
        return Center == Other.Center && Radius == Other.Radius;
    }

    b8 Sphere::operator!=(const Sphere& Other) const noexcept
    {
        return !(*this == Other);
    }
}