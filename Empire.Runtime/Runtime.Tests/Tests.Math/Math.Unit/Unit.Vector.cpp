#include "Empire.Runtime/Runtime.Tests/Tests.Math/Math.Unit/Unit.Math.hpp"
#include "Empire.Runtime/Runtime.Tests/Tests.Math/Math.Unit/Unit.Check.hpp"

#include "Empire.Engine/Engine.Math/Math.Vector/Vector3.hpp"
#include "Empire.Engine/Engine.Math/Math.Vector/Vector4.hpp"
#include "Empire.Engine/Engine.Math/Math.Vector/Float2.hpp"
#include "Empire.Engine/Engine.Math/Math.Vector/Float3.hpp"
#include "Empire.Engine/Engine.Math/Math.Vector/Float4.hpp"
#include "Empire.Engine/Engine.Math/Math.Vector/Double3.hpp"
#include "Empire.Engine/Engine.Math/Math.Vector/BVector16.hpp"
#include "Empire.Engine/Engine.Math/Math.Vector/UVector4.hpp"
#include "Empire.Engine/Engine.Math/Math.Vector/Swizzle.hpp"

namespace MV = DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineMath::MathVector;
namespace MathScalar = DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineMath::MathScalar;
namespace KT = DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineKernel::KernelTypes;
namespace KernelContainers = DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineKernel::KernelContainers;
namespace KernelCompiler = DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineKernel::KernelCompiler;
using MV::Vector3;
using MV::Vector4;
using MV::Float2;
using MV::Float3;
using MV::Float4;
using MV::Double3;
using MV::BVector16;
using MV::UVector4;

namespace
{
    constexpr Vector3 ConstexprA3(1.0f, 2.0f, 3.0f);
    constexpr Vector3 ConstexprB3 = ConstexprA3 * 2.0f;
    static_assert(ConstexprB3.GetX() == 2.0f && ConstexprB3.GetY() == 4.0f && ConstexprB3.GetZ() == 6.0f);
    constexpr UVector4 ConstexprAUInt4(0xffffffffu, 0u, 0x80000000u, 7u);
    constexpr UVector4 ConstexprBUInt4(1u, 1u, 0x80000000u, 9u);
    static_assert(ConstexprAUInt4 + ConstexprBUInt4 == UVector4(0u, 1u, 0u, 16u));
    static_assert(ConstexprAUInt4 - ConstexprBUInt4 == UVector4(0xfffffffeu, 0xffffffffu, 0u, 0xfffffffeu));
    constexpr Vector4 ConstexprA4(1.0f, 2.0f, 3.0f, 4.0f);
    constexpr Vector4 ConstexprB4 = -ConstexprA4;
    constexpr Vector4 ConstexprC4 = ConstexprA4 + ConstexprB4;
    static_assert(ConstexprB4.W == -4.0f);
    static_assert(ConstexprC4 == Vector4::Zero());
    static_assert(sizeof(Vector3) == 16 && alignof(Vector3) == 16);
    static_assert(sizeof(Vector4) == 16 && alignof(Vector4) == 16);

    Vector3 RandomVector3(MathTests::Random& Generator)
    {
        return Vector3(Generator.Range(-10.0f, 10.0f), Generator.Range(-10.0f, 10.0f), Generator.Range(-10.0f, 10.0f));
    }

    struct RandomRange
    {
        KT::UInt32 State;

        static constexpr KT::UInt32 min() noexcept { return 0; }
        static constexpr KT::UInt32 max() noexcept { return 0xffffffffu; }

        KT::UInt32 operator()() noexcept
        {
            State ^= State << 13;
            State ^= State >> 17;
            State ^= State << 5;
            return State;
        }
    };
}

