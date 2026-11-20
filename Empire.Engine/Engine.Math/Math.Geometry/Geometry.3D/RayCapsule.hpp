#pragma once

#include "Empire.Engine/Engine.Kernel/Kernel.Compiler/Compiler.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Types/Types.hpp"
#include "Empire.Engine/Engine.Math/Math.Geometry/Geometry.3D/RayCylinder.hpp"
#include "Empire.Engine/Engine.Math/Math.Geometry/Geometry.3D/RaySphere.hpp"
#include "Empire.Engine/Engine.Math/Math.Scalar/Scalar.hpp"
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

                    [[nodiscard]] inline KernelTypes::Float RayCapsule(
                        const Vector3& Origin, const Vector3& Direction,
                        KernelTypes::Float HalfHeight, KernelTypes::Float Radius) noexcept
                    {
                        const KernelTypes::Float MaximumFloat = KernelCompiler::BitCast<KernelTypes::Float>(0x7f7fffffu);
                        const KernelTypes::Float CylinderFraction = RayCylinder(Origin, Direction, Radius);
                        if (CylinderFraction != MaximumFloat
                            && MathScalar::Abs(Origin.GetY() + CylinderFraction * Direction.GetY()) <= HalfHeight)
                            return CylinderFraction;

                        const Vector3 SphereCenter(0.0f, HalfHeight, 0.0f);
                        const KernelTypes::Float Upper = RaySphere(Origin, Direction, SphereCenter, Radius);
                        const KernelTypes::Float Lower = RaySphere(Origin, Direction, -SphereCenter, Radius);
                        return Upper < Lower ? Upper : Lower;
                    }
                }
            }
        }
    }
}
