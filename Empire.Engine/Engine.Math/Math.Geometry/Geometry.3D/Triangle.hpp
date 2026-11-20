#pragma once

#include "Empire.Engine/Engine.Kernel/Kernel.Containers/Containers.Array.hpp"
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

                    class Triangle
                    {
                    public:
                        Triangle() noexcept : mVertices(KernelContainers::StaticArray<Float3, 3>::Of(Float3{}, Float3{}, Float3{})) {}
                        Triangle(const Float3& Vertex1, const Float3& Vertex2, const Float3& Vertex3,
                            KernelTypes::UInt32 MaterialIndex = 0, KernelTypes::UInt32 UserData = 0)
                            : mVertices(KernelContainers::StaticArray<Float3, 3>::Of(Vertex1, Vertex2, Vertex3))
                            , mMaterialIndex(MaterialIndex), mUserData(UserData)
                        {
                        }
                        Triangle(const Vector3& Vertex1, const Vector3& Vertex2, const Vector3& Vertex3,
                            KernelTypes::UInt32 MaterialIndex = 0, KernelTypes::UInt32 UserData = 0)
                            : mVertices(KernelContainers::StaticArray<Float3, 3>::Of(Float3{}, Float3{}, Float3{}))
                            , mMaterialIndex(MaterialIndex), mUserData(UserData)
                        {
                            Vertex1.StoreFloat3(&mVertices[0]);
                            Vertex2.StoreFloat3(&mVertices[1]);
                            Vertex3.StoreFloat3(&mVertices[2]);
                        }

                        [[nodiscard]] Vector3 GetCentroid() const noexcept
                        {
                            return (Vector3::sLoadFloat3Unsafe(mVertices[0])
                                + Vector3::sLoadFloat3Unsafe(mVertices[1])
                                + Vector3::sLoadFloat3Unsafe(mVertices[2])) * (1.0f / 3.0f);
                        }

                        KernelContainers::StaticArray<Float3, 3> mVertices;
                        KernelTypes::UInt32 mMaterialIndex = 0;
                        KernelTypes::UInt32 mUserData = 0;
                    };

                    using TriangleList = KernelContainers::Array<Triangle>;
                }
            }
        }
    }
}
