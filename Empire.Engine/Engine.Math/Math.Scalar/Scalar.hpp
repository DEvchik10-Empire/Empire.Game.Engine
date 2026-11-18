#pragma once

#include "Empire.Engine/Engine.Kernel/Kernel.Types/Types.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireEngine
        {
            namespace EngineMath
            {
                namespace MathScalar
                {
                    using namespace EngineKernel;

                    inline constexpr KernelTypes::Float Pi = 3.14159265358979323846f;
                    inline constexpr KernelTypes::Float TwoPi = 6.28318530717958647692f;
                    inline constexpr KernelTypes::Float HalfPi = 1.57079632679489661923f;
                    inline constexpr KernelTypes::Float Epsilon = 0.000001f;

                    KernelTypes::Float DegreesToRadians(KernelTypes::Float Degrees) noexcept;
                    KernelTypes::Float RadiansToDegrees(KernelTypes::Float Radians) noexcept;
                    KernelTypes::Float Sin(KernelTypes::Float Value) noexcept;
                    KernelTypes::Float Cos(KernelTypes::Float Value) noexcept;
                    KernelTypes::Float Tan(KernelTypes::Float Value) noexcept;
                    KernelTypes::Float Asin(KernelTypes::Float Value) noexcept;
                    KernelTypes::Float Acos(KernelTypes::Float Value) noexcept;
                    KernelTypes::Float Atan2(KernelTypes::Float Y,KernelTypes::Float X) noexcept;
                    KernelTypes::Float Sqrt(KernelTypes::Float Value) noexcept;
                    KernelTypes::Float Abs(KernelTypes::Float Value) noexcept;
                    KernelTypes::Float Min(KernelTypes::Float A,KernelTypes::Float B) noexcept;
                    KernelTypes::Float Max(KernelTypes::Float A,KernelTypes::Float B) noexcept;
                    KernelTypes::Float Clamp(KernelTypes::Float Value,KernelTypes::Float Minimum,KernelTypes::Float Maximum) noexcept;
                    KernelTypes::Float Lerp(KernelTypes::Float A,KernelTypes::Float B,KernelTypes::Float Alpha) noexcept;
                    KernelTypes::Float Sign(KernelTypes::Float Value) noexcept;
                    KernelTypes::Bool IsZero(KernelTypes::Float Value) noexcept;
                    KernelTypes::Bool NearlyEqual(KernelTypes::Float A,KernelTypes::Float B,KernelTypes::Float Tolerance = Epsilon) noexcept;
                }
            }
        }
    }
}
