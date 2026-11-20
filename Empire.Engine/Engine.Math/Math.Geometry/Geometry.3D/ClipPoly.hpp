#pragma once

#include "Empire.Engine/Engine.Kernel/Kernel.Assert/Assert.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Containers/Containers.Array.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Types/Types.hpp"
#include "Empire.Engine/Engine.Math/Math.Geometry/Geometry.3D/AABB.hpp"
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

                    template <typename VertexArray>
                    void ClipPolyVsPlane(const VertexArray& Polygon, const Vector3& PlaneOrigin,
                        const Vector3& PlaneNormal, VertexArray& ClippedPolygon)
                    {
                        KernelAssert::DebugCheck(Polygon.GetCount() >= 2, "Polygon has at least two vertices");
                        KernelAssert::DebugCheck(ClippedPolygon.GetCount() == 0, "Output polygon is empty");
                        Vector3 Previous = Polygon[Polygon.GetCount() - 1];
                        KernelTypes::Float PreviousDistance = (PlaneOrigin - Previous).Dot(PlaneNormal);
                        KernelTypes::Bool PreviousInside = PreviousDistance < 0.0f;
                        for (KernelTypes::Size Index = 0; Index < Polygon.GetCount(); ++Index)
                        {
                            const Vector3 Current = Polygon[Index];
                            const KernelTypes::Float CurrentDistance = (PlaneOrigin - Current).Dot(PlaneNormal);
                            KernelTypes::Bool CurrentInside = CurrentDistance < 0.0f;
                            if (CurrentInside != PreviousInside)
                            {
                                const Vector3 Edge = Current - Previous;
                                const KernelTypes::Float Denominator = Edge.Dot(PlaneNormal);
                                if (Denominator != 0.0f)
                                    ClippedPolygon.Add(Previous + (PreviousDistance / Denominator) * Edge);
                                else
                                    CurrentInside = PreviousInside;
                            }
                            if (CurrentInside)
                                ClippedPolygon.Add(Current);
                            PreviousDistance = CurrentDistance;
                            PreviousInside = CurrentInside;
                            Previous = Current;
                        }
                    }

                    template <typename VertexArray>
                    void ClipPolyVsPoly(const VertexArray& Polygon, const VertexArray& ClippingPolygon,
                        const Vector3& ClippingNormal, VertexArray& ClippedPolygon)
                    {
                        KernelAssert::DebugCheck(Polygon.GetCount() >= 2, "Input polygon has at least two vertices");
                        KernelAssert::DebugCheck(ClippingPolygon.GetCount() >= 3, "Clip polygon has at least three vertices");
                        VertexArray TemporaryA;
                        VertexArray TemporaryB;
                        const VertexArray* Source = &Polygon;
                        VertexArray* Target = &TemporaryA;
                        for (KernelTypes::Size Index = 0; Index < ClippingPolygon.GetCount(); ++Index)
                        {
                            const Vector3 EdgeStart = ClippingPolygon[Index];
                            const Vector3 EdgeEnd = ClippingPolygon[(Index + 1) % ClippingPolygon.GetCount()];
                            const Vector3 EdgeNormal = ClippingNormal.Cross(EdgeEnd - EdgeStart);
                            const KernelTypes::Bool Last = Index + 1 == ClippingPolygon.GetCount();
                            if (Last)
                                Target = &ClippedPolygon;
                            else
                                Target = Target == &TemporaryA ? &TemporaryB : &TemporaryA;
                            Target->Clear();
                            ClipPolyVsPlane(*Source, EdgeStart, EdgeNormal, *Target);
                            if (Target->GetCount() < 3)
                            {
                                ClippedPolygon.Clear();
                                return;
                            }
                            Source = Target;
                        }
                    }

                    template <typename VertexArray>
                    void ClipPolyVsEdge(const VertexArray& Polygon, const Vector3& EdgeVertex1,
                        const Vector3& EdgeVertex2, const Vector3& ClippingEdgeNormal,
                        VertexArray& ClippedPolygon)
                    {
                        KernelAssert::DebugCheck(Polygon.GetCount() >= 3, "Input polygon has at least three vertices");
                        KernelAssert::DebugCheck(ClippedPolygon.GetCount() == 0, "Output polygon is empty");
                        const Vector3 Edge = EdgeVertex2 - EdgeVertex1;
                        const Vector3 EdgeNormal = ClippingEdgeNormal.Cross(Edge);
                        const Vector3 PolygonNormal =
                            (Polygon[2] - Polygon[0]).Cross(Polygon[1] - Polygon[0]);
                        const KernelTypes::Float PolygonNormalLengthSquared = PolygonNormal.LengthSq();
                        const Vector3 V1 = EdgeVertex1
                            + PolygonNormal.Dot(Polygon[0] - EdgeVertex1) * PolygonNormal / PolygonNormalLengthSquared;
                        const Vector3 V2 = EdgeVertex2
                            + PolygonNormal.Dot(Polygon[0] - EdgeVertex2) * PolygonNormal / PolygonNormalLengthSquared;
                        const Vector3 V12 = V2 - V1;
                        const KernelTypes::Float V12LengthSquared = V12.LengthSq();

                        Vector3 Previous = Polygon[Polygon.GetCount() - 1];
                        KernelTypes::Float PreviousDistance = (EdgeVertex1 - Previous).Dot(EdgeNormal);
                        KernelTypes::Bool PreviousInside = PreviousDistance < 0.0f;
                        for (KernelTypes::Size Index = 0; Index < Polygon.GetCount(); ++Index)
                        {
                            const Vector3 Current = Polygon[Index];
                            const KernelTypes::Float CurrentDistance = (EdgeVertex1 - Current).Dot(EdgeNormal);
                            const KernelTypes::Bool CurrentInside = CurrentDistance < 0.0f;
                            if (CurrentInside != PreviousInside)
                            {
                                const Vector3 Segment = Current - Previous;
                                const KernelTypes::Float Denominator = Segment.Dot(EdgeNormal);
                                const Vector3 Intersection = Denominator != 0.0f
                                    ? Previous + (PreviousDistance / Denominator) * Segment : Previous;
                                const KernelTypes::Float Projection = (Intersection - V1).Dot(V12);
                                ClippedPolygon.Add(Projection < 0.0f ? V1
                                    : (Projection > V12LengthSquared ? V2 : Intersection));
                            }
                            PreviousDistance = CurrentDistance;
                            PreviousInside = CurrentInside;
                            Previous = Current;
                        }
                    }

                    template <typename VertexArray>
                    void ClipPolyVsAABB(const VertexArray& Polygon, const AABB& Bounds, VertexArray& ClippedPolygon)
                    {
                        KernelAssert::DebugCheck(Polygon.GetCount() >= 2, "Input polygon has at least two vertices");
                        VertexArray TemporaryA;
                        VertexArray TemporaryB;
                        const VertexArray* Source = &Polygon;
                        VertexArray* Target = &TemporaryA;
                        for (KernelTypes::UInt32 Axis = 0; Axis < 3; ++Axis)
                        {
                            for (KernelTypes::UInt32 Side = 0; Side < 2; ++Side)
                            {
                                Vector3 Origin = Vector3::sZero();
                                Vector3 Normal = Vector3::sZero();
                                Origin.SetComponent(Axis, Side == 0 ? Bounds.mMin[Axis] : Bounds.mMax[Axis]);
                                Normal.SetComponent(Axis, Side == 0 ? 1.0f : -1.0f);

                                const KernelTypes::Bool Last = Axis == 2 && Side == 1;
                                if (Last)
                                    Target = &ClippedPolygon;
                                else
                                    Target = Target == &TemporaryA ? &TemporaryB : &TemporaryA;
                                Target->Clear();
                                ClipPolyVsPlane(*Source, Origin, Normal, *Target);
                                if (Target->GetCount() < 3)
                                {
                                    ClippedPolygon.Clear();
                                    return;
                                }
                                Source = Target;
                            }
                        }
                    }
                }
            }
        }
    }
}
