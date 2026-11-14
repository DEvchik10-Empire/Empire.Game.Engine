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

                    struct Ray
                    {
                        Vector3 Origin;
                        Vector3 Direction;

                        Ray() noexcept;
                        Ray(const Vector3& Origin, const Vector3& Direction) noexcept;

                        Vector3 GetPoint(KernelTypes::Float Distance) const noexcept;

                        KernelTypes::Bool operator==(const Ray& Other) const noexcept;
                        KernelTypes::Bool operator!=(const Ray& Other) const noexcept;
                    };
                }
            }
        }
    }
}
