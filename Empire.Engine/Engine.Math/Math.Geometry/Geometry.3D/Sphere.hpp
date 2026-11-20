#pragma once

#include "Empire.Engine/Engine.Kernel/Kernel.Types/Types.hpp"
#include "Empire.Engine/Engine.Math/Math.Scalar/Scalar.hpp"
#include "Empire.Engine/Engine.Math/Math.Vector/Float3.hpp"
#include "Empire.Engine/Engine.Math/Math.Vector/Vector3.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireEngine
        {
            namespace EngineMath
            {
                namespace Geometry3D
                {
                    using namespace EngineKernel;
                    using namespace MathVector;

                    class AABB;

                    class [[nodiscard]] Sphere
                    {
                    public:
                        Sphere() noexcept = default;
                        Sphere(const Float3& Center, KernelTypes::Float Radius) noexcept
                            : mCenter(Center), mRadius(Radius)
                        {
                        }
                        Sphere(const Vector3& Center, KernelTypes::Float Radius) noexcept
                            : mRadius(Radius)
                        {
                            Center.StoreFloat3(&mCenter);
                        }

                        [[nodiscard]] Vector3 GetSupport(const Vector3& Direction) const noexcept
                        {
                            const KernelTypes::Float Length = Direction.Length();
                            const Vector3 Center = GetCenter();
                            return Length > 0.0f ? Center + (mRadius / Length) * Direction : Center;
                        }

                        [[nodiscard]] Vector3 GetCenter() const noexcept { return Vector3::sLoadFloat3Unsafe(mCenter); }
                        [[nodiscard]] KernelTypes::Float GetRadius() const noexcept { return mRadius; }

                        [[nodiscard]] KernelTypes::Bool Overlaps(const Sphere& Other) const noexcept
                        {
                            return (GetCenter() - Other.GetCenter()).LengthSq()
                                <= MathScalar::Square(mRadius + Other.mRadius);
                        }

                        [[nodiscard]] KernelTypes::Bool Overlaps(const AABB& Other) const noexcept;

                        void EncapsulatePoint(const Vector3& Point) noexcept
                        {
                            Vector3 Center = GetCenter();
                            const Vector3 Difference = Point - Center;
                            const KernelTypes::Float DistanceSquared = Difference.LengthSq();
                            if (DistanceSquared > MathScalar::Square(mRadius))
                            {
                                const KernelTypes::Float Distance = MathScalar::Sqrt(DistanceSquared);
                                const KernelTypes::Float Radius = 0.5f * (mRadius + Distance);
                                Center += ((Radius - mRadius) / Distance) * Difference;
                                Center.StoreFloat3(&mCenter);
                                mRadius = Radius;
                            }
                        }

                    private:
                        Float3 mCenter;
                        KernelTypes::Float mRadius = 0.0f;
                    };
                }
            }
        }
    }
}
