// Jolt Physics Library (https://github.com/jrouwe/JoltPhysics)
// SPDX-FileCopyrightText: 2021 Jorrit Rouwe
// SPDX-License-Identifier: MIT

#pragma once

#include "Empire.Engine/Engine.Kernel/Kernel.Assert/Assert.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Containers/Containers.Array.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Types/Types.hpp"
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
                namespace Geometry2D
                {
                    using namespace EngineKernel;
                    using namespace MathScalar;
                    using namespace MathVector;

                    class ConvexHullBuilder2D
                    {
                    public:
                        using Positions = KernelContainers::Array<Vector3>;
                        using Edges = KernelContainers::Array<KernelTypes::Int32>;

                        enum class Result
                        {
                            Success,
                            MaxVerticesReached
                        };

                        explicit ConvexHullBuilder2D(const Positions& InputPositions) noexcept
                            : PositionsReference(InputPositions)
                        {
                        }

                        ConvexHullBuilder2D(const ConvexHullBuilder2D&) = delete;
                        ConvexHullBuilder2D& operator=(const ConvexHullBuilder2D&) = delete;

                        [[nodiscard]] Result Initialize(
                            KernelTypes::Int32 Index1,
                            KernelTypes::Int32 Index2,
                            KernelTypes::Int32 Index3,
                            KernelTypes::Int32 MaxVertices,
                            KernelTypes::Float Tolerance,
                            Edges& OutputEdges
                        )
                        {
                            EdgesState.Clear();
                            OutputEdges.Clear();
                            KernelAssert::DebugCheck(
                                Index1 >= 0 && Index2 >= 0 && Index3 >= 0
                                    && static_cast<KernelTypes::Size>(Index1) < PositionsReference.GetCount()
                                    && static_cast<KernelTypes::Size>(Index2) < PositionsReference.GetCount()
                                    && static_cast<KernelTypes::Size>(Index3) < PositionsReference.GetCount()
                                    && Index1 != Index2 && Index1 != Index3 && Index2 != Index3,
                                "Initial hull indices are distinct valid positions"
                            );
                            KernelAssert::DebugCheck(MaxVertices >= 3, "Maximum hull vertices is at least three");

                            KernelTypes::Float MaximumX = 0.0f;
                            KernelTypes::Float MaximumY = 0.0f;
                            for (const Vector3& Position : PositionsReference)
                            {
                                MaximumX = Max(MaximumX, Abs(Position.GetX()));
                                MaximumY = Max(MaximumY, Abs(Position.GetY()));
                            }
                            const KernelTypes::Float CollinearTolerance =
                                2.0f * 1.1920928955078125e-7f * (MaximumX + MaximumY);
                            const KernelTypes::Float CollinearToleranceSquared = Square(CollinearTolerance);
                            const KernelTypes::Float ToleranceSquared = Max(
                                CollinearToleranceSquared,
                                Square(Tolerance)
                            );

                            if (CrossZ(
                                    PositionsReference[static_cast<KernelTypes::Size>(Index2)]
                                        - PositionsReference[static_cast<KernelTypes::Size>(Index1)],
                                    PositionsReference[static_cast<KernelTypes::Size>(Index3)]
                                        - PositionsReference[static_cast<KernelTypes::Size>(Index1)]
                                ) < 0.0f)
                            {
                                KernelTypes::Swap(Index1, Index2);
                            }

                            AddEdge(Index1, 0, 2);
                            AddEdge(Index2, 1, 0);
                            AddEdge(Index3, 2, 1);
                            EdgesState[0].Next = 1;
                            EdgesState[1].Next = 2;
                            EdgesState[2].Next = 0;
                            EdgesState[0].Previous = 2;
                            EdgesState[1].Previous = 0;
                            EdgesState[2].Previous = 1;
                            FirstEdge = 0;
                            EdgeCount = 3;

                            for (KernelTypes::Size EdgeIndex = 0; EdgeIndex < 3; ++EdgeIndex)
                                CalculateNormalAndCenter(static_cast<KernelTypes::Int32>(EdgeIndex));

                            for (KernelTypes::Int32 Index = 0;
                                 static_cast<KernelTypes::Size>(Index) < PositionsReference.GetCount();
                                 ++Index)
                            {
                                if (Index != Index1 && Index != Index2 && Index != Index3)
                                    AssignPointToEdges(Index, 0, 1, 2);
                            }

                            Result BuildResult = Result::Success;
                            for (;;)
                            {
                                if (EdgeCount >= MaxVertices)
                                {
                                    BuildResult = Result::MaxVerticesReached;
                                    break;
                                }

                                KernelTypes::Int32 FurthestEdge = -1;
                                KernelTypes::Float FurthestDistanceSquared = 0.0f;
                                KernelTypes::Int32 EdgeIndex = FirstEdge;
                                do
                                {
                                    if (EdgesState[static_cast<KernelTypes::Size>(EdgeIndex)].FurthestDistanceSquared
                                        > FurthestDistanceSquared)
                                    {
                                        FurthestDistanceSquared =
                                            EdgesState[static_cast<KernelTypes::Size>(EdgeIndex)].FurthestDistanceSquared;
                                        FurthestEdge = EdgeIndex;
                                    }
                                    EdgeIndex = EdgesState[static_cast<KernelTypes::Size>(EdgeIndex)].Next;
                                } while (EdgeIndex != FirstEdge);

                                if (FurthestEdge < 0 || FurthestDistanceSquared < ToleranceSquared)
                                    break;

                                Edge& SelectedEdge = EdgesState[static_cast<KernelTypes::Size>(FurthestEdge)];
                                const KernelTypes::Int32 FurthestPointIndex = SelectedEdge.Conflict.Back();
                                SelectedEdge.Conflict.PopBack();
                                const Vector3 FurthestPoint =
                                    PositionsReference[static_cast<KernelTypes::Size>(FurthestPointIndex)];

                                KernelTypes::Int32 FirstVisibleEdge = FurthestEdge;
                                do
                                {
                                    const KernelTypes::Int32 Previous =
                                        EdgesState[static_cast<KernelTypes::Size>(FirstVisibleEdge)].Previous;
                                    if (!IsFacing(Previous, FurthestPoint))
                                        break;
                                    FirstVisibleEdge = Previous;
                                } while (FirstVisibleEdge != FurthestEdge);

                                KernelTypes::Int32 LastVisibleEdge = FurthestEdge;
                                do
                                {
                                    const KernelTypes::Int32 Next =
                                        EdgesState[static_cast<KernelTypes::Size>(LastVisibleEdge)].Next;
                                    if (!IsFacing(Next, FurthestPoint))
                                        break;
                                    LastVisibleEdge = Next;
                                } while (LastVisibleEdge != FurthestEdge);

                                const KernelTypes::Int32 NewFirstEdge =
                                    static_cast<KernelTypes::Int32>(EdgesState.GetCount());
                                AddEdge(
                                    EdgesState[static_cast<KernelTypes::Size>(FirstVisibleEdge)].StartIndex,
                                    -1,
                                    -1
                                );
                                const KernelTypes::Int32 NewSecondEdge =
                                    static_cast<KernelTypes::Int32>(EdgesState.GetCount());
                                AddEdge(FurthestPointIndex, -1, -1);

                                const KernelTypes::Int32 BeforeVisible =
                                    EdgesState[static_cast<KernelTypes::Size>(FirstVisibleEdge)].Previous;
                                const KernelTypes::Int32 AfterVisible =
                                    EdgesState[static_cast<KernelTypes::Size>(LastVisibleEdge)].Next;
                                EdgesState[static_cast<KernelTypes::Size>(NewFirstEdge)].Next = NewSecondEdge;
                                EdgesState[static_cast<KernelTypes::Size>(NewFirstEdge)].Previous = BeforeVisible;
                                EdgesState[static_cast<KernelTypes::Size>(NewSecondEdge)].Previous = NewFirstEdge;
                                EdgesState[static_cast<KernelTypes::Size>(NewSecondEdge)].Next = AfterVisible;
                                EdgesState[static_cast<KernelTypes::Size>(BeforeVisible)].Next = NewFirstEdge;
                                EdgesState[static_cast<KernelTypes::Size>(AfterVisible)].Previous = NewSecondEdge;
                                FirstEdge = NewFirstEdge;
                                EdgeCount += 2;
                                CalculateNormalAndCenter(NewFirstEdge);
                                CalculateNormalAndCenter(NewSecondEdge);

                                KernelTypes::Int32 RemovedEdge = FirstVisibleEdge;
                                for (;;)
                                {
                                    const KernelTypes::Int32 Next =
                                        EdgesState[static_cast<KernelTypes::Size>(RemovedEdge)].Next;
                                    const KernelTypes::Size ConflictCount =
                                        EdgesState[static_cast<KernelTypes::Size>(RemovedEdge)].Conflict.GetCount();
                                    for (KernelTypes::Size ConflictIndex = 0; ConflictIndex < ConflictCount; ++ConflictIndex)
                                    {
                                        AssignPointToEdges(
                                            EdgesState[static_cast<KernelTypes::Size>(RemovedEdge)].Conflict[ConflictIndex],
                                            NewFirstEdge,
                                            NewSecondEdge,
                                            -1
                                        );
                                    }
                                    --EdgeCount;
                                    if (RemovedEdge == LastVisibleEdge)
                                        break;
                                    RemovedEdge = Next;
                                }
                            }

                            OutputEdges.Reserve(static_cast<KernelTypes::Size>(EdgeCount));
                            KernelTypes::Int32 EdgeIndex = FirstEdge;
                            do
                            {
                                OutputEdges.Add(EdgesState[static_cast<KernelTypes::Size>(EdgeIndex)].StartIndex);
                                EdgeIndex = EdgesState[static_cast<KernelTypes::Size>(EdgeIndex)].Next;
                            } while (EdgeIndex != FirstEdge);
                            return BuildResult;
                        }

                    private:
                        struct Edge
                        {
                            KernelTypes::Int32 StartIndex = -1;
                            KernelTypes::Int32 Previous = -1;
                            KernelTypes::Int32 Next = -1;
                            KernelTypes::Float NormalX = 0.0f;
                            KernelTypes::Float NormalY = 0.0f;
                            KernelTypes::Float CenterX = 0.0f;
                            KernelTypes::Float CenterY = 0.0f;
                            KernelTypes::Float FurthestDistanceSquared = 0.0f;
                            KernelContainers::Array<KernelTypes::Int32> Conflict;
                        };

                        [[nodiscard]] static KernelTypes::Float CrossZ(const Vector3& A, const Vector3& B) noexcept
                        {
                            return A.GetX() * B.GetY() - A.GetY() * B.GetX();
                        }

                        void AddEdge(KernelTypes::Int32 StartIndex, KernelTypes::Int32 Previous, KernelTypes::Int32 Next)
                        {
                            Edge NewEdge;
                            NewEdge.StartIndex = StartIndex;
                            NewEdge.Previous = Previous;
                            NewEdge.Next = Next;
                            EdgesState.Add(KernelTypes::Move(NewEdge));
                        }

                        void CalculateNormalAndCenter(KernelTypes::Int32 EdgeIndex) noexcept
                        {
                            Edge& Current = EdgesState[static_cast<KernelTypes::Size>(EdgeIndex)];
                            const Vector3& Start = PositionsReference[static_cast<KernelTypes::Size>(Current.StartIndex)];
                            const Edge& NextEdge = EdgesState[static_cast<KernelTypes::Size>(Current.Next)];
                            const Vector3& End = PositionsReference[static_cast<KernelTypes::Size>(NextEdge.StartIndex)];
                            Current.CenterX = 0.5f * (Start.GetX() + End.GetX());
                            Current.CenterY = 0.5f * (Start.GetY() + End.GetY());
                            Current.NormalX = End.GetY() - Start.GetY();
                            Current.NormalY = Start.GetX() - End.GetX();
                            Current.FurthestDistanceSquared = 0.0f;
                        }

                        [[nodiscard]] KernelTypes::Bool IsFacing(KernelTypes::Int32 EdgeIndex, const Vector3& Point) const noexcept
                        {
                            const Edge& Current = EdgesState[static_cast<KernelTypes::Size>(EdgeIndex)];
                            return Current.NormalX * (Point.GetX() - Current.CenterX)
                                + Current.NormalY * (Point.GetY() - Current.CenterY) > 0.0f;
                        }

                        void AssignPointToEdges(
                            KernelTypes::Int32 PositionIndex,
                            KernelTypes::Int32 Edge1,
                            KernelTypes::Int32 Edge2,
                            KernelTypes::Int32 Edge3
                        )
                        {
                            const Vector3& Point = PositionsReference[static_cast<KernelTypes::Size>(PositionIndex)];
                            KernelTypes::Int32 BestEdge = -1;
                            KernelTypes::Float BestDistanceSquared = 0.0f;
                            const auto ConsiderCandidate = [&](KernelTypes::Int32 Candidate)
                            {
                                if (Candidate < 0)
                                    return;
                                const Edge& Current = EdgesState[static_cast<KernelTypes::Size>(Candidate)];
                                const KernelTypes::Float Dot =
                                    Current.NormalX * (Point.GetX() - Current.CenterX)
                                    + Current.NormalY * (Point.GetY() - Current.CenterY);
                                const KernelTypes::Float NormalLengthSquared =
                                    Current.NormalX * Current.NormalX + Current.NormalY * Current.NormalY;
                                if (Dot > 0.0f && NormalLengthSquared > 0.0f)
                                {
                                    const KernelTypes::Float DistanceSquared = Dot * Dot / NormalLengthSquared;
                                    if (DistanceSquared > BestDistanceSquared)
                                    {
                                        BestEdge = Candidate;
                                        BestDistanceSquared = DistanceSquared;
                                    }
                                }
                            };
                            ConsiderCandidate(Edge1);
                            ConsiderCandidate(Edge2);
                            ConsiderCandidate(Edge3);

                            if (BestEdge < 0)
                                return;

                            Edge& Target = EdgesState[static_cast<KernelTypes::Size>(BestEdge)];
                            if (BestDistanceSquared > Target.FurthestDistanceSquared)
                            {
                                Target.FurthestDistanceSquared = BestDistanceSquared;
                                Target.Conflict.Add(PositionIndex);
                            }
                            else
                            {
                                const KernelTypes::Size InsertIndex =
                                    Target.Conflict.GetCount() == 0 ? 0 : Target.Conflict.GetCount() - 1;
                                Target.Conflict.Insert(InsertIndex, PositionIndex);
                            }
                        }

                        const Positions& PositionsReference;
                        KernelContainers::Array<Edge> EdgesState;
                        KernelTypes::Int32 FirstEdge = -1;
                        KernelTypes::Int32 EdgeCount = 0;
                    };
                }
            }
        }
    }
}
