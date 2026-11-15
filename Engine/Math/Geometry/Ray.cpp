#include "Ray.hpp"

namespace Engine
{
    namespace Math
    {
        using namespace Kernel;
        Ray::Ray() noexcept
            : Origin(0.0f, 0.0f, 0.0f), Direction(0.0f, 0.0f, -1.0f)
        {
        }

        Ray::Ray(const Vector3& Origin, const Vector3& Direction) noexcept
            : Origin(Origin), Direction(Direction.Normalized())
        {
        }

        Vector3 Ray::GetPoint(Types::f32 Distance) const noexcept
        {
            return Origin + Direction * Distance;
        }

        Types::b8 Ray::operator==(const Ray& Other) const noexcept
        {
            return Origin == Other.Origin && Direction == Other.Direction;
        }

        Types::b8 Ray::operator!=(const Ray& Other) const noexcept
        {
            return !(*this == Other);
        }
    }
}