void TestVectorStorage()
{
    SECTION("Jolt vector storage and integer types");
    const Float2 Stored2(1.0f, 2.0f);
    CHECK(sizeof(Float2) == 8 && Stored2[0] == 1.0f && Stored2[1] == 2.0f);
    const Float3 Stored3(1.0f, 2.0f, 3.0f);
    CHECK(sizeof(Float3) == 12 && Stored3[2] == 3.0f);
    MV::VertexList VertexValues;
    VertexValues.Add(Stored3);
    CHECK(VertexValues.GetCount() == 1 && VertexValues[0] == Stored3);
    CHECK(KernelContainers::Hash<Float3>{}(Stored3) == KernelContainers::Hash<Float3>{}(Float3(1.0f, 2.0f, 3.0f)));
    CHECK(KernelContainers::Hash<Float3>{}(Float3(-0.0f, 2.0f, 3.0f))
        == KernelContainers::Hash<Float3>{}(Float3(0.0f, 2.0f, 3.0f)));
    const Float4 Stored4(1.0f, 2.0f, 3.0f, 4.0f);
    CHECK(sizeof(Float4) == 16 && alignof(Float4) == 16 && Stored4[3] == 4.0f);
    const Double3 StoredDouble3(1.0, 2.0, 3.0);
    CHECK(sizeof(Double3) == 24 && StoredDouble3[2] == 3.0);
    CHECK(KernelContainers::Hash<Double3>{}(StoredDouble3)
        == KernelContainers::Hash<Double3>{}(Double3(1.0, 2.0, 3.0)));
    CHECK(sizeof(BVector16) == 16 && alignof(BVector16) == 16);
    const BVector16 Bytes(0x80, 0, 0xff, 0x7f, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 0x80);
    CHECK(Bytes[0] == 0x80 && Bytes[3] == 0x7f && Bytes[15] == 0x80);
    CHECK(Bytes.GetTrues() == 0x8005);
    CHECK(Bytes.TestAnyTrue() && !Bytes.TestAllTrue());
    CHECK(BVector16::sEquals(Bytes, Bytes).TestAllTrue());
    CHECK(BVector16::sZero().GetTrues() == 0);
    CHECK(BVector16::sReplicate(0xff).TestAllTrue());
    CHECK(BVector16::sAnd(Bytes, BVector16::sReplicate(0x80)).GetTrues() == 0x8005);
    CHECK(BVector16::sOr(BVector16::sZero(), Bytes) == Bytes);
    CHECK(BVector16::sXor(Bytes, Bytes) == BVector16::sZero());
    CHECK(BVector16::sNot(BVector16::sZero()).TestAllTrue());
    CHECK(sizeof(UVector4) == 16 && alignof(UVector4) == 16);
    const UVector4 Integers(1, 2, 3, 4);
    CHECK(Integers.GetX() == 1 && Integers.GetY() == 2 && Integers.GetZ() == 3 && Integers.GetW() == 4);
    CHECK(UVector4::sZero() == UVector4(0, 0, 0, 0));
    CHECK(UVector4::sReplicate(7) == UVector4(7, 7, 7, 7));
    CHECK((Integers.Swizzle<
        static_cast<KT::UInt32>(MV::SwizzleComponent::W),
        static_cast<KT::UInt32>(MV::SwizzleComponent::Z),
        static_cast<KT::UInt32>(MV::SwizzleComponent::Y),
        static_cast<KT::UInt32>(MV::SwizzleComponent::X)
    >() == UVector4(4, 3, 2, 1)));
    CHECK(Integers.SplatY() == UVector4(2, 2, 2, 2));
    CHECK(Integers + UVector4(4, 3, 2, 1) == UVector4(5, 5, 5, 5));
    CHECK(Integers - UVector4(1, 1, 1, 1) == UVector4(0, 1, 2, 3));
    CHECK(Integers * UVector4(2, 3, 4, 5) == UVector4(2, 6, 12, 20));
    CHECK(UVector4::sMin(Integers, UVector4(0, 3, 2, 5)) == UVector4(0, 2, 2, 4));
    CHECK(UVector4::sMax(Integers, UVector4(0, 3, 2, 5)) == UVector4(1, 3, 3, 5));
    CHECK(UVector4::sEquals(Integers, UVector4(1, 0, 3, 0)).GetTrues() == 5);
    CHECK(UVector4::sSelect(
        UVector4(0, 0, 0, 0),
        UVector4(0x80000001, 2, 0xffffffff, 4),
        UVector4(0x80000000, 0, 0xffffffff, 0)
    ).GetTrues() == 5);
    CHECK(UVector4::sOr(UVector4(1, 2, 4, 8), UVector4(8, 4, 2, 1)) == UVector4(9, 6, 6, 9));
    CHECK(UVector4::sXor(UVector4(15, 5, 9, 0), UVector4(3, 5, 12, 0)) == UVector4(12, 0, 5, 0));
    CHECK(UVector4::sAnd(UVector4(15, 5, 9, 0), UVector4(3, 5, 12, 0)) == UVector4(3, 5, 8, 0));
    CHECK(UVector4::sNot(UVector4(0, 0xffffffff, 0xaaaaaaaa, 0x55555555))
        == UVector4(0xffffffff, 0, 0x55555555, 0xaaaaaaaa));
    CHECK(Integers.Dot(UVector4(4, 3, 2, 1)) == 20);
    CHECK(Integers.DotV(UVector4(4, 3, 2, 1)) == UVector4(20, 20, 20, 20));
    CHECK(Integers.ToFloat() == Vector4(1, 2, 3, 4));
    CHECK(UVector4(0x3f800000, 0x40000000, 0x40400000, 0x40800000).ReinterpretAsFloat() == Vector4(1, 2, 3, 4));
    CHECK(UVector4(0x80000000, 0x7ffffffe, 0, 0).GetTrues() == 1);
    CHECK(UVector4(0x80000000, 0, 0x80000000, 0).TestAnyXYZTrue());
    CHECK(UVector4(0xffffffff, 0xffffffff, 0xffffffff, 0).TestAllXYZTrue());
    CHECK(UVector4(0x80000000, 0x80000000, 0, 0).CountTrues() == 2);
    CHECK(Integers.LogicalShiftLeft<1>() == UVector4(2, 4, 6, 8));
    CHECK(UVector4(0x80000000, 8, 16, 32).LogicalShiftRight<1>() == UVector4(0x40000000, 4, 8, 16));
    CHECK(UVector4(0x80000000, 0x7ffffff0, 0, 0).ArithmeticShiftRight<1>()
        == UVector4(0xc0000000, 0x3ffffff8, 0, 0));
    CHECK(UVector4(0x00020001, 0x00040003, 0x00060005, 0x00080007).Expand4Uint16Lo()
        == UVector4(1, 2, 3, 4));
    CHECK(UVector4(0x00020001, 0x00040003, 0x00060005, 0x00080007).Expand4Uint16Hi()
        == UVector4(5, 6, 7, 8));
    const UVector4 BytePacked(0x04030201, 0x08070605, 0x0c0b0a09, 0x100f0e0d);
    CHECK(BytePacked.Expand4Byte0() == UVector4(1, 2, 3, 4));
    CHECK(BytePacked.Expand4Byte4() == UVector4(5, 6, 7, 8));
    CHECK(BytePacked.Expand4Byte8() == UVector4(9, 10, 11, 12));
    CHECK(BytePacked.Expand4Byte12() == UVector4(13, 14, 15, 16));
    CHECK(UVector4(1, 2, 3, 4).ShiftComponents4Minus(2) == UVector4(3, 4, 0, 0));
    CHECK(UVector4::sSort4True(UVector4(0x80000000, 0, 0xffffffff, 0), UVector4(1, 2, 3, 4))
        == UVector4(1, 3, 4, 4));
    const KT::UInt32 Input[4] = {11, 22, 33, 44};
    CHECK(UVector4::sLoadInt(Input) == UVector4(11, 0, 0, 0));
    CHECK(UVector4::sLoadInt4(Input) == UVector4(11, 22, 33, 44));
    alignas(16) const KT::UInt32 AlignedInput[4] = {5, 6, 7, 8};
    CHECK(UVector4::sLoadInt4Aligned(AlignedInput) == UVector4(5, 6, 7, 8));
    CHECK(UVector4::sGatherInt4<4>(Input, UVector4(3, 0, 2, 1)) == UVector4(44, 11, 33, 22));
    KT::UInt32 Output[4] = {};
    Integers.StoreInt4(Output);
    CHECK(Output[0] == 1 && Output[1] == 2 && Output[2] == 3 && Output[3] == 4);
    alignas(16) KT::UInt32 AlignedOutput[4] = {};
    Integers.StoreInt4Aligned(AlignedOutput);
    CHECK(AlignedOutput[0] == 1 && AlignedOutput[3] == 4);
}

