#include "Empire.Runtime/Runtime.Tests/Tests.Math/Math.Unit/Unit.Math.hpp"
#include "Empire.Runtime/Runtime.Tests/Tests.Math/Math.Unit/Unit.Check.hpp"

#include "Empire.Engine/Engine.Math/Math.Scalar/Scalar.hpp"
#include "Empire.Engine/Engine.Math/Math.Scalar/FindRoot.hpp"
#include "Empire.Engine/Engine.Math/Math.Scalar/HalfFloat.hpp"
#include "Empire.Engine/Engine.Math/Math.Vector/HalfFloatConversion.hpp"
#include "Empire.Engine/Engine.Math/Math.Vector/Trigonometry.hpp"

namespace MS = DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineMath::MathScalar;
namespace KC = DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineKernel::KernelCompiler;
namespace KT = DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineKernel::KernelTypes;
namespace Half = DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineMath::MathScalar::HalfFloatConversion;
using HalfFloatRounding = DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineMath::MathScalar::HalfFloatRoundingMode;

void TestScalar()
{
    SECTION("Jolt scalar utilities");
    CHECK(MS::Pi == 3.14159265358979323846f);
    CHECK(MS::LargeFloat == 1.0e15f);
    CHECK_NEAR(MS::DegreesToRadians(180.0f), MS::Pi);
    CHECK_NEAR(MS::RadiansToDegrees(MS::Pi), 180.0f);
    CHECK_NEAR(MS::CenterAngleAroundZero(3.0f * MS::Pi), MS::Pi);
    CHECK_NEAR(MS::CenterAngleAroundZero(-3.0f * MS::Pi), -MS::Pi);
    CHECK(MS::Clamp(4, 0, 3) == 3 && MS::Clamp(-1, 0, 3) == 0);
    CHECK(MS::Clamp(1, 3, 0) == 0);
    const KT::Float NegativeZero = KC::BitCast<KT::Float>(0x80000000u);
    CHECK(KC::BitCast<KT::UInt32>(MS::Min(NegativeZero, 0.0f)) == 0x80000000u);
    CHECK(KC::BitCast<KT::UInt32>(MS::Max(NegativeZero, 0.0f)) == 0x80000000u);
    CHECK(KC::BitCast<KT::UInt32>(MS::Abs(-0.0f)) == 0u);
    CHECK_NEAR(MS::Sqrt(9.0f), 3.0f);
    CHECK(MS::Sqrt(9.0) == 3.0);
    CHECK(MS::Square(4) == 16 && MS::Cubed(3) == 27);
    CHECK(MS::Sign(-4) == -1 && MS::Sign(0) == 1 && MS::Sign(4) == 1);
    CHECK(MS::IsPowerOf2(1u) && MS::IsPowerOf2(1024u));
    CHECK(!MS::IsPowerOf2(0u) && !MS::IsPowerOf2(15u));
    CHECK(MS::AlignUp(17u, 16u) == 32u && MS::IsAligned(32u, 16u));
    CHECK_NEAR(MS::MulRSqrtApproximate(12.0f, 9.0f), 4.0f);
    CHECK(MS::CountTrailingZeros(0u) == 32u && MS::CountLeadingZeros(0u) == 32u);
    for (KT::UInt32 Index = 0; Index < 32; ++Index)
    {
        const KT::UInt32 Value = 1u << Index;
        CHECK(MS::CountTrailingZeros(Value) == Index);
        CHECK(MS::CountLeadingZeros(Value) == 31u - Index);
        CHECK(MS::CountBits(Value) == 1u);
        CHECK(MS::GetNextPowerOf2(Value) == Value);
    }
    CHECK(MS::GetNextPowerOf2(0u) == 1u && MS::GetNextPowerOf2(17u) == 32u);
    CHECK(MS::GetNextPowerOf2(0x80000000u) == 0x80000000u);
    CHECK(MS::GetNextPowerOf2(0x80000001u) == 0u);
    CHECK(MS::CountBits(0xffffffffu) == 32u && MS::CountBits(0xaaaaaaaau) == 16u);
    CHECK(MS::DifferenceOfProducts(2.0f, 4.0f, 3.0f, 2.0f) == 2.0f);
    KT::Float Root1 = 0.0f;
    KT::Float Root2 = 0.0f;
    CHECK(MS::FindRoot(1.0f, -3.0f, 2.0f, Root1, Root2) == 2);
    CHECK(Root1 == 2.0f && Root2 == 1.0f);
    CHECK(MS::FindRoot(1.0f, 2.0f, 1.0f, Root1, Root2) == 2);
    CHECK(Root1 == -1.0f && Root2 == -1.0f);
    CHECK(MS::FindRoot(0.0f, 2.0f, -6.0f, Root1, Root2) == 1);
    CHECK(Root1 == 3.0f && Root2 == 3.0f);
    CHECK(MS::FindRoot(0.0f, 0.0f, 0.0f, Root1, Root2) == 1);
    CHECK(Root1 == 0.0f && Root2 == 0.0f);
    CHECK(MS::FindRoot(0.0f, 0.0f, 1.0f, Root1, Root2) == 0);
    CHECK(MS::FindRoot(1.0f, 0.0f, 1.0f, Root1, Root2) == 0);

    SECTION("Jolt deterministic trigonometry");
    for (KT::Int32 Index = -80; Index <= 80; ++Index)
    {
        const KT::Float Angle = static_cast<KT::Float>(Index) * 0.0625f;
        const KT::Float Sine = MS::Sin(Angle);
        const KT::Float Cosine = MS::Cos(Angle);
        CHECK_NEAR_TOL(Sine * Sine + Cosine * Cosine, 1.0f, 2.0e-6f);
        CHECK_NEAR_TOL(MS::Tan(Angle) * Cosine, Sine, 1.0e-5f);
    }
    CHECK_NEAR_TOL(MS::Sin(0.125f), 0.12467473f, 2.0e-7f);
    CHECK_NEAR_TOL(MS::Cos(0.125f), 0.99219767f, 2.0e-7f);
    CHECK_NEAR_TOL(MS::Tan(0.125f), 0.12565514f, 2.0e-7f);
    CHECK_NEAR_TOL(MS::ASin(0.5f), MS::Pi / 6.0f, 2.0e-6f);
    CHECK_NEAR_TOL(MS::ACos(0.5f), MS::Pi / 3.0f, 2.0e-6f);
    CHECK_NEAR_TOL(MS::ATan(1.0f), MS::Pi / 4.0f, 2.0e-6f);
    CHECK_NEAR_TOL(MS::ACosApproximate(0.0f), MS::HalfPi, 1.0e-6f);
    CHECK_NEAR_TOL(MS::ACosApproximate(1.0f), 0.0f, 1.0e-6f);
    CHECK_NEAR_TOL(MS::ACosApproximate(-1.0f), MS::Pi, 1.0e-6f);
    CHECK(MS::ASin(-2.0f) == -MS::HalfPi && MS::ASin(2.0f) == MS::HalfPi);
    CHECK_NEAR(MS::ATan2(0.0f, 1.0f), 0.0f);
    CHECK_NEAR(MS::ATan2(1.0f, 1.0f), MS::Pi / 4.0f);
    CHECK_NEAR(MS::ATan2(1.0f, 0.0f), MS::HalfPi);
    CHECK_NEAR(MS::ATan2(1.0f, -1.0f), 3.0f * MS::Pi / 4.0f);
    CHECK_NEAR(MS::ATan2(-1.0f, -1.0f), -3.0f * MS::Pi / 4.0f);
    CHECK_NEAR(MS::ATan2(-1.0f, 0.0f), -MS::HalfPi);
    CHECK_NEAR(MS::ATan2(0.0f, -1.0f), MS::Pi);

    SECTION("Jolt half-float conversion");
    CHECK(Half::FromFloat<HalfFloatRounding::ToNearest>(0.0f) == 0x0000);
    CHECK(Half::FromFloat<HalfFloatRounding::ToNearest>(-0.0f) == 0x8000);
    CHECK(Half::FromFloat<HalfFloatRounding::ToNearest>(1.0f) == 0x3c00);
    CHECK(Half::FromFloat<HalfFloatRounding::ToNearest>(-2.0f) == 0xc000);
    CHECK(Half::FromFloat<HalfFloatRounding::ToNearest>(65504.0f) == 0x7bff);
    CHECK(Half::FromFloat<HalfFloatRounding::ToNearest>(65520.0f) == 0x7c00);
    CHECK(Half::FromFloat<HalfFloatRounding::ToNearest>(1.0f / 16777216.0f) == 0x0001);
    CHECK(Half::FromFloat<HalfFloatRounding::ToNearest>(1.0f / 33554432.0f) == 0x0000);
    CHECK(Half::FromFloat<HalfFloatRounding::TowardPositiveInfinity>(1.0f / 16777216.0f) == 0x0001);
    CHECK(Half::FromFloat<HalfFloatRounding::TowardNegativeInfinity>(-1.0f / 16777216.0f) == 0x8001);
    CHECK(Half::FromFloat<HalfFloatRounding::ToNearest>(1.0f / 1024.0f) == 0x1400);
    CHECK(Half::FromFloat<HalfFloatRounding::ToNearest>(1.0e-8f) == 0x0000);
    CHECK(Half::FromFloat<HalfFloatRounding::TowardPositiveInfinity>(1.0e-8f) == 0x0001);
    const KT::Float PositiveInfinity = KC::BitCast<KT::Float>(0x7f800000u);
    const KT::Float NegativeInfinity = KC::BitCast<KT::Float>(0xff800000u);
    const KT::Float QuietNaN = KC::BitCast<KT::Float>(0x7fc00000u);
    CHECK(Half::FromFloat<HalfFloatRounding::ToNearest>(PositiveInfinity) == 0x7c00);
    CHECK(Half::FromFloat<HalfFloatRounding::ToNearest>(NegativeInfinity) == 0xfc00);
    CHECK(Half::FromFloat<HalfFloatRounding::ToNearest>(QuietNaN) == 0x7e00);
    CHECK_NEAR(Half::ToFloat(0x0001), 1.0f / 16777216.0f);
    CHECK_NEAR(Half::ToFloat(0x03ff), 1023.0f / 16777216.0f);
    CHECK(Half::ToFloat(0x7c00) == PositiveInfinity);
    CHECK(Half::ToFloat(0xfc00) == NegativeInfinity);
    CHECK(Half::ToFloat(0x8000) == 0.0f && KC::BitCast<KT::UInt32>(Half::ToFloat(0x8000)) == 0x80000000u);
    const DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineMath::MathVector::UVector4 Packed(
        0x3c000000u, 0x00007c00u, 0, 0);
    const auto Unpacked = Half::ToFloat(Packed);
    CHECK(Unpacked == DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineMath::MathVector::Vector4(
        0.0f, 1.0f, PositiveInfinity, 0.0f));
    for (KT::UInt32 HalfBits = 0; HalfBits <= 0xffff; ++HalfBits)
    {
        const auto Encoded = Half::FromFloat<HalfFloatRounding::ToNearest>(Half::ToFloat(static_cast<MS::HalfFloat>(HalfBits)));
        const auto Expected = (HalfBits & 0x7c00u) == 0x7c00u && (HalfBits & 0x03ffu) != 0
            ? static_cast<MS::HalfFloat>((HalfBits & 0x8000u) | 0x7e00u)
            : static_cast<MS::HalfFloat>(HalfBits);
        CHECK(Encoded == Expected);
    }
    using UVector4 = DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineMath::MathVector::UVector4;
    using Vector4 = DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineMath::MathVector::Vector4;
    for (KT::UInt32 HalfBits = 0; HalfBits <= 0xffffu; HalfBits += 4)
    {
        const UVector4 Packed(
            HalfBits | ((HalfBits + 1u) << 16),
            (HalfBits + 2u) | ((HalfBits + 3u) << 16),
            0,
            0
        );
        const Vector4 Decoded = Half::ToFloat(Packed);
        CHECK(
            KC::BitCast<KT::UInt32>(Decoded.GetX()) == KC::BitCast<KT::UInt32>(Half::ToFloat(static_cast<MS::HalfFloat>(HalfBits)))
            && KC::BitCast<KT::UInt32>(Decoded.GetY()) == KC::BitCast<KT::UInt32>(Half::ToFloat(static_cast<MS::HalfFloat>(HalfBits + 1u)))
            && KC::BitCast<KT::UInt32>(Decoded.GetZ()) == KC::BitCast<KT::UInt32>(Half::ToFloat(static_cast<MS::HalfFloat>(HalfBits + 2u)))
            && KC::BitCast<KT::UInt32>(Decoded.GetW()) == KC::BitCast<KT::UInt32>(Half::ToFloat(static_cast<MS::HalfFloat>(HalfBits + 3u)))
        );
    }
}
