#pragma once

#include "Empire.Engine/Engine.Kernel/Kernel.Types/Types.hpp"
#include "Empire.Engine/Engine.Math/Math.Matrix/Matrix4.hpp"
#include "Empire.Engine/Engine.Math/Math.Vector/Float4.hpp"
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
                    using namespace MathMatrix;
                    using namespace MathVector;

                    class [[nodiscard]] Plane
                    {
                    public:
                        Plane() noexcept = default;
                        explicit Plane(const Float4& NormalAndConstant) noexcept
                            : mNormal(NormalAndConstant.X, NormalAndConstant.Y, NormalAndConstant.Z)
                            , mConstant(NormalAndConstant.W)
                        {
                        }
                        Plane(const Vector3& Normal, KernelTypes::Float Constant) noexcept
                            : mNormal(Normal), mConstant(Constant)
                        {
                        }

                        [[nodiscard]] static Plane sFromPointAndNormal(const Vector3& Point, const Vector3& Normal) noexcept
                        {
                            return Plane(Normal, -Normal.Dot(Point));
                        }

                        [[nodiscard]] static Plane sFromPointsCCW(const Vector3& Point1, const Vector3& Point2, const Vector3& Point3) noexcept
                        {
                            return sFromPointAndNormal(Point1, (Point2 - Point1).Cross(Point3 - Point1).Normalized());
                        }

                        [[nodiscard]] Vector3 GetNormal() const noexcept { return mNormal; }
                        void SetNormal(const Vector3& Normal) noexcept { mNormal = Normal; }
                        [[nodiscard]] KernelTypes::Float GetConstant() const noexcept { return mConstant; }
                        void SetConstant(KernelTypes::Float Constant) noexcept { mConstant = Constant; }

                        [[nodiscard]] Float4 GetFloat4() const noexcept
                        {
                            return Float4(mNormal.GetX(), mNormal.GetY(), mNormal.GetZ(), mConstant);
                        }
                        void StoreFloat4(Float4* Output) const noexcept
                        {
                            *Output = GetFloat4();
                        }

                        [[nodiscard]] Plane Offset(KernelTypes::Float Distance) const noexcept
                        {
                            return Plane(mNormal, mConstant - Distance);
                        }

                        [[nodiscard]] Plane GetTransformed(const Matrix4& Transform) const noexcept
                        {
                            const Vector3 TransformedNormal = Transform.Multiply3x3(mNormal);
                            return Plane(TransformedNormal, mConstant - Transform.GetTranslation().Dot(TransformedNormal));
                        }

                        [[nodiscard]] Plane Scaled(const Vector3& Scale) const noexcept
                        {
                            const Vector3 ScaledNormal = mNormal / Scale;
                            const KernelTypes::Float NormalLength = ScaledNormal.Length();
                            return Plane(ScaledNormal / NormalLength, mConstant / NormalLength);
                        }

                        [[nodiscard]] KernelTypes::Float SignedDistance(const Vector3& Point) const noexcept
                        {
                            return Point.Dot(mNormal) + mConstant;
                        }

                        [[nodiscard]] Vector3 ProjectPointOnPlane(const Vector3& Point) const noexcept
                        {
                            return Point - mNormal * SignedDistance(Point);
                        }

                        [[nodiscard]] static KernelTypes::Bool sIntersectPlanes(
                            const Plane& Plane1, const Plane& Plane2, const Plane& Plane3, Vector3& Point) noexcept
                        {
                            const Vector3 Cross23 = Plane2.mNormal.Cross(Plane3.mNormal);
                            const Vector3 Cross31 = Plane3.mNormal.Cross(Plane1.mNormal);
                            const Vector3 Cross12 = Plane1.mNormal.Cross(Plane2.mNormal);
                            const KernelTypes::Float Denominator = Plane1.mNormal.Dot(Cross23);
                            if (Denominator == 0.0f)
                                return false;

                            Point = (-Plane1.mConstant * Cross23
                                - Plane2.mConstant * Cross31
                                - Plane3.mConstant * Cross12) / Denominator;
                            return true;
                        }

                    private:
                        Vector3 mNormal;
                        KernelTypes::Float mConstant = 0.0f;
                    };
                }
            }
        }
    }
}
