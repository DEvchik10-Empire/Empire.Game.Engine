#include "Empire.Math/Math.Vector/Vector.Vector2.hpp"

#include <cmath>

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireMath
        {
            namespace MathVector
            {
                using namespace EmpireKernel;

                KernelTypes::Float Vector2::Length() const noexcept
                {
                    return std::sqrt(LengthSquared());
                }

                KernelTypes::Float Vector2::LengthSquared() const noexcept
                {
                    return X * X + Y * Y;
                }

                Vector2 Vector2::Normalized() const noexcept
                {
                    const KernelTypes::Float Length = this->Length();

                    if (Length == 0.0f)
                        return {};

                    return *this / Length;
                }

                KernelTypes::Float Vector2::Dot(const Vector2& Other) const noexcept
                {
                    return X * Other.X + Y * Other.Y;
                }

                KernelTypes::Float Vector2::Distance(const Vector2& Other) const noexcept
                {
                    return (*this - Other).Length();
                }

                KernelTypes::Float Vector2::DistanceSquared(const Vector2& Other) const noexcept
                {
                    return (*this - Other).LengthSquared();
                }
            }
        }
    }
}
