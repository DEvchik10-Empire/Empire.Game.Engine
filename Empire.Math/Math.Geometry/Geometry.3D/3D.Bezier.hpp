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

                    struct Bezier
                    {
                        static Vector3 Cubic(const Vector3& P0, const Vector3& P1, const Vector3& P2, const Vector3& P3, KernelTypes::Float T) noexcept;
                    };
                }
            }
        }
    }
}
