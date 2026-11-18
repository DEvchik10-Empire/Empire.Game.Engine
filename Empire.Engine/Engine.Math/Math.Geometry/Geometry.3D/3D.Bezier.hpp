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

                        struct Bezier
                        {
                            static Vector3 Cubic(const Vector3& P0, const Vector3& P1, const Vector3& P2, const Vector3& P3, KernelTypes::Float T) noexcept;
                        };
                    }
                }
            }
        }
    }
}
