#include "Empire.Math/Math.Vector/Vector.Vector4.hpp"

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

                KernelTypes::Float Vector4::Length() const noexcept
                {
                    return std::sqrt(LengthSquared());
                }

                KernelTypes::Float Vector4::LengthSquared() const noexcept
                {
                    return X * X +
                           Y * Y +
                           Z * Z +
                           W * W;
                }

                Vector4 Vector4::Normalized() const noexcept
                {
                    const KernelTypes::Float Length = this->Length();

                    if (Length == 0.0f)
                        return {};

                    return *this / Length;
                }

                KernelTypes::Float Vector4::Dot(const Vector4& Other) const noexcept
                {
                    return X * Other.X +
                           Y * Other.Y +
                           Z * Other.Z +
                           W * Other.W;
                }

                KernelTypes::Float Vector4::Distance(const Vector4& Other) const noexcept
                {
                    return (*this - Other).Length();
                }

                KernelTypes::Float Vector4::DistanceSquared(const Vector4& Other) const noexcept
                {
                    return (*this - Other).LengthSquared();
                }
            }
        }
    }
}
