#pragma once

#include "Empire.Engine/Engine.Kernel/Kernel.Compiler/Compiler.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Containers/Containers.StaticArray.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Types/Types.hpp"
#include "Empire.Engine/Engine.Math/Math.Geometry/Geometry.3D/AABB.hpp"
#include "Empire.Engine/Engine.Math/Math.Vector/UVector4.hpp"
#include "Empire.Engine/Engine.Math/Math.Vector/Vector3.hpp"
#include "Empire.Engine/Engine.Math/Math.Vector/Vector4.hpp"

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

                    class RayInvDirection
                    {
                    public:
                        RayInvDirection() noexcept = default;
                        explicit RayInvDirection(const Vector3& Direction) noexcept { Set(Direction); }

                        void Set(const Vector3& Direction) noexcept
                        {
                            constexpr KernelTypes::Float ParallelEpsilon = 1.0e-20f;
                            mIsParallel = UVector4(
                                Direction.GetX() >= -ParallelEpsilon && Direction.GetX() <= ParallelEpsilon,
                                Direction.GetY() >= -ParallelEpsilon && Direction.GetY() <= ParallelEpsilon,
                                Direction.GetZ() >= -ParallelEpsilon && Direction.GetZ() <= ParallelEpsilon,
                                true);
                            mInvDirection = Vector3(
                                mIsParallel.GetX() ? 1.0f : 1.0f / Direction.GetX(),
                                mIsParallel.GetY() ? 1.0f : 1.0f / Direction.GetY(),
                                mIsParallel.GetZ() ? 1.0f : 1.0f / Direction.GetZ());
                        }

                        Vector3 mInvDirection;
                        UVector4 mIsParallel;
                    };

                    [[nodiscard]] inline KernelTypes::Float RayAABox(
                        const Vector3& Origin, const RayInvDirection& InverseDirection,
                        const Vector3& BoundsMin, const Vector3& BoundsMax) noexcept
                    {
                        KernelTypes::Float Minimum = -KernelCompiler::BitCast<KernelTypes::Float>(0x7f7fffffu);
                        KernelTypes::Float Maximum = KernelCompiler::BitCast<KernelTypes::Float>(0x7f7fffffu);
                        const Vector3& Reciprocal = InverseDirection.mInvDirection;

                        for (KernelTypes::UInt32 Axis = 0; Axis < 3; ++Axis)
                        {
                            if (InverseDirection.mIsParallel[Axis])
                            {
                                if (Origin[Axis] < BoundsMin[Axis] || Origin[Axis] > BoundsMax[Axis])
                                    return KernelCompiler::BitCast<KernelTypes::Float>(0x7f7fffffu);
                                continue;
                            }

                            const KernelTypes::Float First = (BoundsMin[Axis] - Origin[Axis]) * Reciprocal[Axis];
                            const KernelTypes::Float Second = (BoundsMax[Axis] - Origin[Axis]) * Reciprocal[Axis];
                            const KernelTypes::Float AxisMinimum = First < Second ? First : Second;
                            const KernelTypes::Float AxisMaximum = First > Second ? First : Second;
                            Minimum = Minimum < AxisMinimum ? AxisMinimum : Minimum;
                            Maximum = Maximum > AxisMaximum ? AxisMaximum : Maximum;
                        }

                        if (Minimum > Maximum || Maximum < 0.0f)
                            return KernelCompiler::BitCast<KernelTypes::Float>(0x7f7fffffu);
                        return Minimum;
                    }

                    [[nodiscard]] inline KernelTypes::Float RayAABox(
                        const Vector3& Origin, const RayInvDirection& InverseDirection, const AABB& Bounds) noexcept
                    {
                        return RayAABox(Origin, InverseDirection, Bounds.mMin, Bounds.mMax);
                    }

                    [[nodiscard]] inline Vector4 RayAABox4(const Vector3& Origin,
                        const RayInvDirection& InverseDirection,
                        const Vector4& BoundsMinX, const Vector4& BoundsMinY, const Vector4& BoundsMinZ,
                        const Vector4& BoundsMaxX, const Vector4& BoundsMaxY, const Vector4& BoundsMaxZ) noexcept
                    {
                        KernelContainers::StaticArray<KernelTypes::Float, 4> Fractions;
                        for (KernelTypes::UInt32 Lane = 0; Lane < 4; ++Lane)
                        {
                            Fractions.EmplaceBack(RayAABox(Origin, InverseDirection,
                                Vector3(BoundsMinX[Lane], BoundsMinY[Lane], BoundsMinZ[Lane]),
                                Vector3(BoundsMaxX[Lane], BoundsMaxY[Lane], BoundsMaxZ[Lane])));
                        }
                        return Vector4(Fractions[0], Fractions[1], Fractions[2], Fractions[3]);
                    }

                    inline void RayAABox(
                        const Vector3& Origin, const RayInvDirection& InverseDirection,
                        const Vector3& BoundsMin, const Vector3& BoundsMax,
                        KernelTypes::Float& OutMinimum, KernelTypes::Float& OutMaximum) noexcept
                    {
                        const KernelTypes::Float Hit = RayAABox(Origin, InverseDirection, BoundsMin, BoundsMax);
                        if (Hit == KernelCompiler::BitCast<KernelTypes::Float>(0x7f7fffffu))
                        {
                            OutMinimum = KernelCompiler::BitCast<KernelTypes::Float>(0x7f7fffffu);
                            OutMaximum = -OutMinimum;
                            return;
                        }

                        OutMinimum = -KernelCompiler::BitCast<KernelTypes::Float>(0x7f7fffffu);
                        OutMaximum = KernelCompiler::BitCast<KernelTypes::Float>(0x7f7fffffu);
                        for (KernelTypes::UInt32 Axis = 0; Axis < 3; ++Axis)
                        {
                            if (InverseDirection.mIsParallel[Axis])
                                continue;
                            const KernelTypes::Float First =
                                (BoundsMin[Axis] - Origin[Axis]) * InverseDirection.mInvDirection[Axis];
                            const KernelTypes::Float Second =
                                (BoundsMax[Axis] - Origin[Axis]) * InverseDirection.mInvDirection[Axis];
                            const KernelTypes::Float AxisMinimum = First < Second ? First : Second;
                            const KernelTypes::Float AxisMaximum = First > Second ? First : Second;
                            OutMinimum = OutMinimum > AxisMinimum ? OutMinimum : AxisMinimum;
                            OutMaximum = OutMaximum < AxisMaximum ? OutMaximum : AxisMaximum;
                        }
                    }

                }
            }
        }
    }
}
