#include "Empire.Math/Math.Geometry/Geometry.3D/3D.Ray.hpp"

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

                    Ray::Ray() noexcept
                        : Origin(0.0f, 0.0f, 0.0f), Direction(0.0f, 0.0f, -1.0f)
                    {
                    }

                    Ray::Ray(const Vector3& Origin, const Vector3& Direction) noexcept
                        : Origin(Origin), Direction(Direction.Normalized())
                    {
                    }

                    Vector3 Ray::GetPoint(KernelTypes::Float Distance) const noexcept
                    {
                        return Origin + Direction * Distance;
                    }

                    KernelTypes::Bool Ray::operator==(const Ray& Other) const noexcept
                    {
                        return Origin == Other.Origin && Direction == Other.Direction;
                    }

                    KernelTypes::Bool Ray::operator!=(const Ray& Other) const noexcept
                    {
                        return !(*this == Other);
                    }
                }
            }
        }
    }
}
