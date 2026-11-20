#pragma once

#include "Empire.Engine/Engine.Kernel/Kernel.Containers/Containers.Array.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Containers/Containers.Hash.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Containers/Containers.StaticArray.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Types/Types.hpp"
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

                    class IndexedTriangleNoMaterial
                    {
                    public:
                        IndexedTriangleNoMaterial() : mIdx(KernelContainers::StaticArray<KernelTypes::UInt32, 3>::Of(0u, 0u, 0u)) {}
                        IndexedTriangleNoMaterial(KernelTypes::UInt32 Index1, KernelTypes::UInt32 Index2, KernelTypes::UInt32 Index3)
                            : mIdx(KernelContainers::StaticArray<KernelTypes::UInt32, 3>::Of(Index1, Index2, Index3))
                        {
                        }

                        [[nodiscard]] KernelTypes::Bool operator==(const IndexedTriangleNoMaterial& Other) const noexcept
                        {
                            return mIdx[0] == Other.mIdx[0] && mIdx[1] == Other.mIdx[1] && mIdx[2] == Other.mIdx[2];
                        }
                        [[nodiscard]] KernelTypes::Bool IsEquivalent(const IndexedTriangleNoMaterial& Other) const noexcept
                        {
                            return (mIdx[0] == Other.mIdx[0] && mIdx[1] == Other.mIdx[1] && mIdx[2] == Other.mIdx[2])
                                || (mIdx[0] == Other.mIdx[1] && mIdx[1] == Other.mIdx[2] && mIdx[2] == Other.mIdx[0])
                                || (mIdx[0] == Other.mIdx[2] && mIdx[1] == Other.mIdx[0] && mIdx[2] == Other.mIdx[1]);
                        }
                        [[nodiscard]] KernelTypes::Bool IsOpposite(const IndexedTriangleNoMaterial& Other) const noexcept
                        {
                            return (mIdx[0] == Other.mIdx[0] && mIdx[1] == Other.mIdx[2] && mIdx[2] == Other.mIdx[1])
                                || (mIdx[0] == Other.mIdx[1] && mIdx[1] == Other.mIdx[0] && mIdx[2] == Other.mIdx[2])
                                || (mIdx[0] == Other.mIdx[2] && mIdx[1] == Other.mIdx[1] && mIdx[2] == Other.mIdx[0]);
                        }
                        [[nodiscard]] KernelTypes::Bool IsDegenerate(
                            const KernelContainers::Array<Float3>& Vertices) const noexcept
                        {
                            const Vector3 V0 = Vector3::sLoadFloat3Unsafe(Vertices[mIdx[0]]);
                            const Vector3 V1 = Vector3::sLoadFloat3Unsafe(Vertices[mIdx[1]]);
                            const Vector3 V2 = Vector3::sLoadFloat3Unsafe(Vertices[mIdx[2]]);
                            return (V1 - V0).Cross(V2 - V0).IsNearZero();
                        }
                        void Rotate() noexcept
                        {
                            const KernelTypes::UInt32 First = mIdx[0];
                            mIdx[0] = mIdx[1];
                            mIdx[1] = mIdx[2];
                            mIdx[2] = First;
                        }
                        [[nodiscard]] Vector3 GetCentroid(const KernelContainers::Array<Float3>& Vertices) const noexcept
                        {
                            return (Vector3::sLoadFloat3Unsafe(Vertices[mIdx[0]])
                                + Vector3::sLoadFloat3Unsafe(Vertices[mIdx[1]])
                                + Vector3::sLoadFloat3Unsafe(Vertices[mIdx[2]])) / 3.0f;
                        }
                        [[nodiscard]] KernelTypes::UInt64 GetHash() const noexcept
                        {
                            KernelTypes::UInt64 Result = KernelContainers::HashBytes(&mIdx[0], sizeof(KernelTypes::UInt32));
                            Result = KernelContainers::HashBytes(&mIdx[1], sizeof(KernelTypes::UInt32), Result);
                            return KernelContainers::HashBytes(&mIdx[2], sizeof(KernelTypes::UInt32), Result);
                        }

                        KernelContainers::StaticArray<KernelTypes::UInt32, 3> mIdx;
                    };

                    class IndexedTriangle : public IndexedTriangleNoMaterial
                    {
                    public:
                        using IndexedTriangleNoMaterial::IndexedTriangleNoMaterial;
                        IndexedTriangle(KernelTypes::UInt32 Index1, KernelTypes::UInt32 Index2,
                            KernelTypes::UInt32 Index3, KernelTypes::UInt32 MaterialIndex,
                            KernelTypes::UInt32 UserData = 0)
                            : IndexedTriangleNoMaterial(Index1, Index2, Index3)
                            , mMaterialIndex(MaterialIndex), mUserData(UserData)
                        {
                        }

                        [[nodiscard]] KernelTypes::Bool operator==(const IndexedTriangle& Other) const noexcept
                        {
                            return mMaterialIndex == Other.mMaterialIndex && mUserData == Other.mUserData
                                && IndexedTriangleNoMaterial::operator==(Other);
                        }
                        [[nodiscard]] IndexedTriangle GetLowestIndexFirst() const
                        {
                            if (mIdx[0] < mIdx[1])
                            {
                                if (mIdx[0] < mIdx[2])
                                    return IndexedTriangle(mIdx[0], mIdx[1], mIdx[2], mMaterialIndex, mUserData);
                                return IndexedTriangle(mIdx[2], mIdx[0], mIdx[1], mMaterialIndex, mUserData);
                            }
                            if (mIdx[1] < mIdx[2])
                                return IndexedTriangle(mIdx[1], mIdx[2], mIdx[0], mMaterialIndex, mUserData);
                            return IndexedTriangle(mIdx[2], mIdx[0], mIdx[1], mMaterialIndex, mUserData);
                        }
                        [[nodiscard]] KernelTypes::UInt64 GetHash() const noexcept
                        {
                            KernelTypes::UInt64 Result = IndexedTriangleNoMaterial::GetHash();
                            Result = KernelContainers::HashBytes(&mMaterialIndex, sizeof(mMaterialIndex), Result);
                            return KernelContainers::HashBytes(&mUserData, sizeof(mUserData), Result);
                        }

                        KernelTypes::UInt32 mMaterialIndex = 0;
                        KernelTypes::UInt32 mUserData = 0;
                    };

                    using IndexedTriangleNoMaterialList = KernelContainers::Array<IndexedTriangleNoMaterial>;
                    using IndexedTriangleList = KernelContainers::Array<IndexedTriangle>;
                }
            }
        }
    }
}
