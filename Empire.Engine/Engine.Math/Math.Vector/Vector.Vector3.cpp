#include "Empire.Engine/Engine.Math/Math.Vector/Vector.Vector3.hpp"

#include <cmath>

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireEngine
        {
            namespace EngineMath
            {
                namespace MathVector
                {
                    using namespace EngineKernel;

                    KernelTypes::Float Vector3::Length() const noexcept
                    {
                        return std::sqrt(LengthSquared());
                    }

                    KernelTypes::Float Vector3::LengthSquared() const noexcept
                    {
                        return X * X + Y * Y + Z * Z;
                    }

                    Vector3 Vector3::Normalized() const noexcept
                    {
                        const KernelTypes::Float Length = this->Length();

                        if (Length == 0.0f)
                            return {};

                        return *this / Length;
                    }

                    KernelTypes::Float Vector3::Dot(const Vector3& Other) const noexcept
                    {
                        return X * Other.X +
                               Y * Other.Y +
                               Z * Other.Z;
                    }

                    Vector3 Vector3::Cross(const Vector3& Other) const noexcept
                    {
                        return {
                            Y * Other.Z - Z * Other.Y,
                            Z * Other.X - X * Other.Z,
                            X * Other.Y - Y * Other.X
                        };
                    }

                    KernelTypes::Float Vector3::Distance(const Vector3& Other) const noexcept
                    {
                        return (*this - Other).Length();
                    }

                    KernelTypes::Float Vector3::DistanceSquared(const Vector3& Other) const noexcept
                    {
                        return (*this - Other).LengthSquared();
                    }
                }
            }
        }
    }
}
