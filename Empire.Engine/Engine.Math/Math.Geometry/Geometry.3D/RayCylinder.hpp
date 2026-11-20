#pragma once

#include "Empire.Engine/Engine.Kernel/Kernel.Compiler/Compiler.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Types/Types.hpp"
#include "Empire.Engine/Engine.Math/Math.Geometry/Geometry.3D/RaySphere.hpp"
#include "Empire.Engine/Engine.Math/Math.Scalar/FindRoot.hpp"
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

                    [[nodiscard]] inline KernelTypes::Float RayCylinder(
                        const Vector3& Origin, const Vector3& Direction, KernelTypes::Float Radius) noexcept
                    {
                        const KernelTypes::Float OriginLengthSquared =
                            MathScalar::Square(Origin.GetX()) + MathScalar::Square(Origin.GetZ());
                        const KernelTypes::Float RadiusSquared = MathScalar::Square(Radius);
                        if (OriginLengthSquared <= RadiusSquared)
                            return 0.0f;

                        const KernelTypes::Float A =
                            MathScalar::Square(Direction.GetX()) + MathScalar::Square(Direction.GetZ());
                        const KernelTypes::Float B = 2.0f
                            * (Origin.GetX() * Direction.GetX() + Origin.GetZ() * Direction.GetZ());
                        const KernelTypes::Float C = OriginLengthSquared - RadiusSquared;
                        KernelTypes::Float Fraction1 = 0.0f;
                        KernelTypes::Float Fraction2 = 0.0f;
                        if (MathScalar::FindRoot(A, B, C, Fraction1, Fraction2) == 0)
                            return KernelCompiler::BitCast<KernelTypes::Float>(0x7f7fffffu);

                        const KernelTypes::Float Fraction = Fraction1 < Fraction2 ? Fraction1 : Fraction2;
                        return Fraction >= 0.0f ? Fraction : KernelCompiler::BitCast<KernelTypes::Float>(0x7f7fffffu);
                    }

                    [[nodiscard]] inline KernelTypes::Float RayCylinder(
                        const Vector3& Origin, const Vector3& Direction,
                        KernelTypes::Float HalfHeight, KernelTypes::Float Radius) noexcept
                    {
                        const KernelTypes::Float Fraction = RayCylinder(Origin, Direction, Radius);
                        const KernelTypes::Float MaximumFloat = KernelCompiler::BitCast<KernelTypes::Float>(0x7f7fffffu);
                        if (Fraction == MaximumFloat)
                            return MaximumFloat;

                        if (MathScalar::Abs(Origin.GetY() + Fraction * Direction.GetY()) <= HalfHeight)
                            return Fraction;

                        const KernelTypes::Float DirectionY = Direction.GetY();
                        if (DirectionY != 0.0f)
                        {
                            const KernelTypes::Float PlaneFraction = DirectionY < 0.0f
                                ? (HalfHeight - Origin.GetY()) / DirectionY
                                : -(HalfHeight + Origin.GetY()) / DirectionY;
                            if (PlaneFraction >= 0.0f)
                            {
                                const Vector3 Point = Origin + PlaneFraction * Direction;
                                if (MathScalar::Square(Point.GetX()) + MathScalar::Square(Point.GetZ())
                                    <= MathScalar::Square(Radius))
                                    return PlaneFraction;
                            }
                        }
                        return MaximumFloat;
                    }
                }
            }
        }
    }
}
