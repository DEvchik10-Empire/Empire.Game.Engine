#pragma once

#include "Aliases.hpp"

using namespace Engine;

namespace Math
{
    inline constexpr f32 Pi = 3.14159265358979323846f;
    inline constexpr f32 TwoPi = 6.28318530717958647692f;
    inline constexpr f32 HalfPi = 1.57079632679489661923f;

    inline constexpr f32 Epsilon = 0.000001f;

    f32 DegreesToRadians(
        f32 Degrees
    ) noexcept;

    f32 RadiansToDegrees(
        f32 Radians
    ) noexcept;

    f32 Sin(
        f32 Value
    ) noexcept;

    f32 Cos(
        f32 Value
    ) noexcept;

    f32 Tan(
        f32 Value
    ) noexcept;

    f32 Asin(
        f32 Value
    ) noexcept;

    f32 Acos(
        f32 Value
    ) noexcept;

    f32 Atan2(
        f32 Y,
        f32 X
    ) noexcept;

    f32 Sqrt(
        f32 Value
    ) noexcept;

    f32 Abs(
        f32 Value
    ) noexcept;

    f32 Min(
        f32 A,
        f32 B
    ) noexcept;

    f32 Max(
        f32 A,
        f32 B
    ) noexcept;

    f32 Clamp(
        f32 Value,
        f32 Minimum,
        f32 Maximum
    ) noexcept;

    f32 Lerp(
        f32 A,
        f32 B,
        f32 Alpha
    ) noexcept;

    f32 Sign(
        f32 Value
    ) noexcept;

    b8 IsZero(
        f32 Value
    ) noexcept;

    b8 NearlyEqual(
        f32 A,
        f32 B,
        f32 Tolerance = Epsilon
    ) noexcept;
}