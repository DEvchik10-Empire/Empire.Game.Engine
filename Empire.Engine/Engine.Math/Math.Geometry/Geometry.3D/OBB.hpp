#pragma once

#include "Empire.Engine/Engine.Kernel/Kernel.Containers/Containers.StaticArray.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Types/Types.hpp"
#include "Empire.Engine/Engine.Math/Math.Geometry/Geometry.3D/AABB.hpp"
#include "Empire.Engine/Engine.Math/Math.Matrix/Matrix4.hpp"
#include "Empire.Engine/Engine.Math/Math.Scalar/Scalar.hpp"
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

                    class [[nodiscard]] OBB
                    {
                    public:
                        OBB() noexcept = default;
                        OBB(const Matrix4& Orientation, const Vector3& HalfExtents) noexcept
                            : mOrientation(Orientation), mHalfExtents(HalfExtents)
                        {
                        }
                        OBB(const Matrix4& Orientation, const AABB& Box) noexcept
                            : mOrientation(Orientation.PreTranslated(Box.GetCenter()))
                            , mHalfExtents(Box.GetExtent())
                        {
                        }

                        [[nodiscard]] KernelTypes::Bool Overlaps(const AABB& Box, KernelTypes::Float Epsilon = 1.0e-6f) const noexcept
                        {
                            const auto AxisA = KernelContainers::StaticArray<Vector3, 3>::Of(
                                Vector3::sAxisX(), Vector3::sAxisY(), Vector3::sAxisZ());
                            const auto AxisB = KernelContainers::StaticArray<Vector3, 3>::Of(
                                mOrientation.GetAxisX(), mOrientation.GetAxisY(), mOrientation.GetAxisZ());
                            return OverlapsAxes(Box.GetCenter(), Box.GetExtent(), AxisA,
                                mOrientation.GetTranslation(), mHalfExtents, AxisB, Epsilon);
                        }

                        [[nodiscard]] KernelTypes::Bool Overlaps(const OBB& Other, KernelTypes::Float Epsilon = 1.0e-6f) const noexcept
                        {
                            const auto AxisA = KernelContainers::StaticArray<Vector3, 3>::Of(
                                mOrientation.GetAxisX(), mOrientation.GetAxisY(), mOrientation.GetAxisZ());
                            const auto AxisB = KernelContainers::StaticArray<Vector3, 3>::Of(
                                Other.mOrientation.GetAxisX(), Other.mOrientation.GetAxisY(), Other.mOrientation.GetAxisZ());
                            return OverlapsAxes(mOrientation.GetTranslation(), mHalfExtents, AxisA,
                                Other.mOrientation.GetTranslation(), Other.mHalfExtents, AxisB, Epsilon);
                        }

                        Matrix4 mOrientation;
                        Vector3 mHalfExtents;

                    private:
                        [[nodiscard]] static KernelTypes::Bool OverlapsAxes(
                            const Vector3& CenterA, const Vector3& HalfA, const KernelContainers::StaticArray<Vector3, 3>& AxisA,
                            const Vector3& CenterB, const Vector3& HalfB, const KernelContainers::StaticArray<Vector3, 3>& AxisB,
                            KernelTypes::Float Epsilon) noexcept
                        {
                            Matrix4 Rotation = Matrix4::sIdentity();
                            Matrix4 AbsoluteRotation = Matrix4::sIdentity();
                            for (KernelTypes::UInt32 I = 0; I < 3; ++I)
                                for (KernelTypes::UInt32 J = 0; J < 3; ++J)
                                {
                                    Rotation(I, J) = AxisA[I].Dot(AxisB[J]);
                                    AbsoluteRotation(I, J) = MathScalar::Abs(Rotation(I, J)) + Epsilon;
                                }

                            const Vector3 Difference = CenterB - CenterA;
                            const Vector3 Translation(
                                Difference.Dot(AxisA[0]), Difference.Dot(AxisA[1]), Difference.Dot(AxisA[2]));

                            for (KernelTypes::UInt32 I = 0; I < 3; ++I)
                            {
                                const KernelTypes::Float RadiusB =
                                    HalfB[0] * AbsoluteRotation(I, 0)
                                    + HalfB[1] * AbsoluteRotation(I, 1)
                                    + HalfB[2] * AbsoluteRotation(I, 2);
                                if (MathScalar::Abs(Translation[I]) > HalfA[I] + RadiusB)
                                    return false;
                            }

                            for (KernelTypes::UInt32 J = 0; J < 3; ++J)
                            {
                                const KernelTypes::Float RadiusA =
                                    HalfA[0] * AbsoluteRotation(0, J)
                                    + HalfA[1] * AbsoluteRotation(1, J)
                                    + HalfA[2] * AbsoluteRotation(2, J);
                                if (MathScalar::Abs(Difference.Dot(AxisB[J])) > RadiusA + HalfB[J])
                                    return false;
                            }

                            for (KernelTypes::UInt32 I = 0; I < 3; ++I)
                            {
                                const KernelTypes::UInt32 K = (I + 1) % 3;
                                const KernelTypes::UInt32 L = (I + 2) % 3;
                                for (KernelTypes::UInt32 J = 0; J < 3; ++J)
                                {
                                    const KernelTypes::UInt32 NextJ = (J + 1) % 3;
                                    const KernelTypes::UInt32 LastJ = (J + 2) % 3;
                                    const KernelTypes::Float RadiusA =
                                        HalfA[K] * AbsoluteRotation(L, J)
                                        + HalfA[L] * AbsoluteRotation(K, J);
                                    const KernelTypes::Float RadiusB =
                                        HalfB[NextJ] * AbsoluteRotation(I, LastJ)
                                        + HalfB[LastJ] * AbsoluteRotation(I, NextJ);
                                    const KernelTypes::Float Distance =
                                        MathScalar::Abs(Translation[L] * Rotation(K, J)
                                            - Translation[K] * Rotation(L, J));
                                    if (Distance > RadiusA + RadiusB)
                                        return false;
                                }
                            }
                            return true;
                        }
                    };
                }
            }
        }
    }
}
