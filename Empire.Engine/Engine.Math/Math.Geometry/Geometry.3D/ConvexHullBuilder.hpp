#pragma once

#include "Empire.Engine/Engine.Kernel/Kernel.Containers/Containers.Array.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Types/Types.hpp"
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

                    class ConvexHullBuilder
                    {
                    public:
                        class Face;

                        class Edge
                        {
                        public:
                            Edge(Face* FaceValue, KernelTypes::Int32 StartIndex) noexcept
                                : OwnerFace(FaceValue), StartIndex(StartIndex)
                            {
                            }

                            [[nodiscard]] Edge* GetPreviousEdge() noexcept;

                            Face* OwnerFace = nullptr;
                            Edge* NextEdge = nullptr;
                            Edge* NeighbourEdge = nullptr;
                            KernelTypes::Int32 StartIndex = -1;
                        };

                        class Face
                        {
                        public:
                            ~Face();
                            void Initialize(KernelTypes::Int32 Index0, KernelTypes::Int32 Index1,
                                KernelTypes::Int32 Index2, const Vector3* Positions);
                            void CalculateNormalAndCentroid(const Vector3* Positions);
                            [[nodiscard]] KernelTypes::Bool IsFacing(const Vector3& Position) const noexcept;

                            Vector3 Normal;
                            Vector3 Centroid;
                            KernelContainers::Array<KernelTypes::Int32> ConflictList;
                            Edge* FirstEdge = nullptr;
                            KernelTypes::Float FurthestPointDistanceSquared = 0.0f;
                            KernelTypes::Bool Removed = false;
                        };

                        using Positions = KernelContainers::Array<Vector3>;
                        using Faces = KernelContainers::Array<Face*>;

                        enum class Result
                        {
                            Success,
                            MaxVerticesReached,
                            TooFewPoints,
                            TooFewFaces,
                            Degenerate
                        };

                        explicit ConvexHullBuilder(const Positions& InputPositions) noexcept
                            : PositionsReference(InputPositions)
                        {
                        }
                        ConvexHullBuilder(const ConvexHullBuilder&) = delete;
                        ConvexHullBuilder& operator=(const ConvexHullBuilder&) = delete;
                        ~ConvexHullBuilder();

                        [[nodiscard]] Result Initialize(
                            KernelTypes::Int32 MaxVertices,
                            KernelTypes::Float Tolerance,
                            const char*& Error
                        );
                        [[nodiscard]] KernelTypes::Int32 GetNumVerticesUsed() const;
                        [[nodiscard]] KernelTypes::Bool ContainsFace(
                            const KernelContainers::Array<KernelTypes::Int32>& Indices) const;
                        void GetCenterOfMassAndVolume(Vector3& CenterOfMass, KernelTypes::Float& Volume) const;
                        void DetermineMaxError(
                            Face*& FaceWithMaxError,
                            KernelTypes::Float& MaxError,
                            KernelTypes::Int32& MaxErrorPositionIndex,
                            KernelTypes::Float& CoplanarDistance
                        ) const;
                        [[nodiscard]] const Faces& GetFaces() const noexcept { return HullFaces; }

                    private:
                        static constexpr KernelTypes::Float MinimumTriangleAreaSquared = 1.0e-12f;
                        static constexpr KernelTypes::Float CoplanarSlopFactor = 6.0f;

                        class FullEdge
                        {
                        public:
                            Edge* NeighbourEdge = nullptr;
                            KernelTypes::Int32 StartIndex = -1;
                            KernelTypes::Int32 EndIndex = -1;
                        };

                        class Coplanar
                        {
                        public:
                            KernelTypes::Int32 PositionIndex = -1;
                            KernelTypes::Float DistanceSquared = 0.0f;
                        };

                        using FullEdges = KernelContainers::Array<FullEdge>;
                        using CoplanarList = KernelContainers::Array<Coplanar>;

                        [[nodiscard]] KernelTypes::Float DetermineCoplanarDistance() const noexcept;
                        void GetFaceForPoint(const Vector3& Point, const Faces& FaceList, Face*& OutFace,
                            KernelTypes::Float& OutDistanceSquared) const;
                        [[nodiscard]] KernelTypes::Float GetDistanceToEdgeSquared(
                            const Vector3& Point, const Face* FaceValue) const;
                        KernelTypes::Bool AssignPointToFace(KernelTypes::Int32 PositionIndex,
                            const Faces& FaceList, KernelTypes::Float ToleranceSquared);
                        void AddPoint(Face* FacingFace, KernelTypes::Int32 Index,
                            KernelTypes::Float CoplanarToleranceSquared, Faces& OutNewFaces);
                        void GarbageCollectFaces();
                        [[nodiscard]] Face* CreateFace();
                        [[nodiscard]] Face* CreateTriangle(KernelTypes::Int32 Index1,
                            KernelTypes::Int32 Index2, KernelTypes::Int32 Index3);
                        void FreeFace(Face* FaceValue);
                        void ClearFaces() noexcept;
                        static void LinkFace(Edge* Edge1, Edge* Edge2) noexcept;
                        static void UnlinkFace(Face* FaceValue) noexcept;
                        void FindEdge(Face* FacingFace, const Vector3& Vertex, FullEdges& OutEdges) const;
                        void MergeFaces(Edge* EdgeValue);
                        void MergeDegenerateFace(Face* FaceValue, Faces& AffectedFaces);
                        void MergeCoplanarOrConcaveFaces(Face* FaceValue,
                            KernelTypes::Float CoplanarToleranceSquared, Faces& AffectedFaces);
                        static void MarkAffected(Face* FaceValue, Faces& AffectedFaces);
                        void RemoveInvalidEdges(Face* FaceValue, Faces& AffectedFaces);
                        [[nodiscard]] static KernelTypes::Bool RemoveTwoEdgeFace(
                            Face* FaceValue, Faces& AffectedFaces);
                        void ValidateFace(const Face* FaceValue) const;
                        void ValidateFaces() const;

                        const Positions& PositionsReference;
                        Faces HullFaces;
                        CoplanarList CoplanarPositions;
                    };
                }
            }
        }
    }
}
