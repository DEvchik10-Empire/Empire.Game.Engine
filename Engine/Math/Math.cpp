#include "Math.hpp"

#include <cmath>

namespace Engine
{
    namespace Math
    {
        using namespace Kernel;
        Types::f32 DegreesToRadians(Types::f32 Degrees) noexcept
        {
            return Degrees * (Pi / 180.0f);
        }

        Types::f32 RadiansToDegrees(Types::f32 Radians) noexcept
        {
            return Radians * (180.0f / Pi);
        }

        Types::f32 Sin(Types::f32 Value) noexcept
        {
            return std::sin(Value);
        }

        Types::f32 Cos(Types::f32 Value) noexcept
        {
            return std::cos(Value);
        }

        Types::f32 Tan(Types::f32 Value) noexcept
        {
            return std::tan(Value);
        }

        Types::f32 Asin(Types::f32 Value) noexcept
        {
            return std::asin(Value);
        }

        Types::f32 Acos(Types::f32 Value) noexcept
        {
            return std::acos(Value);
        }

        Types::f32 Atan2(Types::f32 Y, Types::f32 X) noexcept
        {
            return std::atan2(Y, X);
        }

        Types::f32 Sqrt(Types::f32 Value) noexcept
        {
            return std::sqrt(Value);
        }

        Types::f32 Abs(Types::f32 Value) noexcept
        {
            return std::abs(Value);
        }

        Types::f32 Min(Types::f32 A, Types::f32 B) noexcept
        {
            return A < B ? A : B;
        }

        Types::f32 Max(Types::f32 A, Types::f32 B) noexcept
        {
            return A > B ? A : B;
        }

        Types::f32 Clamp(Types::f32 Value, Types::f32 Minimum, Types::f32 Maximum) noexcept
        {
            if (Value < Minimum)
                return Minimum;

            if (Value > Maximum)
                return Maximum;

            return Value;
        }

        Types::f32 Lerp(Types::f32 A, Types::f32 B, Types::f32 Alpha) noexcept
        {
            return A + (B - A) * Alpha;
        }

        Types::f32 Sign(Types::f32 Value) noexcept
        {
            if (Value > 0.0f)
                return 1.0f;

            if (Value < 0.0f)
                return -1.0f;

            return 0.0f;
        }

        Types::b8 IsZero(Types::f32 Value) noexcept
        {
            return Abs(Value) <= Epsilon;
        }

        Types::b8 NearlyEqual(Types::f32 A, Types::f32 B, Types::f32 Tolerance) noexcept
        {
            return Abs(A - B) <= Tolerance;
        }
    }
}
