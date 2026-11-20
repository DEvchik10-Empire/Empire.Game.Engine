#include "Empire.Engine/Engine.Math/Math.Vector/Vector4.hpp"
#include "Empire.Engine/Engine.Math/Math.Vector/UVector4.hpp"
#include "Empire.Engine/Engine.Math/Math.Scalar/Scalar.hpp"

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

                    Vector4 Vector4::sNaN() noexcept
                    {
                        return sReplicate(KernelCompiler::BitCast<KernelTypes::Float>(0x7fc00000u));
                    }

                    Vector4 Vector4::sLoadFloat4(const Float4* Value) noexcept
                    {
                        return {Value->X, Value->Y, Value->Z, Value->W};
                    }

                    Vector4 Vector4::sLoadFloat4Aligned(const Float4* Value) noexcept
                    {
                        KernelAssert::DebugCheck(
                            (reinterpret_cast<KernelTypes::UIntPtr>(Value) & 15u) == 0,
                            "Value is 16-byte aligned",
                            "Vector4 aligned load requires 16-byte alignment"
                        );
                        return sLoadFloat4(Value);
                    }

                    Vector4 Vector4::sGatherFloat4Scaled(
                        const KernelTypes::Float* Base,
                        const UVector4& Offsets,
                        KernelTypes::Int32 Scale
                    ) noexcept
                    {
                        KernelAssert::DebugCheck(Scale > 0, "Scale > 0", "Vector4 gather scale must be positive");
                        if (Scale <= 0)
                            return sZero();
                        const auto* BaseBytes = reinterpret_cast<const KernelTypes::UInt8*>(Base);
                        const auto LoadAtOffset = [BaseBytes, Scale](KernelTypes::UInt32 Offset) noexcept
                        {
                            const KernelTypes::UIntPtr ByteOffset =
                                static_cast<KernelTypes::UIntPtr>(Offset) * static_cast<KernelTypes::UIntPtr>(Scale);
                            return KernelCompiler::LoadUnaligned<KernelTypes::Float>(BaseBytes + ByteOffset);
                        };
                        return {
                            LoadAtOffset(Offsets[0]),
                            LoadAtOffset(Offsets[1]),
                            LoadAtOffset(Offsets[2]),
                            LoadAtOffset(Offsets[3])
                        };
                    }

                    Vector4 Vector4::sMin(const Vector4& A, const Vector4& B) noexcept
                    {
                        return {A.X < B.X ? A.X : B.X, A.Y < B.Y ? A.Y : B.Y,
                                A.Z < B.Z ? A.Z : B.Z, A.W < B.W ? A.W : B.W};
                    }

                    Vector4 Vector4::sMax(const Vector4& A, const Vector4& B) noexcept
                    {
                        return {A.X > B.X ? A.X : B.X, A.Y > B.Y ? A.Y : B.Y,
                                A.Z > B.Z ? A.Z : B.Z, A.W > B.W ? A.W : B.W};
                    }

                    Vector4 Vector4::sClamp(const Vector4& Value, const Vector4& Minimum, const Vector4& Maximum) noexcept
                    {
                        return sMax(sMin(Value, Maximum), Minimum);
                    }

                    UVector4 Vector4::sEquals(const Vector4& A, const Vector4& B) noexcept
                    {
                        return {A.X == B.X ? 0xffffffffu : 0, A.Y == B.Y ? 0xffffffffu : 0,
                                A.Z == B.Z ? 0xffffffffu : 0, A.W == B.W ? 0xffffffffu : 0};
                    }

                    UVector4 Vector4::sLess(const Vector4& A, const Vector4& B) noexcept
                    {
                        return {A.X < B.X ? 0xffffffffu : 0, A.Y < B.Y ? 0xffffffffu : 0,
                                A.Z < B.Z ? 0xffffffffu : 0, A.W < B.W ? 0xffffffffu : 0};
                    }

                    UVector4 Vector4::sLessOrEqual(const Vector4& A, const Vector4& B) noexcept
                    {
                        return {A.X <= B.X ? 0xffffffffu : 0, A.Y <= B.Y ? 0xffffffffu : 0,
                                A.Z <= B.Z ? 0xffffffffu : 0, A.W <= B.W ? 0xffffffffu : 0};
                    }

                    UVector4 Vector4::sGreater(const Vector4& A, const Vector4& B) noexcept
                    {
                        return sLess(B, A);
                    }

                    UVector4 Vector4::sGreaterOrEqual(const Vector4& A, const Vector4& B) noexcept
                    {
                        return sLessOrEqual(B, A);
                    }

                    Vector4 Vector4::sFusedMultiplyAdd(const Vector4& A, const Vector4& B, const Vector4& Addend) noexcept
                    {
                        return {A.X * B.X + Addend.X, A.Y * B.Y + Addend.Y,
                                A.Z * B.Z + Addend.Z, A.W * B.W + Addend.W};
                    }

                    Vector4 Vector4::sSelect(const Vector4& NotSet, const Vector4& Set, const UVector4& Control) noexcept
                    {
                        return {
                            Control[0] & 0x80000000u ? Set.X : NotSet.X,
                            Control[1] & 0x80000000u ? Set.Y : NotSet.Y,
                            Control[2] & 0x80000000u ? Set.Z : NotSet.Z,
                            Control[3] & 0x80000000u ? Set.W : NotSet.W
                        };
                    }

                    Vector4 Vector4::sOr(const Vector4& A, const Vector4& B) noexcept
                    {
                        return UVector4::sOr(A.ReinterpretAsInt(), B.ReinterpretAsInt()).ReinterpretAsFloat();
                    }

                    Vector4 Vector4::sXor(const Vector4& A, const Vector4& B) noexcept
                    {
                        return UVector4::sXor(A.ReinterpretAsInt(), B.ReinterpretAsInt()).ReinterpretAsFloat();
                    }

                    Vector4 Vector4::sAnd(const Vector4& A, const Vector4& B) noexcept
                    {
                        return UVector4::sAnd(A.ReinterpretAsInt(), B.ReinterpretAsInt()).ReinterpretAsFloat();
                    }

                    void Vector4::sSort4(Vector4& Values, UVector4& Indices) noexcept
                    {
                        const auto CompareAndSwap = [&Values, &Indices](
                            KernelTypes::UInt32 Left, KernelTypes::UInt32 Right) noexcept
                        {
                            if (Values[Left] > Values[Right])
                            {
                                const KernelTypes::Float Value = Values[Left];
                                Values[Left] = Values[Right];
                                Values[Right] = Value;
                                const KernelTypes::UInt32 Index = Indices[Left];
                                Indices[Left] = Indices[Right];
                                Indices[Right] = Index;
                            }
                        };
                        CompareAndSwap(0, 2);
                        CompareAndSwap(1, 3);
                        CompareAndSwap(0, 1);
                        CompareAndSwap(2, 3);
                        CompareAndSwap(1, 2);
                    }

                    void Vector4::sSort4Reverse(Vector4& Values, UVector4& Indices) noexcept
                    {
                        const auto CompareAndSwap = [&Values, &Indices](
                            KernelTypes::UInt32 Left, KernelTypes::UInt32 Right) noexcept
                        {
                            if (Values[Left] < Values[Right])
                            {
                                const KernelTypes::Float Value = Values[Left];
                                Values[Left] = Values[Right];
                                Values[Right] = Value;
                                const KernelTypes::UInt32 Index = Indices[Left];
                                Indices[Left] = Indices[Right];
                                Indices[Right] = Index;
                            }
                        };
                        CompareAndSwap(0, 2);
                        CompareAndSwap(1, 3);
                        CompareAndSwap(0, 1);
                        CompareAndSwap(2, 3);
                        CompareAndSwap(1, 2);
                    }

                    KernelTypes::Bool Vector4::IsClose(const Vector4& Other, KernelTypes::Float MaxDistanceSquared) const noexcept
                    {
                        return (*this - Other).LengthSquared() <= MaxDistanceSquared;
                    }

                    KernelTypes::Bool Vector4::IsNearZero(KernelTypes::Float MaxDistanceSquared) const noexcept
                    {
                        return LengthSquared() <= MaxDistanceSquared;
                    }

                    KernelTypes::Bool Vector4::IsNormalized(KernelTypes::Float Tolerance) const noexcept
                    {
                        return MathScalar::Abs(LengthSquared() - 1.0f) <= Tolerance;
                    }

                    KernelTypes::Bool Vector4::IsNaN() const noexcept
                    {
                        return X != X || Y != Y || Z != Z || W != W;
                    }

                    KernelTypes::Float Vector4::Length() const noexcept
                    {
                        return MathScalar::Sqrt(LengthSquared());
                    }

                    KernelTypes::Float Vector4::LengthSquared() const noexcept
                    {
                        return (X * X + Y * Y) + (Z * Z + W * W);
                    }

                    Vector4 Vector4::Normalized() const noexcept
                    {
                        return *this / Length();
                    }

                    KernelTypes::Float Vector4::Distance(const Vector4& Other) const noexcept
                    {
                        return (*this - Other).Length();
                    }

                    KernelTypes::Float Vector4::DistanceSquared(const Vector4& Other) const noexcept
                    {
                        return (*this - Other).LengthSquared();
                    }

                    Vector4 Vector4::operator/(const Vector4& Other) const noexcept
                    {
                        return {X / Other.X, Y / Other.Y, Z / Other.Z, W / Other.W};
                    }

                    Vector4& Vector4::operator*=(const Vector4& Other) noexcept
                    {
                        X *= Other.X; Y *= Other.Y; Z *= Other.Z; W *= Other.W;
                        return *this;
                    }

                    Vector4& Vector4::operator/=(const Vector4& Other) noexcept
                    {
                        X /= Other.X; Y /= Other.Y; Z /= Other.Z; W /= Other.W;
                        return *this;
                    }

                    KernelTypes::Int32 Vector4::GetLowestComponentIndex() const noexcept
                    {
                        KernelTypes::Int32 Result = 0;
                        for (KernelTypes::Int32 Index = 1; Index < 4; ++Index)
                            if ((*this)[Index] < (*this)[Result])
                                Result = Index;
                        return Result;
                    }

                    KernelTypes::Int32 Vector4::GetHighestComponentIndex() const noexcept
                    {
                        KernelTypes::Int32 Result = 0;
                        for (KernelTypes::Int32 Index = 1; Index < 4; ++Index)
                            if ((*this)[Index] > (*this)[Result])
                                Result = Index;
                        return Result;
                    }

                    Vector4 Vector4::Abs() const noexcept
                    {
                        return {
                            KernelCompiler::BitCast<KernelTypes::Float>(KernelCompiler::BitCast<KernelTypes::UInt32>(X) & 0x7fffffffu),
                            KernelCompiler::BitCast<KernelTypes::Float>(KernelCompiler::BitCast<KernelTypes::UInt32>(Y) & 0x7fffffffu),
                            KernelCompiler::BitCast<KernelTypes::Float>(KernelCompiler::BitCast<KernelTypes::UInt32>(Z) & 0x7fffffffu),
                            KernelCompiler::BitCast<KernelTypes::Float>(KernelCompiler::BitCast<KernelTypes::UInt32>(W) & 0x7fffffffu)
                        };
                    }

                    Vector4 Vector4::Reciprocal() const noexcept
                    {
                        return {1.0f / X, 1.0f / Y, 1.0f / Z, 1.0f / W};
                    }

                    Vector4 Vector4::sDifferenceOfProducts(
                        const Vector4& A, const Vector4& B, const Vector4& C, const Vector4& D
                    ) noexcept
                    {
                        return {
                            MathScalar::DifferenceOfProducts(A.X, B.X, C.X, D.X),
                            MathScalar::DifferenceOfProducts(A.Y, B.Y, C.Y, D.Y),
                            MathScalar::DifferenceOfProducts(A.Z, B.Z, C.Z, D.Z),
                            MathScalar::DifferenceOfProducts(A.W, B.W, C.W, D.W)
                        };
                    }

                    Vector4 Vector4::DotV(const Vector4& Other) const noexcept
                    {
                        return sReplicate(Dot(Other));
                    }

                    UVector4 Vector4::ToInt() const noexcept
                    {
                        const auto Convert = [](KernelTypes::Float Value) noexcept
                        {
                            if (!(Value >= -2147483648.0f && Value < 2147483648.0f))
                                return 0x80000000u;
                            return KernelCompiler::BitCast<KernelTypes::UInt32>(
                                static_cast<KernelTypes::Int32>(Value)
                            );
                        };
                        return {Convert(X), Convert(Y), Convert(Z), Convert(W)};
                    }

                    UVector4 Vector4::ReinterpretAsInt() const noexcept
                    {
                        return {
                            KernelCompiler::BitCast<KernelTypes::UInt32>(X),
                            KernelCompiler::BitCast<KernelTypes::UInt32>(Y),
                            KernelCompiler::BitCast<KernelTypes::UInt32>(Z),
                            KernelCompiler::BitCast<KernelTypes::UInt32>(W)
                        };
                    }

                    KernelTypes::Int32 Vector4::GetSignBits() const noexcept
                    {
                        const UVector4 Bits = ReinterpretAsInt();
                        return static_cast<KernelTypes::Int32>(
                            (Bits[0] >> 31) | ((Bits[1] >> 31) << 1)
                            | ((Bits[2] >> 31) << 2) | ((Bits[3] >> 31) << 3)
                        );
                    }

                    KernelTypes::Float Vector4::ReduceMin() const noexcept
                    {
                        return MathScalar::Min(MathScalar::Min(X, Y), MathScalar::Min(Z, W));
                    }

                    KernelTypes::Float Vector4::ReduceMax() const noexcept
                    {
                        return MathScalar::Max(MathScalar::Max(X, Y), MathScalar::Max(Z, W));
                    }

                    KernelTypes::Float Vector4::ReduceSum() const noexcept
                    {
                        return (X + Y) + (Z + W);
                    }

                    Vector4 Vector4::Sqrt() const noexcept
                    {
                        return {MathScalar::Sqrt(X), MathScalar::Sqrt(Y), MathScalar::Sqrt(Z), MathScalar::Sqrt(W)};
                    }

                    Vector4 Vector4::GetSign() const noexcept
                    {
                        const KernelTypes::UInt32 SignBits = static_cast<KernelTypes::UInt32>(GetSignBits());
                        return {
                            (SignBits & 1u) != 0 ? -1.0f : 1.0f,
                            (SignBits & 2u) != 0 ? -1.0f : 1.0f,
                            (SignBits & 4u) != 0 ? -1.0f : 1.0f,
                            (SignBits & 8u) != 0 ? -1.0f : 1.0f
                        };
                    }

                    void Vector4::SinCos(Vector4& Sine, Vector4& Cosine) const noexcept
                    {
                        UVector4 SineSign = UVector4::sAnd(ReinterpretAsInt(), UVector4::sReplicate(0x80000000u));
                        const Vector4 X = sXor(*this, SineSign.ReinterpretAsFloat());
                        const UVector4 Quadrant = (0.6366197723675814f * X + sReplicate(0.5f)).ToInt();
                        const Vector4 FloatQuadrant = Quadrant.ToFloat();
                        const Vector4 Reduced = ((X - FloatQuadrant * 1.5703125f)
                            - FloatQuadrant * 0.0004837512969970703125f)
                            - FloatQuadrant * 7.549789948768648e-8f;
                        const Vector4 X2 = Reduced * Reduced;
                        const Vector4 TaylorCosine =
                            ((2.443315711809948e-5f * X2 - sReplicate(1.388731625493765e-3f)) * X2
                                + sReplicate(4.166664568298827e-2f)) * X2 * X2
                            - 0.5f * X2 + sOne();
                        const Vector4 TaylorSine =
                            ((-1.9515295891e-4f * X2 + sReplicate(8.3321608736e-3f)) * X2
                                - sReplicate(1.6666654611e-1f)) * X2 * Reduced + Reduced;

                        const UVector4 Bit1 = Quadrant.LogicalShiftLeft<31>();
                        const UVector4 Bit2 = UVector4::sAnd(
                            Quadrant.LogicalShiftLeft<30>(),
                            UVector4::sReplicate(0x80000000u)
                        );
                        const Vector4 S = sSelect(TaylorSine, TaylorCosine, Bit1);
                        const Vector4 C = sSelect(TaylorCosine, TaylorSine, Bit1);
                        SineSign = UVector4::sXor(SineSign, Bit2);
                        const UVector4 CosineSign = UVector4::sXor(Bit1, Bit2);
                        Sine = sXor(S, SineSign.ReinterpretAsFloat());
                        Cosine = sXor(C, CosineSign.ReinterpretAsFloat());
                    }

                    Vector4 Vector4::Tan() const noexcept
                    {
                        const UVector4 Sign = UVector4::sAnd(ReinterpretAsInt(), UVector4::sReplicate(0x80000000u));
                        const Vector4 X = sXor(*this, Sign.ReinterpretAsFloat());
                        const UVector4 Quadrant = (0.6366197723675814f * X + sReplicate(0.5f)).ToInt();
                        const Vector4 FloatQuadrant = Quadrant.ToFloat();
                        const Vector4 Reduced = ((X - FloatQuadrant * 1.5703125f)
                            - FloatQuadrant * 0.0004837512969970703125f)
                            - FloatQuadrant * 7.549789948768648e-8f;
                        const Vector4 X2 = Reduced * Reduced;
                        Vector4 Tangent =
                            (((((9.38540185543e-3f * X2 + sReplicate(3.11992232697e-3f)) * X2
                                + sReplicate(2.44301354525e-2f)) * X2 + sReplicate(5.34112807005e-2f)) * X2
                                + sReplicate(1.33387994085e-1f)) * X2 + sReplicate(3.33331568548e-1f))
                                * X2 * Reduced + Reduced;
                        const UVector4 Invert = Quadrant.LogicalShiftLeft<31>();
                        Tangent = sSelect(Tangent, sReplicate(-1.0f) / Tangent, Invert);
                        return sXor(Tangent, Sign.ReinterpretAsFloat());
                    }

                    Vector4 Vector4::ASin() const noexcept
                    {
                        const UVector4 Sign = UVector4::sAnd(ReinterpretAsInt(), UVector4::sReplicate(0x80000000u));
                        const Vector4 A = sMin(sXor(*this, Sign.ReinterpretAsFloat()), sOne());
                        const Vector4 Z1 = A * A;
                        const Vector4 X1 = A;
                        const Vector4 Z2 = 0.5f * (sOne() - A);
                        const Vector4 X2 = Z2.Sqrt();
                        const UVector4 Greater = sGreater(A, sReplicate(0.5f));
                        Vector4 Z = sSelect(Z1, Z2, Greater);
                        const Vector4 X = sSelect(X1, X2, Greater);
                        Z = ((((4.2163199048e-2f * Z + sReplicate(2.4181311049e-2f)) * Z
                                + sReplicate(4.5470025998e-2f)) * Z + sReplicate(7.4953002686e-2f)) * Z
                                + sReplicate(1.6666752422e-1f)) * Z * X + X;
                        Z = sSelect(Z, sReplicate(0.5f * MathScalar::Pi) - (Z + Z), Greater);
                        return sXor(Z, Sign.ReinterpretAsFloat());
                    }

                    Vector4 Vector4::ACos() const noexcept
                    {
                        return sReplicate(0.5f * MathScalar::Pi) - ASin();
                    }

                    Vector4 Vector4::ATan() const noexcept
                    {
                        const UVector4 Sign = UVector4::sAnd(ReinterpretAsInt(), UVector4::sReplicate(0x80000000u));
                        Vector4 X = sXor(*this, Sign.ReinterpretAsFloat());
                        Vector4 Y = sZero();
                        const UVector4 Greater1 = sGreater(X, sReplicate(0.4142135623730950f));
                        const Vector4 X1 = (X - sOne()) / (X + sOne());
                        const UVector4 Greater2 = sGreater(X, sReplicate(2.414213562373095f));
                        const Vector4 X2 = sReplicate(-1.0f) / X;
                        X = sSelect(X, X1, Greater1);
                        Y = sSelect(Y, sReplicate(0.25f * MathScalar::Pi), Greater1);
                        X = sSelect(X, X2, Greater2);
                        Y = sSelect(Y, sReplicate(0.5f * MathScalar::Pi), Greater2);
                        const Vector4 Z = X * X;
                        Y += (((8.05374449538e-2f * Z - sReplicate(1.38776856032e-1f)) * Z
                                + sReplicate(1.99777106478e-1f)) * Z - sReplicate(3.33329491539e-1f))
                                * Z * X + X;
                        return sXor(Y, Sign.ReinterpretAsFloat());
                    }

                    Vector4 Vector4::sATan2(const Vector4& Y, const Vector4& X) noexcept
                    {
                        const UVector4 SignMask = UVector4::sReplicate(0x80000000u);
                        const UVector4 YSign = UVector4::sAnd(Y.ReinterpretAsInt(), SignMask);
                        const Vector4 YAbs = sXor(Y, YSign.ReinterpretAsFloat());
                        const UVector4 XSign = UVector4::sAnd(X.ReinterpretAsInt(), SignMask);
                        const Vector4 XAbs = sXor(X, XSign.ReinterpretAsFloat());
                        const UVector4 XIsNumerator = sLess(XAbs, YAbs);
                        const Vector4 Numerator = sSelect(YAbs, XAbs, XIsNumerator);
                        const Vector4 Denominator = sSelect(XAbs, YAbs, XIsNumerator);
                        Vector4 Angle = (Numerator / Denominator).ATan();
                        Angle = sSelect(Angle, sReplicate(0.5f * MathScalar::Pi) - Angle, XIsNumerator);
                        Angle -= sAnd(XSign.ArithmeticShiftRight<31>().ReinterpretAsFloat(), sReplicate(MathScalar::Pi));
                        return sXor(Angle, UVector4::sXor(XSign, YSign).ReinterpretAsFloat());
                    }

                    KernelTypes::UInt32 Vector4::CompressUnitVector() const noexcept
                    {
                        constexpr KernelTypes::Float OneOverSqrt2 = 0.70710678f;
                        constexpr KernelTypes::UInt32 ComponentBits = 9;
                        constexpr KernelTypes::UInt32 ComponentMask = (1u << ComponentBits) - 1u;
                        constexpr KernelTypes::UInt32 MaxValue = ComponentMask - 1u;
                        constexpr KernelTypes::Float Scale = static_cast<KernelTypes::Float>(MaxValue) / (2.0f * OneOverSqrt2);

                        Vector4 Value = *this;
                        const KernelTypes::UInt32 MaxComponent = static_cast<KernelTypes::UInt32>(Abs().GetHighestComponentIndex());
                        KernelTypes::UInt32 Result = 0;
                        if ((*this)[MaxComponent] < 0.0f)
                        {
                            Result = 0x80000000u;
                            Value = -Value;
                        }
                        Result |= MaxComponent << 29;

                        const UVector4 Quantized = sClamp(
                            (Value + sReplicate(OneOverSqrt2)) * Scale + sReplicate(0.5f),
                            sZero(),
                            sReplicate(static_cast<KernelTypes::Float>(MaxValue))
                        ).ToInt();

                        KernelTypes::UInt32 Component0 = 0;
                        KernelTypes::UInt32 Component1 = 0;
                        KernelTypes::UInt32 Component2 = 0;
                        switch (MaxComponent)
                        {
                            case 0:
                                Component0 = Quantized[1];
                                Component1 = Quantized[2];
                                Component2 = Quantized[3];
                                break;
                            case 1:
                                Component0 = Quantized[0];
                                Component1 = Quantized[2];
                                Component2 = Quantized[3];
                                break;
                            case 2:
                                Component0 = Quantized[0];
                                Component1 = Quantized[1];
                                Component2 = Quantized[3];
                                break;
                            default:
                                Component0 = Quantized[0];
                                Component1 = Quantized[1];
                                Component2 = Quantized[2];
                                break;
                        }

                        Result |= Component0 | (Component1 << ComponentBits) | (Component2 << (2u * ComponentBits));
                        return Result;
                    }

                    Vector4 Vector4::sDecompressUnitVector(KernelTypes::UInt32 Value) noexcept
                    {
                        constexpr KernelTypes::Float OneOverSqrt2 = 0.70710678f;
                        constexpr KernelTypes::UInt32 ComponentBits = 9;
                        constexpr KernelTypes::UInt32 ComponentMask = (1u << ComponentBits) - 1u;
                        constexpr KernelTypes::Int32 HalfMaxValue = static_cast<KernelTypes::Int32>((ComponentMask - 1u) >> 1u);
                        constexpr KernelTypes::Float Scale = 2.0f * OneOverSqrt2 / static_cast<KernelTypes::Float>(ComponentMask - 1u);

                        Vector4 Result(
                            static_cast<KernelTypes::Float>(static_cast<KernelTypes::Int32>(Value & ComponentMask) - HalfMaxValue) * Scale,
                            static_cast<KernelTypes::Float>(static_cast<KernelTypes::Int32>((Value >> ComponentBits) & ComponentMask) - HalfMaxValue) * Scale,
                            static_cast<KernelTypes::Float>(static_cast<KernelTypes::Int32>((Value >> (2u * ComponentBits)) & ComponentMask) - HalfMaxValue) * Scale,
                            0.0f
                        );
                        Result.SetW(MathScalar::Sqrt(MathScalar::Max(1.0f - Result.LengthSquared(), 0.0f)));
                        if ((Value & 0x80000000u) != 0)
                            Result = -Result;

                        switch ((Value >> 29u) & 3u)
                        {
                            case 0: return {Result.W, Result.X, Result.Y, Result.Z};
                            case 1: return {Result.X, Result.W, Result.Y, Result.Z};
                            case 2: return {Result.X, Result.Y, Result.W, Result.Z};
                            default: return Result;
                        }
                    }

                    void Vector4::StoreFloat4(Float4* Output) const noexcept
                    {
                        Output->X = X;
                        Output->Y = Y;
                        Output->Z = Z;
                        Output->W = W;
                    }
                }
            }
        }
    }
}
