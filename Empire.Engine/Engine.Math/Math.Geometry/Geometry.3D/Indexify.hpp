#pragma once

#include "Empire.Engine/Engine.Kernel/Kernel.Containers/Containers.Array.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Types/Types.hpp"
#include "Empire.Engine/Engine.Math/Math.Geometry/Geometry.3D/IndexedTriangle.hpp"
#include "Empire.Engine/Engine.Math/Math.Geometry/Geometry.3D/Triangle.hpp"
#include "Empire.Engine/Engine.Math/Math.Vector/Float3.hpp"

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

                    inline void Indexify(const TriangleList& Triangles,
                        KernelContainers::Array<Float3>& Vertices, IndexedTriangleList& IndexedTriangles,
                        KernelTypes::Float VertexWeldDistance = 1.0e-4f)
                    {
                        const KernelTypes::Size VertexCount = Triangles.GetCount() * 3;
                        KernelContainers::Array<KernelTypes::UInt32> WeldedVertices;
                        WeldedVertices.Reserve(VertexCount);
                        for (KernelTypes::Size Index = 0; Index < VertexCount; ++Index)
                            WeldedVertices.Add(static_cast<KernelTypes::UInt32>(Index));

                        const KernelTypes::Float WeldDistanceSquared = VertexWeldDistance * VertexWeldDistance;
                        for (KernelTypes::Size First = 0; First < VertexCount; ++First)
                        {
                            const Float3& FirstStorage = Triangles[First / 3].mVertices[First % 3];
                            const Vector3 FirstVertex = Vector3::sLoadFloat3Unsafe(FirstStorage);
                            for (KernelTypes::Size Second = First + 1; Second < VertexCount; ++Second)
                            {
                                const Vector3 SecondVertex =
                                    Vector3::sLoadFloat3Unsafe(Triangles[Second / 3].mVertices[Second % 3]);
                                if ((SecondVertex - FirstVertex).LengthSq() > WeldDistanceSquared)
                                    continue;

                                KernelTypes::UInt32 FirstRoot = WeldedVertices[First];
                                while (WeldedVertices[FirstRoot] < FirstRoot)
                                    FirstRoot = WeldedVertices[FirstRoot];
                                KernelTypes::UInt32 SecondRoot = WeldedVertices[Second];
                                while (WeldedVertices[SecondRoot] < SecondRoot)
                                    SecondRoot = WeldedVertices[SecondRoot];
                                const KernelTypes::UInt32 Lowest = FirstRoot < SecondRoot ? FirstRoot : SecondRoot;
                                const KernelTypes::UInt32 Highest = FirstRoot > SecondRoot ? FirstRoot : SecondRoot;
                                WeldedVertices[Highest] = Lowest;
                                WeldedVertices[First] = Lowest;
                                WeldedVertices[Second] = Lowest;
                                break;
                            }
                        }

                        KernelContainers::Array<KernelTypes::UInt32> RemappedVertices;
                        RemappedVertices.Reserve(VertexCount);
                        for (KernelTypes::Size Index = 0; Index < VertexCount; ++Index)
                        {
                            KernelTypes::UInt32 Root = WeldedVertices[Index];
                            while (WeldedVertices[Root] < Root)
                                Root = WeldedVertices[Root];
                            WeldedVertices[Index] = Root;
                            RemappedVertices.Add(0);
                        }

                        Vertices.Clear();
                        for (KernelTypes::Size Index = 0; Index < VertexCount; ++Index)
                        {
                            if (WeldedVertices[Index] == Index)
                            {
                                RemappedVertices[Index] = static_cast<KernelTypes::UInt32>(Vertices.GetCount());
                                Vertices.Add(Triangles[Index / 3].mVertices[Index % 3]);
                            }
                            else
                                RemappedVertices[Index] = RemappedVertices[WeldedVertices[Index]];
                        }

                        IndexedTriangles.Clear();
                        IndexedTriangles.Reserve(Triangles.GetCount());
                        for (KernelTypes::Size Index = 0; Index < Triangles.GetCount(); ++Index)
                        {
                            const KernelTypes::Size Vertex = Index * 3;
                            IndexedTriangles.EmplaceBack(RemappedVertices[Vertex], RemappedVertices[Vertex + 1],
                                RemappedVertices[Vertex + 2], Triangles[Index].mMaterialIndex,
                                Triangles[Index].mUserData);
                        }
                    }

                    inline void Deindexify(const KernelContainers::Array<Float3>& Vertices,
                        const IndexedTriangleList& IndexedTriangles, TriangleList& Triangles)
                    {
                        Triangles.Clear();
                        Triangles.Reserve(IndexedTriangles.GetCount());
                        for (const IndexedTriangle& Indexed : IndexedTriangles)
                        {
                            Triangles.EmplaceBack(Vertices[Indexed.mIdx[0]], Vertices[Indexed.mIdx[1]],
                                Vertices[Indexed.mIdx[2]], Indexed.mMaterialIndex, Indexed.mUserData);
                        }
                    }
                }
            }
        }
    }
}
