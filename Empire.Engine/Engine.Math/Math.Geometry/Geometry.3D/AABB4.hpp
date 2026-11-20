#pragma once

#include "Empire.Engine/Engine.Kernel/Kernel.Containers/Containers.StaticArray.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Types/Types.hpp"
#include "Empire.Engine/Engine.Math/Math.Geometry/Geometry.3D/AABB.hpp"
#include "Empire.Engine/Engine.Math/Math.Geometry/Geometry.3D/OBB.hpp"
#include "Empire.Engine/Engine.Math/Math.Vector/UVector4.hpp"
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

                    [[nodiscard]] inline UVector4 AABB4VsBox(const AABB& Box,
                        const Vector4& BoxMinX, const Vector4& BoxMinY, const Vector4& BoxMinZ,
                        const Vector4& BoxMaxX, const Vector4& BoxMaxY, const Vector4& BoxMaxZ) noexcept
                    {
                        const UVector4 NoOverlapX = UVector4::sOr(
                            Vector4::sGreater(Vector4::sReplicate(Box.mMin.GetX()), BoxMaxX),
                            Vector4::sGreater(BoxMinX, Vector4::sReplicate(Box.mMax.GetX())));
                        const UVector4 NoOverlapY = UVector4::sOr(
                            Vector4::sGreater(Vector4::sReplicate(Box.mMin.GetY()), BoxMaxY),
                            Vector4::sGreater(BoxMinY, Vector4::sReplicate(Box.mMax.GetY())));
                        const UVector4 NoOverlapZ = UVector4::sOr(
                            Vector4::sGreater(Vector4::sReplicate(Box.mMin.GetZ()), BoxMaxZ),
                            Vector4::sGreater(BoxMinZ, Vector4::sReplicate(Box.mMax.GetZ())));
                        return UVector4::sNot(UVector4::sOr(UVector4::sOr(NoOverlapX, NoOverlapY), NoOverlapZ));
                    }

                    inline void AABB4Scale(const Vector3& Scale,
                        const Vector4& BoxMinX, const Vector4& BoxMinY, const Vector4& BoxMinZ,
                        const Vector4& BoxMaxX, const Vector4& BoxMaxY, const Vector4& BoxMaxZ,
                        Vector4& BoundsMinX, Vector4& BoundsMinY, Vector4& BoundsMinZ,
                        Vector4& BoundsMaxX, Vector4& BoundsMaxY, Vector4& BoundsMaxZ) noexcept
                    {
                        const Vector4 ScaledMinX = Scale.GetX() * BoxMinX;
                        const Vector4 ScaledMaxX = Scale.GetX() * BoxMaxX;
                        const Vector4 ScaledMinY = Scale.GetY() * BoxMinY;
                        const Vector4 ScaledMaxY = Scale.GetY() * BoxMaxY;
                        const Vector4 ScaledMinZ = Scale.GetZ() * BoxMinZ;
                        const Vector4 ScaledMaxZ = Scale.GetZ() * BoxMaxZ;
                        BoundsMinX = Vector4::sMin(ScaledMinX, ScaledMaxX);
                        BoundsMaxX = Vector4::sMax(ScaledMinX, ScaledMaxX);
                        BoundsMinY = Vector4::sMin(ScaledMinY, ScaledMaxY);
                        BoundsMaxY = Vector4::sMax(ScaledMinY, ScaledMaxY);
                        BoundsMinZ = Vector4::sMin(ScaledMinZ, ScaledMaxZ);
                        BoundsMaxZ = Vector4::sMax(ScaledMinZ, ScaledMaxZ);
                    }

                    inline void AABB4EnlargeWithExtent(const Vector3& Extent,
                        Vector4& BoundsMinX, Vector4& BoundsMinY, Vector4& BoundsMinZ,
                        Vector4& BoundsMaxX, Vector4& BoundsMaxY, Vector4& BoundsMaxZ) noexcept
                    {
                        BoundsMinX -= Vector4::sReplicate(Extent.GetX());
                        BoundsMaxX += Vector4::sReplicate(Extent.GetX());
                        BoundsMinY -= Vector4::sReplicate(Extent.GetY());
                        BoundsMaxY += Vector4::sReplicate(Extent.GetY());
                        BoundsMinZ -= Vector4::sReplicate(Extent.GetZ());
                        BoundsMaxZ += Vector4::sReplicate(Extent.GetZ());
                    }

                    [[nodiscard]] inline UVector4 AABB4VsPoint(const Vector3& Point,
                        const Vector4& BoxMinX, const Vector4& BoxMinY, const Vector4& BoxMinZ,
                        const Vector4& BoxMaxX, const Vector4& BoxMaxY, const Vector4& BoxMaxZ) noexcept
                    {
                        const UVector4 OverlapX = UVector4::sAnd(
                            Vector4::sGreaterOrEqual(Vector4::sReplicate(Point.GetX()), BoxMinX),
                            Vector4::sLessOrEqual(Vector4::sReplicate(Point.GetX()), BoxMaxX));
                        const UVector4 OverlapY = UVector4::sAnd(
                            Vector4::sGreaterOrEqual(Vector4::sReplicate(Point.GetY()), BoxMinY),
                            Vector4::sLessOrEqual(Vector4::sReplicate(Point.GetY()), BoxMaxY));
                        const UVector4 OverlapZ = UVector4::sAnd(
                            Vector4::sGreaterOrEqual(Vector4::sReplicate(Point.GetZ()), BoxMinZ),
                            Vector4::sLessOrEqual(Vector4::sReplicate(Point.GetZ()), BoxMaxZ));
                        return UVector4::sAnd(UVector4::sAnd(OverlapX, OverlapY), OverlapZ);
                    }

                    [[nodiscard]] inline Vector4 AABB4DistanceSqToPoint(
                        const Vector4& PointX, const Vector4& PointY, const Vector4& PointZ,
                        const Vector4& BoxMinX, const Vector4& BoxMinY, const Vector4& BoxMinZ,
                        const Vector4& BoxMaxX, const Vector4& BoxMaxY, const Vector4& BoxMaxZ) noexcept
                    {
                        const Vector4 ClosestX = Vector4::sMin(Vector4::sMax(PointX, BoxMinX), BoxMaxX);
                        const Vector4 ClosestY = Vector4::sMin(Vector4::sMax(PointY, BoxMinY), BoxMaxY);
                        const Vector4 ClosestZ = Vector4::sMin(Vector4::sMax(PointZ, BoxMinZ), BoxMaxZ);
                        const Vector4 DifferenceX = ClosestX - PointX;
                        const Vector4 DifferenceY = ClosestY - PointY;
                        const Vector4 DifferenceZ = ClosestZ - PointZ;
                        return DifferenceX * DifferenceX + DifferenceY * DifferenceY + DifferenceZ * DifferenceZ;
                    }

                    [[nodiscard]] inline Vector4 AABB4DistanceSqToPoint(const Vector3& Point,
                        const Vector4& BoxMinX, const Vector4& BoxMinY, const Vector4& BoxMinZ,
                        const Vector4& BoxMaxX, const Vector4& BoxMaxY, const Vector4& BoxMaxZ) noexcept
                    {
                        return AABB4DistanceSqToPoint(Vector4::sReplicate(Point.GetX()),
                            Vector4::sReplicate(Point.GetY()), Vector4::sReplicate(Point.GetZ()),
                            BoxMinX, BoxMinY, BoxMinZ, BoxMaxX, BoxMaxY, BoxMaxZ);
                    }

                    [[nodiscard]] inline UVector4 AABB4VsSphere(
                        const Vector4& CenterX, const Vector4& CenterY, const Vector4& CenterZ,
                        const Vector4& RadiusSquared, const Vector4& BoxMinX, const Vector4& BoxMinY,
                        const Vector4& BoxMinZ, const Vector4& BoxMaxX, const Vector4& BoxMaxY,
                        const Vector4& BoxMaxZ) noexcept
                    {
                        return Vector4::sLessOrEqual(AABB4DistanceSqToPoint(CenterX, CenterY, CenterZ,
                            BoxMinX, BoxMinY, BoxMinZ, BoxMaxX, BoxMaxY, BoxMaxZ), RadiusSquared);
                    }

                    [[nodiscard]] inline UVector4 AABB4VsSphere(const Vector3& Center,
                        KernelTypes::Float RadiusSquared, const Vector4& BoxMinX, const Vector4& BoxMinY,
                        const Vector4& BoxMinZ, const Vector4& BoxMaxX, const Vector4& BoxMaxY,
                        const Vector4& BoxMaxZ) noexcept
                    {
                        return AABB4VsSphere(Vector4::sReplicate(Center.GetX()),
                            Vector4::sReplicate(Center.GetY()), Vector4::sReplicate(Center.GetZ()),
                            Vector4::sReplicate(RadiusSquared), BoxMinX, BoxMinY, BoxMinZ,
                            BoxMaxX, BoxMaxY, BoxMaxZ);
                    }

                    [[nodiscard]] inline UVector4 AABB4VsOBB(const OBB& Box,
                        const Vector4& BoxMinX, const Vector4& BoxMinY, const Vector4& BoxMinZ,
                        const Vector4& BoxMaxX, const Vector4& BoxMaxY, const Vector4& BoxMaxZ,
                        KernelTypes::Float Epsilon = 1.0e-6f)
                    {
                        UVector4 Result = UVector4::sZero();
                        for (KernelTypes::UInt32 Lane = 0; Lane < 4; ++Lane)
                        {
                            const AABB Candidate(
                                Vector3(BoxMinX[Lane], BoxMinY[Lane], BoxMinZ[Lane]),
                                Vector3(BoxMaxX[Lane], BoxMaxY[Lane], BoxMaxZ[Lane]));
                            Result[Lane] = Box.Overlaps(Candidate, Epsilon) ? 0xffffffffu : 0u;
                        }
                        return Result;
                    }

                    [[nodiscard]] inline UVector4 AABB4VsBox(const OBB& Box,
                        const Vector4& BoxMinX, const Vector4& BoxMinY, const Vector4& BoxMinZ,
                        const Vector4& BoxMaxX, const Vector4& BoxMaxY, const Vector4& BoxMaxZ,
                        KernelTypes::Float Epsilon = 1.0e-6f)
                    {
                        return AABB4VsOBB(Box, BoxMinX, BoxMinY, BoxMinZ,
                            BoxMaxX, BoxMaxY, BoxMaxZ, Epsilon);
                    }
                }
            }
        }
    }
}
