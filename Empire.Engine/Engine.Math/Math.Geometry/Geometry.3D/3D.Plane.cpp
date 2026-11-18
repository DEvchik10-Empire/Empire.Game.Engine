#include "Empire.Engine/Engine.Math/Math.Geometry/Geometry.3D/3D.Plane.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireEngine
        {
            namespace EngineMath
            {
                namespace MathGeometry
                {
                    namespace Geometry3D
                    {
                        using namespace EngineKernel;

                        Plane::Plane() noexcept
                            : Normal(0.0f, 1.0f, 0.0f), Distance(0.0f)
                        {
                        }

                        Plane::Plane(const Vector3& Normal, KernelTypes::Float Distance) noexcept
                            : Normal(Normal.Normalized()), Distance(Distance)
                        {
                        }

                        Plane Plane::FromPointNormal(const Vector3& Point, const Vector3& Normal) noexcept
                        {
                            const Vector3 NormalizedNormal = Normal.Normalized();
                            const KernelTypes::Float PlaneDistance = NormalizedNormal.Dot(Point);

                            return Plane(NormalizedNormal, PlaneDistance);
                        }

                        KernelTypes::Float Plane::DistanceToPoint(const Vector3& Point) const noexcept
                        {
                            return Normal.Dot(Point) - Distance;
                        }

                        KernelTypes::Bool Plane::operator==(const Plane& Other) const noexcept
                        {
                            return Normal == Other.Normal && Distance == Other.Distance;
                        }

                        KernelTypes::Bool Plane::operator!=(const Plane& Other) const noexcept
                        {
                            return !(*this == Other);
                        }
                    }
                }
            }
        }
    }
}
