#pragma once

#include "Empire.Engine/Engine.Kernel/Kernel.Assert/Assert.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Compiler/Compiler.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Types/Types.hpp"

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

                    struct alignas(KernelCompiler::VectorAlignment) BVector16
                    {
                        KernelTypes::UInt8 Byte0;
                        KernelTypes::UInt8 Byte1;
                        KernelTypes::UInt8 Byte2;
                        KernelTypes::UInt8 Byte3;
                        KernelTypes::UInt8 Byte4;
                        KernelTypes::UInt8 Byte5;
                        KernelTypes::UInt8 Byte6;
                        KernelTypes::UInt8 Byte7;
                        KernelTypes::UInt8 Byte8;
                        KernelTypes::UInt8 Byte9;
                        KernelTypes::UInt8 Byte10;
                        KernelTypes::UInt8 Byte11;
                        KernelTypes::UInt8 Byte12;
                        KernelTypes::UInt8 Byte13;
                        KernelTypes::UInt8 Byte14;
                        KernelTypes::UInt8 Byte15;

                        BVector16() noexcept = default;

                        constexpr BVector16(
                            KernelTypes::UInt8 B0, KernelTypes::UInt8 B1,
                            KernelTypes::UInt8 B2, KernelTypes::UInt8 B3,
                            KernelTypes::UInt8 B4, KernelTypes::UInt8 B5,
                            KernelTypes::UInt8 B6, KernelTypes::UInt8 B7,
                            KernelTypes::UInt8 B8, KernelTypes::UInt8 B9,
                            KernelTypes::UInt8 B10, KernelTypes::UInt8 B11,
                            KernelTypes::UInt8 B12, KernelTypes::UInt8 B13,
                            KernelTypes::UInt8 B14, KernelTypes::UInt8 B15
                        ) noexcept
                            : Byte0(B0), Byte1(B1), Byte2(B2), Byte3(B3),
                              Byte4(B4), Byte5(B5), Byte6(B6), Byte7(B7),
                              Byte8(B8), Byte9(B9), Byte10(B10), Byte11(B11),
                              Byte12(B12), Byte13(B13), Byte14(B14), Byte15(B15)
                        {
                        }

                        constexpr BVector16(KernelTypes::UInt64 V0, KernelTypes::UInt64 V1) noexcept
                            : BVector16(
                                static_cast<KernelTypes::UInt8>(V0),
                                static_cast<KernelTypes::UInt8>(V0 >> 8),
                                static_cast<KernelTypes::UInt8>(V0 >> 16),
                                static_cast<KernelTypes::UInt8>(V0 >> 24),
                                static_cast<KernelTypes::UInt8>(V0 >> 32),
                                static_cast<KernelTypes::UInt8>(V0 >> 40),
                                static_cast<KernelTypes::UInt8>(V0 >> 48),
                                static_cast<KernelTypes::UInt8>(V0 >> 56),
                                static_cast<KernelTypes::UInt8>(V1),
                                static_cast<KernelTypes::UInt8>(V1 >> 8),
                                static_cast<KernelTypes::UInt8>(V1 >> 16),
                                static_cast<KernelTypes::UInt8>(V1 >> 24),
                                static_cast<KernelTypes::UInt8>(V1 >> 32),
                                static_cast<KernelTypes::UInt8>(V1 >> 40),
                                static_cast<KernelTypes::UInt8>(V1 >> 48),
                                static_cast<KernelTypes::UInt8>(V1 >> 56)
                            )
                        {
                        }

                        [[nodiscard]] static constexpr BVector16 sZero() noexcept
                        {
                            return BVector16(0, 0);
                        }

                        [[nodiscard]] static constexpr BVector16 sReplicate(KernelTypes::UInt8 Value) noexcept
                        {
                            BVector16 Result;
                            for (KernelTypes::UInt32 Index = 0; Index < 16; ++Index)
                                Result[Index] = Value;
                            return Result;
                        }

                        [[nodiscard]] static BVector16 sLoadByte16(const KernelTypes::UInt8* Values) noexcept
                        {
                            BVector16 Result;
                            for (KernelTypes::UInt32 Index = 0; Index < 16; ++Index)
                                Result[Index] = Values[Index];
                            return Result;
                        }

                        [[nodiscard]] static constexpr BVector16 sEquals(const BVector16& A, const BVector16& B) noexcept
                        {
                            BVector16 Result;
                            for (KernelTypes::UInt32 Index = 0; Index < 16; ++Index)
                                Result[Index] = A[Index] == B[Index] ? 0xff : 0;
                            return Result;
                        }

                        [[nodiscard]] static constexpr BVector16 sOr(const BVector16& A, const BVector16& B) noexcept
                        {
                            BVector16 Result;
                            for (KernelTypes::UInt32 Index = 0; Index < 16; ++Index)
                                Result[Index] = A[Index] | B[Index];
                            return Result;
                        }

                        [[nodiscard]] static constexpr BVector16 sXor(const BVector16& A, const BVector16& B) noexcept
                        {
                            BVector16 Result;
                            for (KernelTypes::UInt32 Index = 0; Index < 16; ++Index)
                                Result[Index] = A[Index] ^ B[Index];
                            return Result;
                        }

                        [[nodiscard]] static constexpr BVector16 sAnd(const BVector16& A, const BVector16& B) noexcept
                        {
                            BVector16 Result;
                            for (KernelTypes::UInt32 Index = 0; Index < 16; ++Index)
                                Result[Index] = A[Index] & B[Index];
                            return Result;
                        }

                        [[nodiscard]] static constexpr BVector16 sNot(const BVector16& Value) noexcept
                        {
                            BVector16 Result;
                            for (KernelTypes::UInt32 Index = 0; Index < 16; ++Index)
                                Result[Index] = static_cast<KernelTypes::UInt8>(~Value[Index]);
                            return Result;
                        }

                        [[nodiscard]] constexpr KernelTypes::UInt8 operator[](KernelTypes::UInt32 Index) const noexcept
                        {
                            KernelAssert::DebugCheck(Index < 16, "Index < 16", "BVector16 index out of range");
                            switch (Index)
                            {
                                case 0: return Byte0;
                                case 1: return Byte1;
                                case 2: return Byte2;
                                case 3: return Byte3;
                                case 4: return Byte4;
                                case 5: return Byte5;
                                case 6: return Byte6;
                                case 7: return Byte7;
                                case 8: return Byte8;
                                case 9: return Byte9;
                                case 10: return Byte10;
                                case 11: return Byte11;
                                case 12: return Byte12;
                                case 13: return Byte13;
                                case 14: return Byte14;
                                case 15: return Byte15;
                                default: return Byte0;
                            }
                        }

                        constexpr KernelTypes::UInt8& operator[](KernelTypes::UInt32 Index) noexcept
                        {
                            KernelAssert::DebugCheck(Index < 16, "Index < 16", "BVector16 index out of range");
                            switch (Index)
                            {
                                case 0: return Byte0;
                                case 1: return Byte1;
                                case 2: return Byte2;
                                case 3: return Byte3;
                                case 4: return Byte4;
                                case 5: return Byte5;
                                case 6: return Byte6;
                                case 7: return Byte7;
                                case 8: return Byte8;
                                case 9: return Byte9;
                                case 10: return Byte10;
                                case 11: return Byte11;
                                case 12: return Byte12;
                                case 13: return Byte13;
                                case 14: return Byte14;
                                case 15: return Byte15;
                                default: return Byte0;
                            }
                        }

                        [[nodiscard]] constexpr KernelTypes::Bool operator==(const BVector16& Other) const noexcept
                        {
                            for (KernelTypes::UInt32 Index = 0; Index < 16; ++Index)
                                if ((*this)[Index] != Other[Index])
                                    return false;
                            return true;
                        }

                        [[nodiscard]] constexpr KernelTypes::Bool operator!=(const BVector16& Other) const noexcept
                        {
                            return !(*this == Other);
                        }

                        [[nodiscard]] constexpr KernelTypes::Bool TestAnyTrue() const noexcept
                        {
                            for (KernelTypes::UInt32 Index = 0; Index < 16; ++Index)
                                if (((*this)[Index] & 0x80) != 0)
                                    return true;
                            return false;
                        }

                        [[nodiscard]] constexpr KernelTypes::Bool TestAllTrue() const noexcept
                        {
                            for (KernelTypes::UInt32 Index = 0; Index < 16; ++Index)
                                if (((*this)[Index] & 0x80) == 0)
                                    return false;
                            return true;
                        }

                        [[nodiscard]] constexpr KernelTypes::Int32 GetTrues() const noexcept
                        {
                            KernelTypes::Int32 Result = 0;
                            for (KernelTypes::UInt32 Index = 0; Index < 16; ++Index)
                                Result |= static_cast<KernelTypes::Int32>(((*this)[Index] >> 7) << Index);
                            return Result;
                        }
                    };

                    static_assert(sizeof(BVector16) == 16);
                    static_assert(alignof(BVector16) == KernelCompiler::VectorAlignment);
                    static_assert(KernelCompiler::IsTriviallyCopyable<BVector16>);
                }
            }
        }
    }
}