void TestVector3()
{
    SECTION("Vector3");
    CHECK(sizeof(Vector3) == 16 && alignof(Vector3) == 16);
    CHECK(Vector3::sOne() == Vector3(1.0f, 1.0f, 1.0f));
    CHECK(Vector3::sAxisX().GetX() == 1.0f && Vector3::sAxisY().GetY() == 1.0f && Vector3::sAxisZ().GetZ() == 1.0f);
    const Vector3 Zero;
    CHECK(Zero.GetX() == 0.0f && Zero.GetY() == 0.0f && Zero.GetZ() == 0.0f);
    const Vector3 A(1.0f, 2.0f, 2.0f);
    CHECK(A.GetX() == 1.0f && A.GetY() == 2.0f && A.GetZ() == 2.0f);
    CHECK(Vector3(Float3(3.0f, 4.0f, 5.0f)) == Vector3(3.0f, 4.0f, 5.0f));
    CHECK(Vector3(Vector4(3.0f, 4.0f, 5.0f, 6.0f)) == Vector3(3.0f, 4.0f, 5.0f));
    CHECK(Vector3::sLoadFloat3Unsafe(Float3(6.0f, 7.0f, 8.0f)) == Vector3(6.0f, 7.0f, 8.0f));
    CHECK(Vector3::sNaN().IsNaN());
    CHECK(Vector3(1.0f, 2.0f, 3.0f).IsClose(Vector3(1.0f, 2.0f, 3.000001f)));
    CHECK(Vector3::sZero().IsNearZero() && Vector3::sAxisX().IsNormalized());
    CHECK(Vector3::sAxisX().GetLowestComponentIndex() == 1);
    CHECK(Vector3(1.0f, 2.0f, 3.0f).GetHighestComponentIndex() == 2);
    CHECK(Vector3::sMin(Vector3(1.0f, 4.0f, 3.0f), Vector3(2.0f, 3.0f, 4.0f)) == Vector3(1.0f, 3.0f, 3.0f));
    CHECK(Vector3::sMax(Vector3(1.0f, 4.0f, 3.0f), Vector3(2.0f, 3.0f, 4.0f)) == Vector3(2.0f, 4.0f, 4.0f));
    CHECK(Vector3::sClamp(Vector3(-1.0f, 2.0f, 5.0f), Vector3::sZero(), Vector3::sReplicate(4.0f))
        == Vector3(0.0f, 2.0f, 4.0f));
    CHECK(Vector3::sEquals(Vector3(1.0f, 2.0f, 3.0f), Vector3(1.0f, 0.0f, 3.0f)).GetTrues() == 13);
    CHECK(Vector3::sLess(Vector3(1.0f, 2.0f, 3.0f), Vector3(2.0f, 2.0f, 4.0f)).GetTrues() == 13);
    CHECK(Vector3::sGreaterOrEqual(Vector3(1.0f, 2.0f, 3.0f), Vector3(1.0f, 3.0f, 3.0f)).GetTrues() == 13);
    CHECK(Vector3::sSelect(Vector3::sZero(), Vector3::sOne(), UVector4(0xffffffff, 0, 0xffffffff, 0)).GetX() == 1.0f);
    CHECK(Vector3(2.0f, 3.0f, 4.0f) * Vector3(5.0f, 6.0f, 7.0f) == Vector3(10.0f, 18.0f, 28.0f));
    CHECK(Vector3(8.0f, 9.0f, 10.0f) / Vector3(2.0f, 3.0f, 5.0f) == Vector3(4.0f, 3.0f, 2.0f));
    CHECK((Vector3(1.0f, 2.0f, 3.0f).Swizzle<2, 0, 1>() == Vector3(3.0f, 1.0f, 2.0f)));
    CHECK(Vector3(1.0f, 2.0f, 3.0f).SplatY() == Vector3::sReplicate(2.0f));
    CHECK(Vector3(-1.0f, 2.0f, -3.0f).Abs() == Vector3(1.0f, 2.0f, 3.0f));
    CHECK(Vector3(2.0f, 4.0f, 5.0f).Reciprocal().IsClose(Vector3(0.5f, 0.25f, 0.2f), 1.0e-12f));
    CHECK(Vector3::sAxisX().DotV(Vector3::sAxisY()) == Vector3::sZero());
    CHECK(Vector3::sAxisX().DotV4(Vector3::sAxisY()) == Vector4::sZero());
    CHECK_NEAR(Vector3(1.0f, 2.0f, 3.0f).ReduceMin(), 1.0f);
    CHECK_NEAR(Vector3(1.0f, 2.0f, 3.0f).ReduceMax(), 3.0f);
    CHECK_NEAR(Vector3(1.0f, 2.0f, 3.0f).ReduceSum(), 6.0f);
    CHECK(Vector3(1.0f, 4.0f, 9.0f).Sqrt() == Vector3(1.0f, 2.0f, 3.0f));
    CHECK(Vector3(1.0f, 2.0f, 3.0f).GetNormalizedPerpendicular().IsNormalized());
    CHECK(Vector3(-1.0f, 0.0f, 3.0f).GetSign() == Vector3(-1.0f, 1.0f, 1.0f));
    CHECK((Vector3(1.0f, 2.0f, 3.0f).FlipSign<-1, 1, -1>() == Vector3(-1.0f, 2.0f, -3.0f)));
    CHECK(Vector3(1.0f, 2.0f, 3.0f).CrossPrecise(Vector3(4.0f, 5.0f, 6.0f))
        == Vector3(-3.0f, 6.0f, -3.0f));
    CHECK(Vector3::sUnitSpherical(1.57079632679f, 0.0f).IsClose(Vector3::sAxisX(), 1.0e-12f));
    const Vector3 CompressedSource = Vector3(1.0f, -2.0f, 3.0f).Normalized();
    const Vector3 Decompressed = Vector3::sDecompressUnitVector(CompressedSource.CompressUnitVector());
    CHECK(Decompressed.IsNormalized(1.0e-5f));
    CHECK(CompressedSource.IsClose(Decompressed, 2.0e-8f));
    const auto& UnitSphere = Vector3::sUnitSphere();
    CHECK(UnitSphere.Size() == 1026 && UnitSphere.GetCapacity() == 1026);
    for (const Vector3& SpherePoint : UnitSphere)
        CHECK(SpherePoint.IsNormalized(1.0e-5f));
    MathTests::Random Vector3CompressionGenerator(0x4a4f4c54u);
    RandomRange ActualRandom{0x12345678u};
    RandomRange ExpectedRandom = ActualRandom;
    const auto ExpectedRandomUnit = [&ExpectedRandom]() noexcept
    {
        return static_cast<float>(ExpectedRandom() - ExpectedRandom.min())
            / static_cast<float>(ExpectedRandom.max() - ExpectedRandom.min());
    };
    const float ExpectedZ = -1.0f + 2.0f * ExpectedRandomUnit();
    const float ExpectedRadius = MathScalar::Sqrt(1.0f - MathScalar::Square(ExpectedZ));
    const float ExpectedTheta = MathScalar::TwoPi * ExpectedRandomUnit();
    Vector4 ExpectedSine;
    Vector4 ExpectedCosine;
    Vector4::sReplicate(ExpectedTheta).SinCos(ExpectedSine, ExpectedCosine);
    CHECK(Vector3::sRandom(ActualRandom)
        == Vector3(ExpectedRadius * ExpectedSine.GetX(), ExpectedRadius * ExpectedCosine.GetX(), ExpectedZ));
    RandomRange RandomVectorGenerator{0x12345678u};
    for (int Index = 0; Index < 512; ++Index)
    {
        const Vector3 Original = RandomVector3(Vector3CompressionGenerator).Normalized();
        const Vector3 RoundTrip = Vector3::sDecompressUnitVector(Original.CompressUnitVector());
        CHECK(RoundTrip.IsNormalized(1.0e-5f));
        CHECK(RoundTrip.IsClose(Original, 2.0e-8f));
        const Vector3 RandomUnit = Vector3::sRandom(RandomVectorGenerator);
        CHECK(RandomUnit.IsNormalized(1.0e-5f));
    }
    Vector3 Mutated = A;
    Mutated.SetZ(7.0f);
    CHECK(Mutated.GetZ() == 7.0f);
    Mutated += Vector3(1.0f, 2.0f, 3.0f);
    CHECK(Mutated.GetZ() == 10.0f);
    Mutated *= 2.0f;
    CHECK(Mutated.GetZ() == 20.0f);

    CHECK_NEAR(A.Length(), 3.0f);
    CHECK_NEAR(A.LengthSquared(), 9.0f);
    CHECK_VEC3(A.Normalized(), 1.0f / 3.0f, 2.0f / 3.0f, 2.0f / 3.0f);
    CHECK_NEAR(A.Normalized().Length(), 1.0f);
    CHECK_VEC3(Zero.NormalizedOr(Vector3::sZero()), 0.0f, 0.0f, 0.0f);
    CHECK_VEC3(Vector3(0.0f, -5.0f, 0.0f).Normalized(), 0.0f, -1.0f, 0.0f);

    CHECK_NEAR(Vector3(1.0f, 2.0f, 3.0f).Dot(Vector3(4.0f, 5.0f, 6.0f)), 32.0f);
    CHECK_NEAR(Vector3(1.0f, 0.0f, 0.0f).Dot(Vector3(0.0f, 1.0f, 0.0f)), 0.0f);
    CHECK_NEAR(Vector3(1.0f, 0.0f, 0.0f).Dot(Vector3(-1.0f, 0.0f, 0.0f)), -1.0f);

    // базис правой системы координат
    const Vector3 X(1.0f, 0.0f, 0.0f), Y(0.0f, 1.0f, 0.0f), Z(0.0f, 0.0f, 1.0f);
    CHECK_VEC3(X.Cross(Y), 0.0f, 0.0f, 1.0f);
    CHECK_VEC3(Y.Cross(Z), 1.0f, 0.0f, 0.0f);
    CHECK_VEC3(Z.Cross(X), 0.0f, 1.0f, 0.0f);
    CHECK_VEC3(Y.Cross(X), 0.0f, 0.0f, -1.0f);
    CHECK_VEC3(Vector3(1.0f, 2.0f, 3.0f).Cross(Vector3(4.0f, 5.0f, 6.0f)), -3.0f, 6.0f, -3.0f);
    CHECK_VEC3(A.Cross(A), 0.0f, 0.0f, 0.0f);
    CHECK_VEC3(A.Cross(A * 3.0f), 0.0f, 0.0f, 0.0f);

    CHECK_NEAR(Vector3(1.0f, 1.0f, 1.0f).Distance(Vector3(4.0f, 5.0f, 1.0f)), 5.0f);
    CHECK_NEAR(Vector3(1.0f, 1.0f, 1.0f).DistanceSquared(Vector3(4.0f, 5.0f, 1.0f)), 25.0f);
    CHECK_NEAR(A.Distance(A), 0.0f);

    CHECK_VEC3(A + Vector3(1.0f, 1.0f, 1.0f), 2.0f, 3.0f, 3.0f);
    CHECK_VEC3(A - Vector3(1.0f, 1.0f, 1.0f), 0.0f, 1.0f, 1.0f);
    CHECK_VEC3(A * 2.0f, 2.0f, 4.0f, 4.0f);
    CHECK_VEC3(2.0f * A, 2.0f, 4.0f, 4.0f);
    CHECK_VEC3(A / 2.0f, 0.5f, 1.0f, 1.0f);
    CHECK_VEC3(-A, -1.0f, -2.0f, -2.0f);

    Vector3 B = A;
    B += Vector3(1.0f, 1.0f, 1.0f);
    CHECK_VEC3(B, 2.0f, 3.0f, 3.0f);
    B -= Vector3(1.0f, 1.0f, 1.0f);
    CHECK_VEC3(B, 1.0f, 2.0f, 2.0f);
    B *= 4.0f;
    CHECK_VEC3(B, 4.0f, 8.0f, 8.0f);
    B /= 4.0f;
    CHECK_VEC3(B, 1.0f, 2.0f, 2.0f);

    CHECK(A == Vector3(1.0f, 2.0f, 2.0f));
    CHECK(!(A == Vector3(1.0f, 2.0f, 3.0f)));
    CHECK(A != Vector3(0.0f, 2.0f, 2.0f));
    CHECK(!(A != Vector3(1.0f, 2.0f, 2.0f)));

    // свойства на случайных векторах
    MathTests::Random Generator(12345);
    for (int Iteration = 0; Iteration < 500; ++Iteration)
    {
        const Vector3 P = RandomVector3(Generator);
        const Vector3 Q = RandomVector3(Generator);
        const Vector3 R = RandomVector3(Generator);

        CHECK_NEAR_TOL(P.Dot(Q), Q.Dot(P), 1e-3f);
        CHECK_NEAR_TOL(P.Dot(Q + R), P.Dot(Q) + P.Dot(R), 1e-2f);
        CHECK_NEAR_TOL(P.Dot(Q * 3.0f), P.Dot(Q) * 3.0f, 1e-2f);

        const Vector3 C = P.Cross(Q);
        CHECK_NEAR_TOL(C.Dot(P), 0.0f, 1e-2f);
        CHECK_NEAR_TOL(C.Dot(Q), 0.0f, 1e-2f);
        const Vector3 Anti = Q.Cross(P);
        CHECK_NEAR_TOL(C.GetX(), -Anti.GetX(), 1e-3f);
        CHECK_NEAR_TOL(C.GetY(), -Anti.GetY(), 1e-3f);
        CHECK_NEAR_TOL(C.GetZ(), -Anti.GetZ(), 1e-3f);
        // |P x Q|^2 = |P|^2 |Q|^2 - (P . Q)^2
        const float Lagrange = P.LengthSquared() * Q.LengthSquared() - P.Dot(Q) * P.Dot(Q);
        CHECK_NEAR_TOL(C.LengthSquared(), Lagrange, 0.5f);

        CHECK(P.Length() <= 1e-3f || MathTests::Near(P.Normalized().Length(), 1.0f));
        CHECK((P + Q).Length() <= P.Length() + Q.Length() + 1e-3f);
        CHECK_NEAR_TOL(P.Distance(Q), Q.Distance(P), 1e-4f);
        CHECK_NEAR_TOL(P.Distance(Q), (P - Q).Length(), 1e-4f);
        CHECK_NEAR_TOL((P + Q - Q).GetX(), P.GetX(), 1e-4f);
    }
}

