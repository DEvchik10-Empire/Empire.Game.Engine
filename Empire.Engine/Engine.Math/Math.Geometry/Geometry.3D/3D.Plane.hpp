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

                        struct Plane
                        {
                            Vector3 Normal;
                            KernelTypes::Float Distance;

                            Plane() noexcept;
                            Plane(const Vector3& Normal, KernelTypes::Float Distance) noexcept;

                            static Plane FromPointNormal(const Vector3& Point, const Vector3& Normal) noexcept;

                            KernelTypes::Float DistanceToPoint(const Vector3& Point) const noexcept;

                            KernelTypes::Bool operator==(const Plane& Other) const noexcept;
                            KernelTypes::Bool operator!=(const Plane& Other) const noexcept;
                        };
                    }
                }
            }
        }
    }
}
