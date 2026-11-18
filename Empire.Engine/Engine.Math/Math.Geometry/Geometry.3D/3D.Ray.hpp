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
}
