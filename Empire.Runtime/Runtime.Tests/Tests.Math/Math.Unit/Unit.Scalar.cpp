#include "Empire.Runtime/Runtime.Tests/Tests.Math/Math.Unit/Unit.Math.hpp"
#include "Empire.Runtime/Runtime.Tests/Tests.Math/Math.Unit/Unit.Check.hpp"

#include "Empire.Engine/Engine.Math/Math.Scalar/Scalar.hpp"

namespace MS = DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineMath::MathScalar;

void TestScalar()
{
    SECTION("Scalar: константы");
    CHECK_NEAR_TOL(MS::Pi, 3.14159265f, 1e-6f);
    CHECK_NEAR_TOL(MS::TwoPi, 2.0f * MS::Pi, 1e-6f);
    CHECK_NEAR_TOL(MS::HalfPi, MS::Pi / 2.0f, 1e-6f);
    CHECK(MS::Epsilon == 0.000001f);

    SECTION("Scalar: градусы и радианы");
    CHECK_NEAR(MS::DegreesToRadians(0.0f), 0.0f);
    CHECK_NEAR(MS::DegreesToRadians(180.0f), MS::Pi);
    CHECK_NEAR(MS::DegreesToRadians(90.0f), MS::HalfPi);
    CHECK_NEAR(MS::DegreesToRadians(-90.0f), -MS::HalfPi);
    CHECK_NEAR(MS::DegreesToRadians(360.0f), MS::TwoPi);
    CHECK_NEAR(MS::RadiansToDegrees(MS::Pi), 180.0f);
    CHECK_NEAR(MS::RadiansToDegrees(MS::HalfPi), 90.0f);
    CHECK_NEAR(MS::RadiansToDegrees(0.0f), 0.0f);
    for (float Degrees = -720.0f; Degrees <= 720.0f; Degrees += 37.5f)
    {
        CHECK_NEAR_TOL(MS::RadiansToDegrees(MS::DegreesToRadians(Degrees)), Degrees, 1e-3f);
    }

    SECTION("Scalar: тригонометрия");
    CHECK_NEAR(MS::Sin(0.0f), 0.0f);
    CHECK_NEAR(MS::Sin(MS::HalfPi), 1.0f);
    CHECK_NEAR(MS::Sin(MS::Pi), 0.0f);
    CHECK_NEAR(MS::Sin(-MS::HalfPi), -1.0f);
    CHECK_NEAR(MS::Cos(0.0f), 1.0f);
    CHECK_NEAR(MS::Cos(MS::HalfPi), 0.0f);
    CHECK_NEAR(MS::Cos(MS::Pi), -1.0f);
    CHECK_NEAR(MS::Tan(0.0f), 0.0f);
    CHECK_NEAR(MS::Tan(MS::Pi / 4.0f), 1.0f);
    CHECK_NEAR(MS::Tan(-MS::Pi / 4.0f), -1.0f);
    for (float Angle = -6.0f; Angle <= 6.0f; Angle += 0.25f)
    {
        const float S = MS::Sin(Angle);
        const float C = MS::Cos(Angle);
        CHECK_NEAR(S * S + C * C, 1.0f);
        CHECK_NEAR(MS::Sin(-Angle), -S);
        CHECK_NEAR(MS::Cos(-Angle), C);
    }
    CHECK_NEAR(MS::Asin(0.0f), 0.0f);
    CHECK_NEAR(MS::Asin(1.0f), MS::HalfPi);
    CHECK_NEAR(MS::Asin(-1.0f), -MS::HalfPi);
    CHECK_NEAR(MS::Acos(1.0f), 0.0f);
    CHECK_NEAR(MS::Acos(0.0f), MS::HalfPi);
    CHECK_NEAR(MS::Acos(-1.0f), MS::Pi);
    for (float Value = -1.0f; Value <= 1.0f; Value += 0.125f)
    {
        CHECK_NEAR(MS::Sin(MS::Asin(Value)), Value);
        CHECK_NEAR(MS::Cos(MS::Acos(Value)), Value);
    }
    CHECK_NEAR(MS::Atan2(0.0f, 1.0f), 0.0f);
    CHECK_NEAR(MS::Atan2(1.0f, 1.0f), MS::Pi / 4.0f);
    CHECK_NEAR(MS::Atan2(1.0f, 0.0f), MS::HalfPi);
    CHECK_NEAR(MS::Atan2(1.0f, -1.0f), 3.0f * MS::Pi / 4.0f);
    CHECK_NEAR(MS::Atan2(-1.0f, -1.0f), -3.0f * MS::Pi / 4.0f);
    CHECK_NEAR(MS::Atan2(-1.0f, 0.0f), -MS::HalfPi);
    CHECK_NEAR(MS::Atan2(0.0f, -1.0f), MS::Pi);

    SECTION("Scalar: Sqrt, Abs");
    CHECK_NEAR(MS::Sqrt(0.0f), 0.0f);
    CHECK_NEAR(MS::Sqrt(1.0f), 1.0f);
    CHECK_NEAR(MS::Sqrt(9.0f), 3.0f);
    CHECK_NEAR(MS::Sqrt(2.0f), 1.41421356f);
    CHECK_NEAR(MS::Abs(-3.5f), 3.5f);
    CHECK_NEAR(MS::Abs(3.5f), 3.5f);
    CHECK_NEAR(MS::Abs(0.0f), 0.0f);

    SECTION("Scalar: Min, Max, Clamp");
    CHECK(MS::Min(1.0f, 2.0f) == 1.0f && MS::Min(2.0f, 1.0f) == 1.0f && MS::Min(2.0f, 2.0f) == 2.0f);
    CHECK(MS::Min(-1.0f, -2.0f) == -2.0f);
    CHECK(MS::Max(1.0f, 2.0f) == 2.0f && MS::Max(2.0f, 1.0f) == 2.0f && MS::Max(2.0f, 2.0f) == 2.0f);
    CHECK(MS::Max(-1.0f, -2.0f) == -1.0f);
    CHECK(MS::Clamp(0.5f, 0.0f, 1.0f) == 0.5f);
    CHECK(MS::Clamp(-3.0f, 0.0f, 1.0f) == 0.0f);
    CHECK(MS::Clamp(3.0f, 0.0f, 1.0f) == 1.0f);
    CHECK(MS::Clamp(0.0f, 0.0f, 1.0f) == 0.0f && MS::Clamp(1.0f, 0.0f, 1.0f) == 1.0f);
    CHECK(MS::Clamp(-5.0f, -10.0f, -2.0f) == -5.0f);
    CHECK(MS::Clamp(5.0f, 2.0f, 2.0f) == 2.0f);

    SECTION("Scalar: Lerp, Sign");
    CHECK_NEAR(MS::Lerp(2.0f, 10.0f, 0.0f), 2.0f);
    CHECK_NEAR(MS::Lerp(2.0f, 10.0f, 1.0f), 10.0f);
    CHECK_NEAR(MS::Lerp(2.0f, 10.0f, 0.5f), 6.0f);
    CHECK_NEAR(MS::Lerp(2.0f, 10.0f, 2.0f), 18.0f);
    CHECK_NEAR(MS::Lerp(2.0f, 10.0f, -1.0f), -6.0f);
    CHECK_NEAR(MS::Lerp(5.0f, 5.0f, 0.37f), 5.0f);
    CHECK(MS::Sign(3.0f) == 1.0f && MS::Sign(0.0001f) == 1.0f);
    CHECK(MS::Sign(-3.0f) == -1.0f && MS::Sign(-0.0001f) == -1.0f);
    CHECK(MS::Sign(0.0f) == 0.0f && MS::Sign(-0.0f) == 0.0f);

    SECTION("Scalar: IsZero, NearlyEqual");
    CHECK(MS::IsZero(0.0f));
    CHECK(MS::IsZero(1e-7f) && MS::IsZero(-1e-7f));
    CHECK(MS::IsZero(MS::Epsilon));
    CHECK(!MS::IsZero(1e-5f) && !MS::IsZero(-1e-5f));
    CHECK(!MS::IsZero(1.0f));
    CHECK(MS::NearlyEqual(1.0f, 1.0f));
    CHECK(MS::NearlyEqual(1.0f, 1.0000005f));
    CHECK(!MS::NearlyEqual(1.0f, 1.001f));
    CHECK(MS::NearlyEqual(1.0f, 1.001f, 0.01f));
    CHECK(!MS::NearlyEqual(1.0f, 1.1f, 0.01f));
    CHECK(MS::NearlyEqual(-2.0f, -2.0000005f));
    CHECK(MS::NearlyEqual(1.0f, 1.001f, 0.01f) == MS::NearlyEqual(1.001f, 1.0f, 0.01f));
}
