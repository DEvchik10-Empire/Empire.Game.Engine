#include "Math.hpp"

#include <cmath>

namespace Math
{
    f32 DegreesToRadians(
        f32 Degrees
    ) noexcept
    {
        return Degrees * (Pi / 180.0f);
    }

    f32 RadiansToDegrees(
        f32 Radians
    ) noexcept
    {
        return Radians * (180.0f / Pi);
    }

    f32 Sin(
        f32 Value
    ) noexcept
    {
        return std::sin(Value);
    }

    f32 Cos(
        f32 Value
    ) noexcept
    {
        return std::cos(Value);
    }

    f32 Tan(
        f32 Value
    ) noexcept
    {
        return std::tan(Value);
    }

    f32 Asin(
        f32 Value
    ) noexcept
    {
        return std::asin(Value);
    }

    f32 Acos(
        f32 Value
    ) noexcept
    {
        return std::acos(Value);
    }

    f32 Atan2(
        f32 Y,
        f32 X
    ) noexcept
    {
        return std::atan2(Y, X);
    }

    f32 Sqrt(
        f32 Value
    ) noexcept
    {
        return std::sqrt(Value);
    }

    f32 Abs(
        f32 Value
    ) noexcept
    {
        return std::abs(Value);
    }

    f32 Min(
        f32 A,
        f32 B
    ) noexcept
    {
        return A < B ? A : B;
    }

    f32 Max(
        f32 A,
        f32 B
    ) noexcept
    {
        return A > B ? A : B;
    }

    f32 Clamp(
        f32 Value,
        f32 Minimum,
        f32 Maximum
    ) noexcept
    {
        if (Value < Minimum)
            return Minimum;

        if (Value > Maximum)
            return Maximum;

        return Value;
    }

    f32 Lerp(
        f32 A,
        f32 B,
        f32 Alpha
    ) noexcept
    {
        return A + (B - A) * Alpha;
    }

    f32 Sign(
        f32 Value
    ) noexcept
    {
        if (Value > 0.0f)
            return 1.0f;

        if (Value < 0.0f)
            return -1.0f;

        return 0.0f;
    }

    b8 IsZero(
        f32 Value
    ) noexcept
    {
        return Abs(Value) <= Epsilon;
    }

    b8 NearlyEqual(
        f32 A,
        f32 B,
        f32 Tolerance
    ) noexcept
    {
        return Abs(A - B) <= Tolerance;
    }
}