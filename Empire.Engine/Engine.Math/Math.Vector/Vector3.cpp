#include "Empire.Engine/Engine.Math/Math.Vector/Vector3.hpp"
#include "Empire.Engine/Engine.Math/Math.Vector/UVector4.hpp"
#include "Empire.Engine/Engine.Math/Math.Vector/Vector4.hpp"
#include "Empire.Engine/Engine.Math/Math.Vector/Trigonometry.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireEngine
        {
            namespace EngineMath
            {
                namespace MathVector
                {
                    using namespace EngineKernel;

                    Vector3::Vector3(const Vector4& Value) noexcept
                        : mX(Value.GetX()), mY(Value.GetY()), mZ(Value.GetZ()), mW(Value.GetZ())
                    {
                    }

                    Vector3 Vector3::sNaN() noexcept
                    {
                        return sReplicate(KernelCompiler::BitCast<KernelTypes::Float>(0x7fc00000u));
                    }

                    Vector3 Vector3::sLoadFloat3Unsafe(const Float3& Value) noexcept
                    {
                        return {Value.X, Value.Y, Value.Z};
                    }

                    Vector3 Vector3::sMin(const Vector3& A, const Vector3& B) noexcept
                    {
                        return {A.mX < B.mX ? A.mX : B.mX,
                            A.mY < B.mY ? A.mY : B.mY,
                            A.mZ < B.mZ ? A.mZ : B.mZ};
                    }

                    Vector3 Vector3::sMax(const Vector3& A, const Vector3& B) noexcept
                    {
                        return {A.mX > B.mX ? A.mX : B.mX,
                            A.mY > B.mY ? A.mY : B.mY,
                            A.mZ > B.mZ ? A.mZ : B.mZ};
                    }

                    Vector3 Vector3::sClamp(const Vector3& Value, const Vector3& Minimum, const Vector3& Maximum) noexcept
                    {
                        return sMax(sMin(Value, Maximum), Minimum);
                    }

                    UVector4 Vector3::sEquals(const Vector3& A, const Vector3& B) noexcept
                    {
                        const KernelTypes::UInt32 X = A.mX == B.mX ? 0xffffffffu : 0;
                        const KernelTypes::UInt32 Y = A.mY == B.mY ? 0xffffffffu : 0;
                        const KernelTypes::UInt32 Z = A.mZ == B.mZ ? 0xffffffffu : 0;
                        return {X, Y, Z, Z};
                    }

                    UVector4 Vector3::sLess(const Vector3& A, const Vector3& B) noexcept
                    {
                        const KernelTypes::UInt32 X = A.mX < B.mX ? 0xffffffffu : 0;
                        const KernelTypes::UInt32 Y = A.mY < B.mY ? 0xffffffffu : 0;
                        const KernelTypes::UInt32 Z = A.mZ < B.mZ ? 0xffffffffu : 0;
                        return {X, Y, Z, Z};
                    }

                    UVector4 Vector3::sLessOrEqual(const Vector3& A, const Vector3& B) noexcept
                    {
                        const KernelTypes::UInt32 X = A.mX <= B.mX ? 0xffffffffu : 0;
                        const KernelTypes::UInt32 Y = A.mY <= B.mY ? 0xffffffffu : 0;
                        const KernelTypes::UInt32 Z = A.mZ <= B.mZ ? 0xffffffffu : 0;
                        return {X, Y, Z, Z};
                    }

                    UVector4 Vector3::sGreater(const Vector3& A, const Vector3& B) noexcept
                    {
                        return sLess(B, A);
                    }

                    UVector4 Vector3::sGreaterOrEqual(const Vector3& A, const Vector3& B) noexcept
                    {
                        return sLessOrEqual(B, A);
                    }

                    Vector3 Vector3::sFusedMultiplyAdd(const Vector3& A, const Vector3& B, const Vector3& Addend) noexcept
                    {
                        return {A.mX * B.mX + Addend.mX, A.mY * B.mY + Addend.mY, A.mZ * B.mZ + Addend.mZ};
                    }

                    Vector3 Vector3::sSelect(const Vector3& NotSet, const Vector3& Set, const UVector4& Control) noexcept
                    {
                        const auto Select = [](KernelTypes::Float A, KernelTypes::Float B, KernelTypes::UInt32 Mask) noexcept
                        {
                            const KernelTypes::UInt32 BitsA = KernelCompiler::BitCast<KernelTypes::UInt32>(A);
                            const KernelTypes::UInt32 BitsB = KernelCompiler::BitCast<KernelTypes::UInt32>(B);
                            return KernelCompiler::BitCast<KernelTypes::Float>(
                                (BitsA & ~Mask) | (BitsB & Mask)
                            );
                        };
                        return {
                            Select(NotSet.mX, Set.mX, Control[0]),
                            Select(NotSet.mY, Set.mY, Control[1]),
                            Select(NotSet.mZ, Set.mZ, Control[2])
                        };
                    }

                    Vector3 Vector3::sOr(const Vector3& A, const Vector3& B) noexcept
                    {
                        return {
                            KernelCompiler::BitCast<KernelTypes::Float>(
                                KernelCompiler::BitCast<KernelTypes::UInt32>(A.mX) | KernelCompiler::BitCast<KernelTypes::UInt32>(B.mX)),
                            KernelCompiler::BitCast<KernelTypes::Float>(
                                KernelCompiler::BitCast<KernelTypes::UInt32>(A.mY) | KernelCompiler::BitCast<KernelTypes::UInt32>(B.mY)),
                            KernelCompiler::BitCast<KernelTypes::Float>(
                                KernelCompiler::BitCast<KernelTypes::UInt32>(A.mZ) | KernelCompiler::BitCast<KernelTypes::UInt32>(B.mZ))
                        };
                    }

                    Vector3 Vector3::sXor(const Vector3& A, const Vector3& B) noexcept
                    {
                        return {
                            KernelCompiler::BitCast<KernelTypes::Float>(
                                KernelCompiler::BitCast<KernelTypes::UInt32>(A.mX) ^ KernelCompiler::BitCast<KernelTypes::UInt32>(B.mX)),
                            KernelCompiler::BitCast<KernelTypes::Float>(
                                KernelCompiler::BitCast<KernelTypes::UInt32>(A.mY) ^ KernelCompiler::BitCast<KernelTypes::UInt32>(B.mY)),
                            KernelCompiler::BitCast<KernelTypes::Float>(
                                KernelCompiler::BitCast<KernelTypes::UInt32>(A.mZ) ^ KernelCompiler::BitCast<KernelTypes::UInt32>(B.mZ))
                        };
                    }

                    Vector3 Vector3::sAnd(const Vector3& A, const Vector3& B) noexcept
                    {
                        return {
                            KernelCompiler::BitCast<KernelTypes::Float>(
                                KernelCompiler::BitCast<KernelTypes::UInt32>(A.mX) & KernelCompiler::BitCast<KernelTypes::UInt32>(B.mX)),
                            KernelCompiler::BitCast<KernelTypes::Float>(
                                KernelCompiler::BitCast<KernelTypes::UInt32>(A.mY) & KernelCompiler::BitCast<KernelTypes::UInt32>(B.mY)),
                            KernelCompiler::BitCast<KernelTypes::Float>(
                                KernelCompiler::BitCast<KernelTypes::UInt32>(A.mZ) & KernelCompiler::BitCast<KernelTypes::UInt32>(B.mZ))
                        };
                    }

                    Vector3 Vector3::sUnitSpherical(KernelTypes::Float Theta, KernelTypes::Float Phi) noexcept
                    {
                        const KernelTypes::Float SinTheta = MathScalar::Sin(Theta);
                        const KernelTypes::Float CosTheta = MathScalar::Cos(Theta);
                        const KernelTypes::Float SinPhi = MathScalar::Sin(Phi);
                        const KernelTypes::Float CosPhi = MathScalar::Cos(Phi);
                        return {SinTheta * CosPhi, SinTheta * SinPhi, CosTheta};
                    }

                    Vector3 Vector3::sRandomFromZTheta(KernelTypes::Float Z, KernelTypes::Float Theta) noexcept
                    {
                        const KernelTypes::Float Radius = MathScalar::Sqrt(1.0f - MathScalar::Square(Z));
                        Vector4 Sine;
                        Vector4 Cosine;
                        Vector4::sReplicate(Theta).SinCos(Sine, Cosine);
                        return {Radius * Sine.GetX(), Radius * Cosine.GetX(), Z};
                    }

                    KernelTypes::Bool Vector3::IsClose(const Vector3& Other, KernelTypes::Float MaxDistSq) const noexcept
                    {
                        return (*this - Other).LengthSq() <= MaxDistSq;
                    }

                    KernelTypes::Bool Vector3::IsNearZero(KernelTypes::Float MaxDistSq) const noexcept
                    {
                        return LengthSq() <= MaxDistSq;
                    }

                    KernelTypes::Bool Vector3::IsNormalized(KernelTypes::Float Tolerance) const noexcept
                    {
                        return MathScalar::Abs(LengthSq() - 1.0f) <= Tolerance;
                    }

                    KernelTypes::Bool Vector3::IsNaN() const noexcept
                    {
                        return mX != mX || mY != mY || mZ != mZ;
                    }

                    Vector3 Vector3::operator/(const Vector3& Other) const noexcept
                    {
                        return {mX / Other.mX, mY / Other.mY, mZ / Other.mZ};
                    }

                    KernelTypes::Int32 Vector3::GetLowestComponentIndex() const noexcept
                    {
                        return mX < mY ? (mZ < mX ? 2 : 0) : (mZ < mY ? 2 : 1);
                    }

                    KernelTypes::Int32 Vector3::GetHighestComponentIndex() const noexcept
                    {
                        return mX > mY ? (mZ > mX ? 2 : 0) : (mZ > mY ? 2 : 1);
                    }

                    Vector3 Vector3::Abs() const noexcept
                    {
                        return {MathScalar::Abs(mX), MathScalar::Abs(mY), MathScalar::Abs(mZ)};
                    }

                    Vector3 Vector3::Reciprocal() const noexcept
                    {
                        return {1.0f / mX, 1.0f / mY, 1.0f / mZ};
                    }

                    Vector3 Vector3::sDifferenceOfProducts(
                        const Vector3& A, const Vector3& B, const Vector3& C, const Vector3& D
                    ) noexcept
                    {
                        return {
                            MathScalar::DifferenceOfProducts(A.mX, B.mX, C.mX, D.mX),
                            MathScalar::DifferenceOfProducts(A.mY, B.mY, C.mY, D.mY),
                            MathScalar::DifferenceOfProducts(A.mZ, B.mZ, C.mZ, D.mZ)
                        };
                    }

                    Vector3 Vector3::Cross(const Vector3& Other) const noexcept
                    {
                        return {mY * Other.mZ - mZ * Other.mY,
                                mZ * Other.mX - mX * Other.mZ,
                                mX * Other.mY - mY * Other.mX};
                    }

                    Vector3 Vector3::CrossPrecise(const Vector3& Other) const noexcept
                    {
                        return sDifferenceOfProducts(
                            {mY, mZ, mX}, {Other.mZ, Other.mX, Other.mY},
                            {mZ, mX, mY}, {Other.mY, Other.mZ, Other.mX}
                        );
                    }

                    Vector3 Vector3::DotV(const Vector3& Other) const noexcept
                    {
                        return sReplicate(Dot(Other));
                    }

                    Vector4 Vector3::DotV4(const Vector3& Other) const noexcept
                    {
                        const KernelTypes::Float X = mX * Other.mX;
                        const KernelTypes::Float Y = mY * Other.mY;
                        const KernelTypes::Float Z = mZ * Other.mZ;
                        return {X + Y + Z, X + Y + Z, X + Y + Z, X + Y + Z};
                    }

                    KernelTypes::Float Vector3::Dot(const Vector3& Other) const noexcept
                    {
                        return mX * Other.mX + mY * Other.mY + mZ * Other.mZ;
                    }

                    KernelTypes::Float Vector3::LengthSq() const noexcept
                    {
                        return Dot(*this);
                    }

                    KernelTypes::Float Vector3::Length() const noexcept
                    {
                        return MathScalar::Sqrt(LengthSq());
                    }

                    Vector3 Vector3::Normalized() const noexcept
                    {
                        return *this / Length();
                    }

                    Vector3 Vector3::NormalizedOr(const Vector3& ZeroValue) const noexcept
                    {
                        const KernelTypes::Float LengthSquared = LengthSq();
                        return LengthSquared > 1.17549435e-38f
                            ? *this / MathScalar::Sqrt(LengthSquared)
                            : ZeroValue;
                    }

                    UVector4 Vector3::ToInt() const noexcept
                    {
                        const auto Convert = [](KernelTypes::Float Value) noexcept
                        {
                            if (!(Value >= -2147483648.0f && Value < 2147483648.0f))
                                return 0x80000000u;
                            return KernelCompiler::BitCast<KernelTypes::UInt32>(
                                static_cast<KernelTypes::Int32>(Value)
                            );
                        };
                        const KernelTypes::UInt32 X = Convert(mX);
                        const KernelTypes::UInt32 Y = Convert(mY);
                        const KernelTypes::UInt32 Z = Convert(mZ);
                        return {X, Y, Z, Z};
                    }

                    UVector4 Vector3::ReinterpretAsInt() const noexcept
                    {
                        const KernelTypes::UInt32 X = KernelCompiler::BitCast<KernelTypes::UInt32>(mX);
                        const KernelTypes::UInt32 Y = KernelCompiler::BitCast<KernelTypes::UInt32>(mY);
                        const KernelTypes::UInt32 Z = KernelCompiler::BitCast<KernelTypes::UInt32>(mZ);
                        return {X, Y, Z, Z};
                    }

                    KernelTypes::Float Vector3::ReduceMin() const noexcept
                    {
                        return MathScalar::Min(mX, MathScalar::Min(mY, mZ));
                    }

                    KernelTypes::Float Vector3::ReduceMax() const noexcept
                    {
                        return MathScalar::Max(mX, MathScalar::Max(mY, mZ));
                    }

                    KernelTypes::Float Vector3::ReduceSum() const noexcept
                    {
                        return mX + mY + mZ;
                    }

                    Vector3 Vector3::Sqrt() const noexcept
                    {
                        return {MathScalar::Sqrt(mX), MathScalar::Sqrt(mY), MathScalar::Sqrt(mZ)};
                    }

                    Vector3 Vector3::GetNormalizedPerpendicular() const noexcept
                    {
                        const KernelTypes::Float XX = mX * mX;
                        const KernelTypes::Float YY = mY * mY;
                        const KernelTypes::Float ZZ = mZ * mZ;
                        const Vector3 Perpendicular = XX > YY ? Vector3(mZ, 0.0f, -mX) : Vector3(0.0f, mZ, -mY);
                        return Perpendicular / MathScalar::Sqrt(MathScalar::Max(XX, YY) + ZZ);
                    }

                    Vector3 Vector3::GetSign() const noexcept
                    {
                        return {
                            (ReinterpretAsInt()[0] & 0x80000000u) != 0 ? -1.0f : 1.0f,
                            (ReinterpretAsInt()[1] & 0x80000000u) != 0 ? -1.0f : 1.0f,
                            (ReinterpretAsInt()[2] & 0x80000000u) != 0 ? -1.0f : 1.0f
                        };
                    }

                    KernelTypes::UInt32 Vector3::CompressUnitVector() const noexcept
                    {
                        constexpr KernelTypes::UInt32 NumBits = 14;
                        constexpr KernelTypes::UInt32 Mask = (1u << NumBits) - 1;
                        constexpr KernelTypes::UInt32 MaxValue = Mask - 1;
                        constexpr KernelTypes::Float OneOverSqrt2 = 0.70710678f;
                        constexpr KernelTypes::Float Scale = static_cast<KernelTypes::Float>(MaxValue) / (2.0f * OneOverSqrt2);
                        Vector3 Value = *this;
                        const KernelTypes::UInt32 MaxIndex = static_cast<KernelTypes::UInt32>(Abs().GetHighestComponentIndex());
                        KernelTypes::UInt32 Result = 0;
                        if (Value[MaxIndex] < 0.0f)
                        {
                            Result = 0x80000000u;
                            Value = -Value;
                        }
                        Result |= MaxIndex << 29;
                        const Vector3 Quantized = sClamp(
                            (Value + sReplicate(OneOverSqrt2)) * Scale + sReplicate(0.5f),
                            sZero(),
                            sReplicate(static_cast<KernelTypes::Float>(MaxValue))
                        );
                        const UVector4 Compressed = Quantized.ToInt();
                        const auto ToPackedInt = [](KernelTypes::Float Component) noexcept
                        {
                            const KernelTypes::Float Clamped = MathScalar::Clamp(Component, 0.0f, 16382.0f);
                            return static_cast<KernelTypes::UInt32>(Clamped);
                        };
                        const KernelTypes::UInt32 C0 = ToPackedInt(Compressed.GetX());
                        const KernelTypes::UInt32 C1 = ToPackedInt(Compressed.GetY());
                        const KernelTypes::UInt32 C2 = ToPackedInt(Compressed.GetZ());
                        const KernelTypes::UInt32 First = MaxIndex == 0 ? C1 : C0;
                        const KernelTypes::UInt32 Second = MaxIndex == 2 ? C1 : C2;
                        Result |= First | (Second << NumBits);
                        return Result;
                    }

                    Vector3 Vector3::sDecompressUnitVector(KernelTypes::UInt32 Value) noexcept
                    {
                        constexpr KernelTypes::UInt32 NumBits = 14;
                        constexpr KernelTypes::UInt32 Mask = (1u << NumBits) - 1;
                        constexpr KernelTypes::Int32 HalfMaxValue = static_cast<KernelTypes::Int32>((Mask - 1) >> 1);
                        constexpr KernelTypes::Float OneOverSqrt2 = 0.70710678f;
                        constexpr KernelTypes::Float Scale = 2.0f * OneOverSqrt2 / static_cast<KernelTypes::Float>(Mask - 1);
                        Vector3 Result(
                            static_cast<KernelTypes::Float>(static_cast<KernelTypes::Int32>(Value & Mask) - HalfMaxValue),
                            static_cast<KernelTypes::Float>(static_cast<KernelTypes::Int32>((Value >> NumBits) & Mask) - HalfMaxValue),
                            0.0f
                        );
                        Result *= Scale;
                        Result.SetZ(MathScalar::Sqrt(MathScalar::Max(1.0f - Result.LengthSq(), 0.0f)));
                        if ((Value & 0x80000000u) != 0)
                            Result = -Result;
                        switch ((Value >> 29) & 3)
                        {
                        case 0:
                            Result = Result.Swizzle<2, 0, 1>();
                            break;
                        case 1:
                            Result = Result.Swizzle<0, 2, 1>();
                            break;
                        default:
                            break;
                        }
                        return Result;
                    }

                    const KernelContainers::StaticArray<Vector3, 1026>& Vector3::sUnitSphere()
                    {
                        static const KernelContainers::StaticArray<Vector3, 1026> Vertices = []
                        {
                            KernelContainers::StaticArray<Vector3, 1026> Result;
                            const auto AddVertex = [&Result](const Vector3& Vertex)
                            {
                                for (const Vector3& Existing : Result)
                                    if (Existing.IsClose(Vertex, 1.0e-6f))
                                        return;
                                Result.PushBack(Vertex);
                            };
                            const auto CreateVertices = [&AddVertex](auto&& Self, const Vector3& Direction1,
                                const Vector3& Direction2, const Vector3& Direction3, KernelTypes::Int32 Level) -> void
                            {
                                const Vector3 Center1 = (Direction1 + Direction2).Normalized();
                                const Vector3 Center2 = (Direction2 + Direction3).Normalized();
                                const Vector3 Center3 = (Direction3 + Direction1).Normalized();
                                AddVertex(Center1);
                                AddVertex(Center2);
                                AddVertex(Center3);
                                if (Level > 0)
                                {
                                    const KernelTypes::Int32 NewLevel = Level - 1;
                                    Self(Self, Direction1, Center1, Center3, NewLevel);
                                    Self(Self, Center1, Center2, Center3, NewLevel);
                                    Self(Self, Center1, Direction2, Center2, NewLevel);
                                    Self(Self, Center3, Center2, Direction3, NewLevel);
                                }
                            };

                            const Vector3 X = Vector3::sAxisX();
                            const Vector3 Y = Vector3::sAxisY();
                            const Vector3 Z = Vector3::sAxisZ();
                            AddVertex(X);
                            AddVertex(-X);
                            AddVertex(Y);
                            AddVertex(-Y);
                            AddVertex(Z);
                            AddVertex(-Z);
                            CreateVertices(CreateVertices, X, Y, Z, 3);
                            CreateVertices(CreateVertices, -X, Y, Z, 3);
                            CreateVertices(CreateVertices, X, -Y, Z, 3);
                            CreateVertices(CreateVertices, -X, -Y, Z, 3);
                            CreateVertices(CreateVertices, X, Y, -Z, 3);
                            CreateVertices(CreateVertices, -X, Y, -Z, 3);
                            CreateVertices(CreateVertices, X, -Y, -Z, 3);
                            CreateVertices(CreateVertices, -X, -Y, -Z, 3);
                            return Result;
                        }();
                        return Vertices;
                    }
                }
            }
        }
    }
}
