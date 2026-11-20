#pragma once

#include "Empire.Engine/Engine.Kernel/Kernel.Assert/Assert.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Compiler/Compiler.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Compiler/Compiler.Simd.UInt4.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Types/Types.hpp"
#include "Empire.Engine/Engine.Math/Math.Vector/Vector4.hpp"
#include "Empire.Engine/Engine.Math/Math.Vector/Swizzle.hpp"

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

                    struct alignas(KernelCompiler::VectorAlignment) UVector4
                    {
                        KernelTypes::UInt32 X;
                        KernelTypes::UInt32 Y;
                        KernelTypes::UInt32 Z;
                        KernelTypes::UInt32 W;

                        UVector4() noexcept = default;

                        constexpr UVector4(
                            KernelTypes::UInt32 X,
                            KernelTypes::UInt32 Y,
                            KernelTypes::UInt32 Z,
                            KernelTypes::UInt32 W
                        ) noexcept
                            : X(X), Y(Y), Z(Z), W(W)
                        {
                        }

                        [[nodiscard]] static constexpr UVector4 sZero() noexcept
                        {
                            return UVector4(0, 0, 0, 0);
                        }

                        [[nodiscard]] static constexpr UVector4 sReplicate(KernelTypes::UInt32 Value) noexcept
                        {
                            return UVector4(Value, Value, Value, Value);
                        }

                        [[nodiscard]] static UVector4 sLoadInt(const KernelTypes::UInt32* Values) noexcept
                        {
                            return UVector4(Values[0], 0, 0, 0);
                        }

                        [[nodiscard]] static UVector4 sLoadInt4(const KernelTypes::UInt32* Values) noexcept
                        {
                            return UVector4(Values[0], Values[1], Values[2], Values[3]);
                        }

                        [[nodiscard]] static UVector4 sLoadInt4Aligned(const KernelTypes::UInt32* Values) noexcept
                        {
                            KernelAssert::DebugCheck(
                                (reinterpret_cast<KernelTypes::UIntPtr>(Values) & 15u) == 0,
                                "Values is 16-byte aligned",
                                "UVector4 aligned load requires 16-byte alignment"
                            );
                            return sLoadInt4(Values);
                        }

                        template <KernelTypes::Int32 Scale>
                        [[nodiscard]] static UVector4 sGatherInt4(
                            const KernelTypes::UInt32* Base,
                            const UVector4& Offsets
                        ) noexcept
                        {
                            static_assert(Scale > 0);
                            const auto* BaseBytes = reinterpret_cast<const KernelTypes::UInt8*>(Base);
                            const auto LoadAtOffset = [BaseBytes](KernelTypes::UInt32 Offset) noexcept
                            {
                                const KernelTypes::UIntPtr ByteOffset =
                                    static_cast<KernelTypes::UIntPtr>(Offset) * static_cast<KernelTypes::UIntPtr>(Scale);
                                return KernelCompiler::LoadUnaligned<KernelTypes::UInt32>(BaseBytes + ByteOffset);
                            };
                            return UVector4(
                                LoadAtOffset(Offsets[0]),
                                LoadAtOffset(Offsets[1]),
                                LoadAtOffset(Offsets[2]),
                                LoadAtOffset(Offsets[3])
                            );
                        }

                        [[nodiscard]] static constexpr UVector4 sMin(const UVector4& A, const UVector4& B) noexcept
                        {
                            return UVector4(
                                A[0] < B[0] ? A[0] : B[0],
                                A[1] < B[1] ? A[1] : B[1],
                                A[2] < B[2] ? A[2] : B[2],
                                A[3] < B[3] ? A[3] : B[3]
                            );
                        }

                        [[nodiscard]] static constexpr UVector4 sMax(const UVector4& A, const UVector4& B) noexcept
                        {
                            return UVector4(
                                A[0] > B[0] ? A[0] : B[0],
                                A[1] > B[1] ? A[1] : B[1],
                                A[2] > B[2] ? A[2] : B[2],
                                A[3] > B[3] ? A[3] : B[3]
                            );
                        }

                        [[nodiscard]] static constexpr UVector4 sEquals(const UVector4& A, const UVector4& B) noexcept
                        {
                            return UVector4(
                                A[0] == B[0] ? 0xffffffffu : 0,
                                A[1] == B[1] ? 0xffffffffu : 0,
                                A[2] == B[2] ? 0xffffffffu : 0,
                                A[3] == B[3] ? 0xffffffffu : 0
                            );
                        }

                        [[nodiscard]] static constexpr UVector4 sSelect(
                            const UVector4& NotSet,
                            const UVector4& Set,
                            const UVector4& Control
                        ) noexcept
                        {
                            return UVector4(
                                (Control[0] & 0x80000000u) ? Set[0] : NotSet[0],
                                (Control[1] & 0x80000000u) ? Set[1] : NotSet[1],
                                (Control[2] & 0x80000000u) ? Set[2] : NotSet[2],
                                (Control[3] & 0x80000000u) ? Set[3] : NotSet[3]
                            );
                        }

                        [[nodiscard]] static constexpr UVector4 sOr(const UVector4& A, const UVector4& B) noexcept
                        {
                            return UVector4(A[0] | B[0], A[1] | B[1], A[2] | B[2], A[3] | B[3]);
                        }

                        [[nodiscard]] static constexpr UVector4 sXor(const UVector4& A, const UVector4& B) noexcept
                        {
                            return UVector4(A[0] ^ B[0], A[1] ^ B[1], A[2] ^ B[2], A[3] ^ B[3]);
                        }

                        [[nodiscard]] static constexpr UVector4 sAnd(const UVector4& A, const UVector4& B) noexcept
                        {
                            return UVector4(A[0] & B[0], A[1] & B[1], A[2] & B[2], A[3] & B[3]);
                        }

                        [[nodiscard]] static constexpr UVector4 sNot(const UVector4& Value) noexcept
                        {
                            return UVector4(~Value[0], ~Value[1], ~Value[2], ~Value[3]);
                        }

                        [[nodiscard]] static constexpr UVector4 sSort4True(
                            const UVector4& Values,
                            const UVector4& Indices
                        ) noexcept
                        {
                            UVector4 Result(Indices[3], Indices[3], Indices[3], Indices[3]);
                            KernelTypes::UInt32 Destination = 0;
                            for (KernelTypes::UInt32 Index = 0; Index < 4; ++Index)
                                if ((Values[Index] & 0x80000000u) != 0)
                                    Result[Destination++] = Indices[Index];
                            return Result;
                        }

                        [[nodiscard]] constexpr KernelTypes::UInt32 GetX() const noexcept { return X; }
                        [[nodiscard]] constexpr KernelTypes::UInt32 GetY() const noexcept { return Y; }
                        [[nodiscard]] constexpr KernelTypes::UInt32 GetZ() const noexcept { return Z; }
                        [[nodiscard]] constexpr KernelTypes::UInt32 GetW() const noexcept { return W; }
                        constexpr void SetX(KernelTypes::UInt32 Value) noexcept { X = Value; }
                        constexpr void SetY(KernelTypes::UInt32 Value) noexcept { Y = Value; }
                        constexpr void SetZ(KernelTypes::UInt32 Value) noexcept { Z = Value; }
                        constexpr void SetW(KernelTypes::UInt32 Value) noexcept { W = Value; }

                        [[nodiscard]] constexpr KernelTypes::UInt32 operator[](KernelTypes::UInt32 Index) const noexcept
                        {
                            KernelAssert::DebugCheck(Index < 4, "Index < 4", "UVector4 index out of range");
                            switch (Index)
                            {
                                case 0: return X;
                                case 1: return Y;
                                case 2: return Z;
                                case 3: return W;
                                default: return X;
                            }
                        }

                        constexpr KernelTypes::UInt32& operator[](KernelTypes::UInt32 Index) noexcept
                        {
                            KernelAssert::DebugCheck(Index < 4, "Index < 4", "UVector4 index out of range");
                            switch (Index)
                            {
                                case 0: return X;
                                case 1: return Y;
                                case 2: return Z;
                                case 3: return W;
                                default: return X;
                            }
                        }

                        [[nodiscard]] constexpr KernelTypes::Bool operator==(const UVector4& Other) const noexcept
                        {
                            return X == Other.X && Y == Other.Y && Z == Other.Z && W == Other.W;
                        }

                        [[nodiscard]] constexpr KernelTypes::Bool operator!=(const UVector4& Other) const noexcept
                        {
                            return !(*this == Other);
                        }

                        [[nodiscard]] constexpr UVector4 operator*(const UVector4& Other) const noexcept
                        {
                            return UVector4(X * Other[0], Y * Other[1], Z * Other[2], W * Other[3]);
                        }

                        [[nodiscard]] constexpr UVector4 operator+(const UVector4& Other) const noexcept
                        {
                            const KernelCompiler::Simd::UInt4 Result =
                                KernelCompiler::Simd::UInt4::Load(X, Y, Z, W)
                                    .Add(KernelCompiler::Simd::UInt4::Load(Other.X, Other.Y, Other.Z, Other.W));
                            return UVector4(Result.Get<0>(), Result.Get<1>(), Result.Get<2>(), Result.Get<3>());
                        }

                        constexpr UVector4& operator+=(const UVector4& Other) noexcept
                        {
                            for (KernelTypes::UInt32 Index = 0; Index < 4; ++Index)
                                (*this)[Index] += Other[Index];
                            return *this;
                        }

                        [[nodiscard]] constexpr UVector4 operator-(const UVector4& Other) const noexcept
                        {
                            const KernelCompiler::Simd::UInt4 Result =
                                KernelCompiler::Simd::UInt4::Load(X, Y, Z, W)
                                    .Subtract(KernelCompiler::Simd::UInt4::Load(Other.X, Other.Y, Other.Z, Other.W));
                            return UVector4(Result.Get<0>(), Result.Get<1>(), Result.Get<2>(), Result.Get<3>());
                        }

                        constexpr UVector4& operator-=(const UVector4& Other) noexcept
                        {
                            for (KernelTypes::UInt32 Index = 0; Index < 4; ++Index)
                                (*this)[Index] -= Other[Index];
                            return *this;
                        }

                        [[nodiscard]] constexpr UVector4 SplatX() const noexcept { return sReplicate(X); }
                        [[nodiscard]] constexpr UVector4 SplatY() const noexcept { return sReplicate(Y); }
                        [[nodiscard]] constexpr UVector4 SplatZ() const noexcept { return sReplicate(Z); }
                        [[nodiscard]] constexpr UVector4 SplatW() const noexcept { return sReplicate(W); }

                        [[nodiscard]] constexpr Vector4 ToFloat() const noexcept
                        {
                            return Vector4(
                                static_cast<KernelTypes::Float>(KernelCompiler::BitCast<KernelTypes::Int32>(X)),
                                static_cast<KernelTypes::Float>(KernelCompiler::BitCast<KernelTypes::Int32>(Y)),
                                static_cast<KernelTypes::Float>(KernelCompiler::BitCast<KernelTypes::Int32>(Z)),
                                static_cast<KernelTypes::Float>(KernelCompiler::BitCast<KernelTypes::Int32>(W))
                            );
                        }

                        [[nodiscard]] constexpr Vector4 ReinterpretAsFloat() const noexcept
                        {
                            return Vector4(
                                KernelCompiler::BitCast<KernelTypes::Float>(X),
                                KernelCompiler::BitCast<KernelTypes::Float>(Y),
                                KernelCompiler::BitCast<KernelTypes::Float>(Z),
                                KernelCompiler::BitCast<KernelTypes::Float>(W)
                            );
                        }

                        [[nodiscard]] constexpr UVector4 DotV(const UVector4& Other) const noexcept
                        {
                            return sReplicate(Dot(Other));
                        }

                        [[nodiscard]] constexpr KernelTypes::UInt32 Dot(const UVector4& Other) const noexcept
                        {
                            return X * Other[0] + Y * Other[1] + Z * Other[2] + W * Other[3];
                        }

                        constexpr void StoreInt4(KernelTypes::UInt32* Output) const noexcept
                        {
                            for (KernelTypes::UInt32 Index = 0; Index < 4; ++Index)
                                Output[Index] = (*this)[Index];
                        }

                        constexpr void StoreInt4Aligned(KernelTypes::UInt32* Output) const noexcept
                        {
                            KernelAssert::DebugCheck(
                                (reinterpret_cast<KernelTypes::UIntPtr>(Output) & 15u) == 0,
                                "Output is 16-byte aligned",
                                "UVector4 aligned store requires 16-byte alignment"
                            );
                            StoreInt4(Output);
                        }

                        [[nodiscard]] constexpr KernelTypes::Int32 GetTrues() const noexcept
                        {
                            return static_cast<KernelTypes::Int32>(
                                (X >> 31) | ((Y >> 31) << 1)
                                | ((Z >> 31) << 2) | ((W >> 31) << 3)
                            );
                        }

                        [[nodiscard]] constexpr KernelTypes::Int32 CountTrues() const noexcept
                        {
                            const KernelTypes::UInt32 Mask = static_cast<KernelTypes::UInt32>(GetTrues());
                            return static_cast<KernelTypes::Int32>(
                                (Mask & 1u) + ((Mask >> 1) & 1u) + ((Mask >> 2) & 1u) + ((Mask >> 3) & 1u)
                            );
                        }

                        [[nodiscard]] constexpr KernelTypes::Bool TestAnyTrue() const noexcept { return GetTrues() != 0; }
                        [[nodiscard]] constexpr KernelTypes::Bool TestAnyXYZTrue() const noexcept { return (GetTrues() & 7) != 0; }
                        [[nodiscard]] constexpr KernelTypes::Bool TestAllTrue() const noexcept { return GetTrues() == 15; }
                        [[nodiscard]] constexpr KernelTypes::Bool TestAllXYZTrue() const noexcept { return (GetTrues() & 7) == 7; }

                        template <KernelTypes::UInt32 Count>
                        [[nodiscard]] constexpr UVector4 LogicalShiftLeft() const noexcept
                        {
                            static_assert(Count <= 31, "UVector4 shift count must be at most 31");
                            return UVector4(X << Count, Y << Count, Z << Count, W << Count);
                        }

                        template <KernelTypes::UInt32 Count>
                        [[nodiscard]] constexpr UVector4 LogicalShiftRight() const noexcept
                        {
                            static_assert(Count <= 31, "UVector4 shift count must be at most 31");
                            return UVector4(X >> Count, Y >> Count, Z >> Count, W >> Count);
                        }

                        template <KernelTypes::UInt32 Count>
                        [[nodiscard]] constexpr UVector4 ArithmeticShiftRight() const noexcept
                        {
                            static_assert(Count <= 31, "UVector4 shift count must be at most 31");
                            constexpr KernelTypes::UInt32 FillMask = Count == 0 ? 0 : (0xffffffffu << (32 - Count));
                            auto Shift = [](KernelTypes::UInt32 Value) constexpr
                            {
                                return (Value >> Count) | ((Value & 0x80000000u) != 0 ? FillMask : 0);
                            };
                            return UVector4(Shift(X), Shift(Y), Shift(Z), Shift(W));
                        }

                        [[nodiscard]] constexpr UVector4 Expand4Uint16Lo() const noexcept
                        {
                            return UVector4(
                                X & 0xffffu, (X >> 16) & 0xffffu,
                                Y & 0xffffu, (Y >> 16) & 0xffffu
                            );
                        }

                        [[nodiscard]] constexpr UVector4 Expand4Uint16Hi() const noexcept
                        {
                            return UVector4(
                                Z & 0xffffu, (Z >> 16) & 0xffffu,
                                W & 0xffffu, (W >> 16) & 0xffffu
                            );
                        }

                        [[nodiscard]] constexpr UVector4 Expand4Byte0() const noexcept { return ExpandBytes(X); }
                        [[nodiscard]] constexpr UVector4 Expand4Byte4() const noexcept { return ExpandBytes(Y); }
                        [[nodiscard]] constexpr UVector4 Expand4Byte8() const noexcept { return ExpandBytes(Z); }
                        [[nodiscard]] constexpr UVector4 Expand4Byte12() const noexcept { return ExpandBytes(W); }

                        [[nodiscard]] constexpr UVector4 ShiftComponents4Minus(KernelTypes::Int32 Count) const noexcept
                        {
                            KernelAssert::DebugCheck(Count >= 0 && Count <= 4, "Count >= 0 && Count <= 4", "Invalid component shift count");
                            if (Count < 0 || Count > 4)
                                return sZero();
                            UVector4 Result = sZero();
                            for (KernelTypes::Int32 Index = 0; Index < Count; ++Index)
                                Result[Index] = (*this)[Index + 4 - Count];
                            return Result;
                        }

                        template <KernelTypes::UInt32 X, KernelTypes::UInt32 Y, KernelTypes::UInt32 Z, KernelTypes::UInt32 W>
                        [[nodiscard]] constexpr UVector4 Swizzle() const noexcept
                        {
                            static_assert(X < 4 && Y < 4 && Z < 4 && W < 4);
                            return UVector4((*this)[X], (*this)[Y], (*this)[Z], (*this)[W]);
                        }

                    private:
                        [[nodiscard]] static constexpr UVector4 ExpandBytes(KernelTypes::UInt32 Value) noexcept
                        {
                            return UVector4(
                                Value & 0xffu, (Value >> 8) & 0xffu,
                                (Value >> 16) & 0xffu, (Value >> 24) & 0xffu
                            );
                        }
                    };

                    static_assert(sizeof(UVector4) == 16);
                    static_assert(alignof(UVector4) == KernelCompiler::VectorAlignment);
                    static_assert(KernelCompiler::IsTriviallyCopyable<UVector4>);
                }
            }
        }
    }
}
