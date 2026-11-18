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

                        struct Sphere
                        {
                            Vector3 Center;
                            KernelTypes::Float Radius;

                            Sphere() noexcept;
                            Sphere(const Vector3& Center, KernelTypes::Float Radius) noexcept;

                            KernelTypes::Bool Contains(const Vector3& Point) const noexcept;
                            KernelTypes::Bool Intersects(const Sphere& Other) const noexcept;

                            KernelTypes::Bool operator==(const Sphere& Other) const noexcept;
                            KernelTypes::Bool operator!=(const Sphere& Other) const noexcept;
                        };
                    }
                }
            }
        }
    }
}
