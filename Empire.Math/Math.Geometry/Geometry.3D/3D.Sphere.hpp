#pragma once

#include "Empire.Kernel/Kernel.Types/Types.hpp"
#include "Empire.Math/Math.Vector/Vector.Vector3.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireMath
        {
            namespace MathGeometry
            {
                namespace Geometry3D
                {
                    using namespace EmpireKernel;
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
