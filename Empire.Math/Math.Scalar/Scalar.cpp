#include "Empire.Math/Math.Scalar/Scalar.hpp"

#include <cmath>

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireMath
        {
            namespace MathScalar
            {
                using namespace EmpireKernel;

                KernelTypes::Float DegreesToRadians(KernelTypes::Float Degrees) noexcept
                {
                    return Degrees * (Pi / 180.0f);
                }

                KernelTypes::Float RadiansToDegrees(KernelTypes::Float Radians) noexcept
                {
                    return Radians * (180.0f / Pi);
                }

                KernelTypes::Float Sin(KernelTypes::Float Value) noexcept
                {
                    return std::sin(Value);
                }

                KernelTypes::Float Cos(KernelTypes::Float Value) noexcept
                {
                    return std::cos(Value);
                }

                KernelTypes::Float Tan(KernelTypes::Float Value) noexcept
                {
                    return std::tan(Value);
                }

                KernelTypes::Float Asin(KernelTypes::Float Value) noexcept
                {
                    return std::asin(Value);
                }

                KernelTypes::Float Acos(KernelTypes::Float Value) noexcept
                {
                    return std::acos(Value);
                }

                KernelTypes::Float Atan2(KernelTypes::Float Y, KernelTypes::Float X) noexcept
                {
                    return std::atan2(Y, X);
                }

                KernelTypes::Float Sqrt(KernelTypes::Float Value) noexcept
                {
                    return std::sqrt(Value);
                }

                KernelTypes::Float Abs(KernelTypes::Float Value) noexcept
                {
                    return std::abs(Value);
                }

                KernelTypes::Float Min(KernelTypes::Float A, KernelTypes::Float B) noexcept
                {
                    return A < B ? A : B;
                }

                KernelTypes::Float Max(KernelTypes::Float A, KernelTypes::Float B) noexcept
                {
                    return A > B ? A : B;
                }

                KernelTypes::Float Clamp(KernelTypes::Float Value, KernelTypes::Float Minimum, KernelTypes::Float Maximum) noexcept
                {
                    if (Value < Minimum)
                        return Minimum;

                    if (Value > Maximum)
                        return Maximum;

                    return Value;
                }

                KernelTypes::Float Lerp(KernelTypes::Float A, KernelTypes::Float B, KernelTypes::Float Alpha) noexcept
                {
                    return A + (B - A) * Alpha;
                }

                KernelTypes::Float Sign(KernelTypes::Float Value) noexcept
                {
                    if (Value > 0.0f)
                        return 1.0f;

                    if (Value < 0.0f)
                        return -1.0f;

                    return 0.0f;
                }

                KernelTypes::Bool IsZero(KernelTypes::Float Value) noexcept
                {
                    return Abs(Value) <= Epsilon;
                }

                KernelTypes::Bool NearlyEqual(KernelTypes::Float A, KernelTypes::Float B, KernelTypes::Float Tolerance) noexcept
                {
                    return Abs(A - B) <= Tolerance;
                }
            }
        }
    }
}
