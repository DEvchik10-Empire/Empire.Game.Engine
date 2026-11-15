#pragma once

#include "../../Kernel/Types.hpp"

namespace Engine
{
    namespace Math
    {
        using namespace Kernel;

        inline constexpr Types::f32 Pi = 3.14159265358979323846f;
        inline constexpr Types::f32 TwoPi = 6.28318530717958647692f;
        inline constexpr Types::f32 HalfPi = 1.57079632679489661923f;
        inline constexpr Types::f32 Epsilon = 0.000001f;

        Types::f32 DegreesToRadians(Types::f32 Degrees) noexcept;
        Types::f32 RadiansToDegrees(Types::f32 Radians) noexcept;
        Types::f32 Sin(Types::f32 Value) noexcept;
        Types::f32 Cos(Types::f32 Value) noexcept;
        Types::f32 Tan(Types::f32 Value) noexcept;
        Types::f32 Asin(Types::f32 Value) noexcept;
        Types::f32 Acos(Types::f32 Value) noexcept;
        Types::f32 Atan2(Types::f32 Y,Types::f32 X) noexcept;
        Types::f32 Sqrt(Types::f32 Value) noexcept;
        Types::f32 Abs(Types::f32 Value) noexcept;
        Types::f32 Min(Types::f32 A,Types::f32 B) noexcept;
        Types::f32 Max(Types::f32 A,Types::f32 B) noexcept;
        Types::f32 Clamp(Types::f32 Value,Types::f32 Minimum,Types::f32 Maximum) noexcept;
        Types::f32 Lerp(Types::f32 A,Types::f32 B,Types::f32 Alpha) noexcept;
        Types::f32 Sign(Types::f32 Value) noexcept;
        Types::b8 IsZero(Types::f32 Value) noexcept;
        Types::b8 NearlyEqual(Types::f32 A,Types::f32 B,Types::f32 Tolerance = Epsilon) noexcept;

    }
}
