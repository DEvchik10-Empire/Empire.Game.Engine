#pragma once

#include "Empire.Engine/Engine.Kernel/Kernel.Types/Types.hpp"
#include "Empire.Engine/Engine.Math/Math.Vector/Vector.Vector3.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireEngine
        {
            namespace EngineMath
            {
                namespace MathGeometry
                {
                    namespace Geometry3D
                    {
                        using namespace EngineKernel;
                        using MathVector::Vector3;

                        struct AABB
                        {
                            Vector3 Min;
                            Vector3 Max;

                            AABB() noexcept;
                            AABB(const Vector3& Min, const Vector3& Max) noexcept;

                            static AABB FromCenterExtents(const Vector3& Center, const Vector3& Extents) noexcept;

                            Vector3 Center() const noexcept;
                            Vector3 Extents() const noexcept;
                            Vector3 Size() const noexcept;

                            KernelTypes::Bool Contains(const Vector3& Point) const noexcept;
                            KernelTypes::Bool Intersects(const AABB& Other) const noexcept;

                            AABB Union(const AABB& Other) const noexcept;

                            KernelTypes::Bool operator==(const AABB& Other) const noexcept;
                            KernelTypes::Bool operator!=(const AABB& Other) const noexcept;
                        };
                    }
                }
            }
        }
    }
}
