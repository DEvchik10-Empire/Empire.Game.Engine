#pragma once

#include "Empire.Engine/Engine.Kernel/Kernel.Types/Types.hpp"
#include "Empire.Engine/Engine.Math/Math.Vector/Vector.Vector3.hpp"
#include "Empire.Engine/Engine.Math/Math.Quaternion/Quaternion.Quaternion.hpp"

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
                        using MathQuaternion::Quaternion;
                        using MathVector::Vector3;

                        struct OBB
                        {
                            Vector3 Center;
                            Vector3 Extents;
                            Quaternion Orientation;

                            OBB() noexcept;
                            OBB(const Vector3& Center, const Vector3& Extents, const Quaternion& Orientation) noexcept;

                            KernelTypes::Bool Contains(const Vector3& Point) const noexcept;

                            KernelTypes::Bool operator==(const OBB& Other) const noexcept;
                            KernelTypes::Bool operator!=(const OBB& Other) const noexcept;
                        };
                    }
                }
            }
        }
    }
}