void TestVector4()
{
    SECTION("Vector4");
    const KernelCompiler::Simd::Float4 SimdA = KernelCompiler::Simd::Float4::Load(1.0f, -2.0f, 3.5f, 4.0f);
    const KernelCompiler::Simd::Float4 SimdB = KernelCompiler::Simd::Float4::Load(2.0f, 4.0f, -0.5f, 8.0f);
    const KernelCompiler::Simd::Float4 SimdSum = SimdA.Add(SimdB);
    const KernelCompiler::Simd::Float4 SimdDifference = SimdA.Subtract(SimdB);
    const KernelCompiler::Simd::Float4 SimdProduct = SimdA.Multiply(SimdB);
    CHECK(SimdSum.Get<0>() == 3.0f && SimdSum.Get<1>() == 2.0f
        && SimdSum.Get<2>() == 3.0f && SimdSum.Get<3>() == 12.0f);
    CHECK(SimdDifference.Get<0>() == -1.0f && SimdDifference.Get<1>() == -6.0f
        && SimdDifference.Get<2>() == 4.0f && SimdDifference.Get<3>() == -4.0f);
    CHECK(SimdProduct.Get<0>() == 2.0f && SimdProduct.Get<1>() == -8.0f
        && SimdProduct.Get<2>() == -1.75f && SimdProduct.Get<3>() == 32.0f);
    CHECK(SimdProduct.ReduceSum() == (2.0f + -8.0f) + (-1.75f + 32.0f));

    CHECK(sizeof(Vector4) == 16 && alignof(Vector4) == 16);
    CHECK(Vector4::One() == Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    CHECK(Vector4::Replicate(3.0f).GetW() == 3.0f);
    CHECK(Vector4::sZero() == Vector4::Zero());
    CHECK(Vector4::sOne() == Vector4::One());
    CHECK(Vector4::sReplicate(4.0f) == Vector4::Replicate(4.0f));
    CHECK(Vector4::sNaN().IsNaN());
    const Vector3 Source3(1.0f, 2.0f, 3.0f);
    CHECK(Vector4(Source3) == Vector4(1.0f, 2.0f, 3.0f, 3.0f));
    CHECK(Vector4(Source3, 4.0f) == Vector4(1.0f, 2.0f, 3.0f, 4.0f));
    const Float4 Stored(4.0f, 3.0f, 2.0f, 1.0f);
    CHECK(Vector4::sLoadFloat4(&Stored) == Vector4(4.0f, 3.0f, 2.0f, 1.0f));
    CHECK(Vector4::sLoadFloat4Aligned(&Stored) == Vector4(4.0f, 3.0f, 2.0f, 1.0f));
    const float GatherValues[4] = {1.5f, 2.5f, 3.5f, 4.5f};
    CHECK(Vector4::sGatherFloat4<4>(GatherValues, UVector4(3, 1, 0, 2)) == Vector4(4.5f, 2.5f, 1.5f, 3.5f));
    const Vector4 Zero;
    CHECK(Zero.X == 0.0f && Zero.Y == 0.0f && Zero.Z == 0.0f && Zero.W == 0.0f);
    const Vector4 A(1.0f, 2.0f, 2.0f, 4.0f);
    CHECK(A.X == 1.0f && A.Y == 2.0f && A.Z == 2.0f && A.W == 4.0f);
    CHECK(A[0] == 1.0f && A[3] == 4.0f);
    CHECK(A.SplatY() == Vector4(2.0f, 2.0f, 2.0f, 2.0f));
    CHECK(A.SplatW3() == Vector3(4.0f, 4.0f, 4.0f));
    CHECK(A.GetLowestComponentIndex() == 0 && A.GetHighestComponentIndex() == 3);
    CHECK(A.IsClose(A + Vector4(0.0f, 0.0f, 0.0f, 0.0000001f)));
    CHECK(!A.IsNearZero() && Vector4::Zero().IsNearZero());
    CHECK(Vector4::Replicate(0.5f).IsNormalized());
    CHECK(!A.IsNormalized());
    CHECK((Vector4(1.0f, 2.0f, 3.0f, 4.0f).Swizzle<3, 2, 1, 0>() == Vector4(4.0f, 3.0f, 2.0f, 1.0f)));

    CHECK_NEAR(A.Length(), 5.0f);
    CHECK_NEAR(A.LengthSquared(), 25.0f);
    CHECK_VEC4(A.Normalized(), 0.2f, 0.4f, 0.4f, 0.8f);
    CHECK_NEAR(A.Normalized().Length(), 1.0f);
    CHECK(Zero.Normalized().IsNaN());

    CHECK_NEAR(Vector4(1.0f, 2.0f, 3.0f, 4.0f).Dot(Vector4(5.0f, 6.0f, 7.0f, 8.0f)), 70.0f);
    CHECK_NEAR(Vector4(1.0f, 0.0f, 0.0f, 0.0f).Dot(Vector4(0.0f, 0.0f, 0.0f, 1.0f)), 0.0f);
    CHECK_NEAR(A.Dot(A), A.LengthSquared());
    CHECK(A.DotV(Vector4(1.0f, 1.0f, 1.0f, 1.0f)) == Vector4::Replicate(9.0f));

    CHECK_NEAR(Vector4(1.0f, 1.0f, 1.0f, 1.0f).Distance(Vector4(4.0f, 5.0f, 1.0f, 1.0f)), 5.0f);
    CHECK_NEAR(Vector4(1.0f, 1.0f, 1.0f, 1.0f).DistanceSquared(Vector4(4.0f, 5.0f, 1.0f, 1.0f)), 25.0f);

    CHECK_VEC4(A + Vector4(1.0f, 1.0f, 1.0f, 1.0f), 2.0f, 3.0f, 3.0f, 5.0f);
    CHECK_VEC4(A - Vector4(1.0f, 1.0f, 1.0f, 1.0f), 0.0f, 1.0f, 1.0f, 3.0f);
    CHECK_VEC4(A * 2.0f, 2.0f, 4.0f, 4.0f, 8.0f);
    CHECK(A * Vector4(2.0f, 3.0f, 4.0f, 5.0f) == Vector4(2.0f, 6.0f, 8.0f, 20.0f));
    CHECK(A / Vector4(1.0f, 2.0f, 1.0f, 2.0f) == Vector4(1.0f, 1.0f, 2.0f, 2.0f));
    CHECK_VEC4(2.0f * A, 2.0f, 4.0f, 4.0f, 8.0f);
    CHECK_VEC4(A / 2.0f, 0.5f, 1.0f, 1.0f, 2.0f);
    CHECK_VEC4(-A, -1.0f, -2.0f, -2.0f, -4.0f);

    Vector4 B = A;
    B += Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    CHECK_VEC4(B, 2.0f, 3.0f, 3.0f, 5.0f);
    B -= Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    CHECK_VEC4(B, 1.0f, 2.0f, 2.0f, 4.0f);
    B *= 2.0f;
    CHECK_VEC4(B, 2.0f, 4.0f, 4.0f, 8.0f);
    B /= 2.0f;
    CHECK_VEC4(B, 1.0f, 2.0f, 2.0f, 4.0f);
    B *= Vector4(2.0f, 3.0f, 4.0f, 5.0f);
    CHECK(B == Vector4(2.0f, 6.0f, 8.0f, 20.0f));
    B /= Vector4(2.0f, 3.0f, 4.0f, 5.0f);
    CHECK(B == A);

    CHECK(Vector4::sMin(A, Vector4(0.0f, 3.0f, 1.0f, 5.0f)) == Vector4(0.0f, 2.0f, 1.0f, 4.0f));
    CHECK(Vector4::sMax(A, Vector4(0.0f, 3.0f, 1.0f, 5.0f)) == Vector4(1.0f, 3.0f, 2.0f, 5.0f));
    const float NegativeZero = KernelCompiler::BitCast<float>(0x80000000u);
    CHECK(KernelCompiler::BitCast<KT::UInt32>(
        Vector4::sMin(Vector4(NegativeZero, 0, 0, 0), Vector4(0, NegativeZero, 0, 0)).GetX()
    ) == 0u);
    CHECK(KernelCompiler::BitCast<KT::UInt32>(
        Vector4::sMax(Vector4(NegativeZero, 0, 0, 0), Vector4(0, NegativeZero, 0, 0)).GetY()
    ) == 0x80000000u);
    CHECK(Vector4::sClamp(Vector4(-1.0f, 1.0f, 3.0f, 5.0f), Vector4::Zero(), Vector4::Replicate(4.0f))
        == Vector4(0.0f, 1.0f, 3.0f, 4.0f));
    CHECK(Vector4::sEquals(A, Vector4(1.0f, 2.0f, 0.0f, 4.0f)).GetTrues() == 11);
    CHECK(Vector4::sLess(Vector4::Zero(), Vector4(1.0f, -1.0f, 2.0f, -2.0f)).GetTrues() == 5);
    CHECK(Vector4::sLessOrEqual(A, A).GetTrues() == 15);
    CHECK(Vector4::sGreater(Vector4(1.0f, -1.0f, 2.0f, -2.0f), Vector4::Zero()).GetTrues() == 5);
    CHECK(Vector4::sGreaterOrEqual(A, A).GetTrues() == 15);
    CHECK(Vector4::sSelect(Vector4::Zero(), A, UVector4(0x80000000u, 0, 0x80000000u, 0xffffffffu))
        == Vector4(1.0f, 0.0f, 2.0f, 4.0f));
    CHECK(Vector4::sFusedMultiplyAdd(A, Vector4::Replicate(2.0f), Vector4::One())
        == Vector4(3.0f, 5.0f, 5.0f, 9.0f));
    CHECK(Vector4::sDifferenceOfProducts(Vector4::Replicate(3.0f), Vector4::Replicate(4.0f),
        Vector4::Replicate(2.0f), Vector4::Replicate(5.0f)) == Vector4::Replicate(2.0f));
    const Vector4 BitsA = UVector4(0x0000000f, 0x0000ff00, 0xaaaaaaaa, 0x12345678).ReinterpretAsFloat();
    const Vector4 BitsB = UVector4(0x000000f0, 0x00000f00, 0x55555555, 0x87654321).ReinterpretAsFloat();
    CHECK(Vector4::sOr(BitsA, BitsB).ReinterpretAsInt()
        == UVector4(0x000000ff, 0x0000ff00, 0xffffffff, 0x97755779));
    CHECK(Vector4::sXor(BitsA, BitsB).ReinterpretAsInt()
        == UVector4(0x000000ff, 0x0000f000, 0xffffffff, 0x95511559));
    CHECK(Vector4::sAnd(BitsA, BitsB).ReinterpretAsInt()
        == UVector4(0, 0x00000f00, 0, 0x02244220));
    Vector4 Sorted(3.0f, 1.0f, 4.0f, 2.0f);
    UVector4 SortedIndices(0, 1, 2, 3);
    Vector4::sSort4(Sorted, SortedIndices);
    CHECK(Sorted == Vector4(1.0f, 2.0f, 3.0f, 4.0f) && SortedIndices == UVector4(1, 3, 0, 2));
    Vector4::sSort4Reverse(Sorted, SortedIndices);
    CHECK(Sorted == Vector4(4.0f, 3.0f, 2.0f, 1.0f) && SortedIndices == UVector4(2, 0, 3, 1));
    Vector4 DuplicateSorted(2.0f, 1.0f, 1.0f, 0.0f);
    UVector4 DuplicateIndices(0, 1, 2, 3);
    Vector4::sSort4(DuplicateSorted, DuplicateIndices);
    CHECK(DuplicateSorted == Vector4(0.0f, 1.0f, 1.0f, 2.0f));
    CHECK(DuplicateIndices == UVector4(3, 2, 1, 0));
    CHECK(Vector4(-0.0f, -1.0f, 2.0f, -3.0f).GetSignBits() == 11);
    CHECK(Vector4(-0.0f, -1.0f, 2.0f, -3.0f).GetSign() == Vector4(-1.0f, -1.0f, 1.0f, -1.0f));
    CHECK(Vector4(-1.0f, -2.0f, 3.0f, -4.0f).Abs() == Vector4(1.0f, 2.0f, 3.0f, 4.0f));
    CHECK(Vector4(1.0f, 2.0f, 4.0f, 8.0f).Reciprocal() == Vector4(1.0f, 0.5f, 0.25f, 0.125f));
    CHECK(Vector4(1.0f, 4.0f, 9.0f, 16.0f).Sqrt() == Vector4(1.0f, 2.0f, 3.0f, 4.0f));
    CHECK(Vector4(1.0f, 2.0f, 3.0f, 4.0f).ReduceMin() == 1.0f);
    CHECK(Vector4(1.0f, 2.0f, 3.0f, 4.0f).ReduceMax() == 4.0f);
    CHECK(Vector4(1.0f, 2.0f, 3.0f, 4.0f).ReduceSum() == 10.0f);
    CHECK(Vector4::sDecompressUnitVector(Vector4(1.0f, 0.0f, 0.0f, 0.0f).CompressUnitVector())
        == Vector4(1.0f, 0.0f, 0.0f, 0.0f));
    CHECK(Vector4::sDecompressUnitVector(Vector4(0.0f, -1.0f, 0.0f, 0.0f).CompressUnitVector())
        == Vector4(0.0f, -1.0f, 0.0f, 0.0f));
    Vector4 Sine;
    Vector4 Cosine;
    const Vector4 Angles(-2.2f, -0.75f, 0.125f, 1.3f);
    Angles.SinCos(Sine, Cosine);
    for (KT::UInt32 Index = 0; Index < 4; ++Index)
    {
        CHECK_NEAR_TOL(Sine[Index] * Sine[Index] + Cosine[Index] * Cosine[Index], 1.0f, 2.0e-6f);
        CHECK_NEAR_TOL(Angles.Tan()[Index] * Cosine[Index], Sine[Index], 2.0e-5f);
    }
    CHECK(Vector4::sReplicate(0.5f).ASin().IsClose(Vector4::sReplicate(0.5235988f), 1.0e-12f));
    CHECK(Vector4::sReplicate(0.5f).ACos().IsClose(Vector4::sReplicate(1.0471976f), 1.0e-12f));
    CHECK(Vector4(-2.0f, -0.5f, 0.5f, 2.0f).ASin().IsClose(
        Vector4(-Vector4::sReplicate(1.57079633f).GetX(), -0.5235988f, 0.5235988f, 1.57079633f),
        1.0e-12f
    ));
    CHECK(Vector4::sReplicate(1.0f).ATan().IsClose(Vector4::sReplicate(0.7853982f), 1.0e-12f));
    CHECK(Vector4::sATan2(Vector4(1.0f, 1.0f, -1.0f, -1.0f), Vector4(1.0f, -1.0f, 1.0f, -1.0f))
        .IsClose(Vector4(0.7853982f, 2.3561945f, -0.7853982f, -2.3561945f), 1.0e-12f));
    const Vector4 UnitForCompression = Vector4(1.0f, -2.0f, 3.0f, -4.0f).Normalized();
    const Vector4 Decompressed = Vector4::sDecompressUnitVector(UnitForCompression.CompressUnitVector());
    CHECK(Decompressed.IsNormalized(1.0e-3f));
    CHECK(Decompressed.IsClose(UnitForCompression, 1.0e-5f));
    MathTests::Random CompressionGenerator(0x4a4f4c54u);
    for (int Index = 0; Index < 512; ++Index)
    {
        const Vector4 Original = Vector4(
            CompressionGenerator.Range(-1.0f, 1.0f),
            CompressionGenerator.Range(-1.0f, 1.0f),
            CompressionGenerator.Range(-1.0f, 1.0f),
            CompressionGenerator.Range(-1.0f, 1.0f)
        ).Normalized();
        const Vector4 RoundTrip = Vector4::sDecompressUnitVector(Original.CompressUnitVector());
        CHECK(RoundTrip.IsNormalized(1.0e-3f));
        CHECK(RoundTrip.IsClose(Original, 2.0e-5f));
    }
    CHECK(Vector4(1.9f, -2.9f, 3.0f, -4.0f).ToInt() == UVector4(1, 0xfffffffeu, 3, 0xfffffffcu));
    CHECK(Vector4(2147483648.0f, 0.0f, 0.0f, 0.0f).ToInt()[0] == 0x80000000u);
    CHECK((Vector4(1.0f, -2.0f, 3.0f, -4.0f).FlipSign<-1, 1, -1, 1>()
        == Vector4(-1.0f, -2.0f, -3.0f, -4.0f)));
    Float4 StoredOutput;
    A.StoreFloat4(&StoredOutput);
    CHECK(StoredOutput == Float4(1.0f, 2.0f, 2.0f, 4.0f));

    CHECK(A == Vector4(1.0f, 2.0f, 2.0f, 4.0f));
    CHECK(!(A == Vector4(1.0f, 2.0f, 2.0f, 5.0f)));
    CHECK(A != Vector4(1.0f, 2.0f, 2.0f, 5.0f));
    CHECK(!(A != Vector4(1.0f, 2.0f, 2.0f, 4.0f)));
}
