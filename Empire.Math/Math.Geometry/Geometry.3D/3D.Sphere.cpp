#include "Empire.Math/Math.Geometry/Geometry.3D/3D.Sphere.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireMath
        {
            namespace MathGeometry
            {
                namespace Geometry3D
                {
                    using namespace EmpireKernel;

                    Sphere::Sphere() noexcept
                        : Center(0.0f, 0.0f, 0.0f), Radius(1.0f)
                    {
                    }

                    Sphere::Sphere(const Vector3& Center, KernelTypes::Float Radius) noexcept
                        : Center(Center), Radius(Radius)
                    {
                    }

                    KernelTypes::Bool Sphere::Contains(const Vector3& Point) const noexcept
                    {
                        return Center.DistanceSquared(Point) <= Radius * Radius;
                    }

                    KernelTypes::Bool Sphere::Intersects(const Sphere& Other) const noexcept
                    {
                        const KernelTypes::Float RadiusSum = Radius + Other.Radius;
                        return Center.DistanceSquared(Other.Center) <= RadiusSum * RadiusSum;
                    }

                    KernelTypes::Bool Sphere::operator==(const Sphere& Other) const noexcept
                    {
                        return Center == Other.Center && Radius == Other.Radius;
                    }

                    KernelTypes::Bool Sphere::operator!=(const Sphere& Other) const noexcept
                    {
                        return !(*this == Other);
                    }
                }
            }
        }
    }
}
