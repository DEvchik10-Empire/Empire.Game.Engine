#pragma once

#include "Empire.Engine/Engine.Kernel/Kernel.Assert/Assert.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Types/Types.hpp"
#include "Empire.Engine/Engine.Math/Math.Scalar/Scalar.hpp"
#include "Empire.Engine/Engine.Math/Math.Vector/Float2.hpp"

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

                    class Ellipse
                    {
                    public:
                        Ellipse(KernelTypes::Float RadiusX, KernelTypes::Float RadiusY) noexcept
                            : mRadiusX(RadiusX), mRadiusY(RadiusY)
                        {
                            KernelAssert::DebugCheck(RadiusX > 0.0f && RadiusY > 0.0f,
                                "Ellipse radii must be positive");
                        }

                        [[nodiscard]] KernelTypes::Bool IsInside(const Float2& Point) const noexcept
                        {
                            return MathScalar::Square(Point.X / mRadiusX)
                                + MathScalar::Square(Point.Y / mRadiusY) <= 1.0f;
                        }

                        [[nodiscard]] Float2 GetClosestPoint(const Float2& Point) const noexcept
                        {
                            const KernelTypes::Float RadiusXSquared = MathScalar::Square(mRadiusX);
                            const KernelTypes::Float RadiusYSquared = MathScalar::Square(mRadiusY);
                            KernelTypes::Float T = 0.0f;
                            for (;;)
                            {
                                const KernelTypes::Float TPlusRadiusXSquared = T + RadiusXSquared;
                                const KernelTypes::Float TPlusRadiusYSquared = T + RadiusYSquared;
                                const KernelTypes::Float Value = MathScalar::Square(mRadiusX * Point.X / TPlusRadiusXSquared)
                                    + MathScalar::Square(mRadiusY * Point.Y / TPlusRadiusYSquared) - 1.0f;
                                if (MathScalar::Abs(Value) < 1.0e-6f)
                                {
                                    return Float2(
                                        RadiusXSquared * Point.X / TPlusRadiusXSquared,
                                        RadiusYSquared * Point.Y / TPlusRadiusYSquared);
                                }

                                const KernelTypes::Float Derivative = -2.0f *
                                    (RadiusXSquared * MathScalar::Square(Point.X) / MathScalar::Cubed(TPlusRadiusXSquared)
                                    + RadiusYSquared * MathScalar::Square(Point.Y) / MathScalar::Cubed(TPlusRadiusYSquared));
                                T -= Value / Derivative;
                            }
                        }

                        [[nodiscard]] Float2 GetNormal(const Float2& Point) const noexcept
                        {
                            return Float2(Point.X / MathScalar::Square(mRadiusX),
                                Point.Y / MathScalar::Square(mRadiusY));
                        }

                    private:
                        KernelTypes::Float mRadiusX;
                        KernelTypes::Float mRadiusY;
                    };
                }
            }
        }
    }
}
