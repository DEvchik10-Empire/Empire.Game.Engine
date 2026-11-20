#pragma once

#include "Empire.Engine/Engine.Kernel/Kernel.Compiler/Compiler.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Config/Config.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Assert/Assert.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Types/Types.hpp"
#include "Empire.Engine/Engine.Math/Math.Scalar/MathTypes.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireEngine
        {
            namespace EngineMath
            {
                namespace MathScalar
                {
                    using namespace EngineKernel;

                    inline constexpr KernelTypes::Float Pi = 3.14159265358979323846f;
                    inline constexpr KernelTypes::Float TwoPi = 6.28318530717958647692f;
                    inline constexpr KernelTypes::Float HalfPi = 1.57079632679489661923f;
                    inline constexpr KernelTypes::Float LargeFloat = 1.0e15f;

                    [[nodiscard]] constexpr KernelTypes::Float DegreesToRadians(KernelTypes::Float Degrees) noexcept
                    {
                        return Degrees * (Pi / 180.0f);
                    }

                    [[nodiscard]] constexpr KernelTypes::Float RadiansToDegrees(KernelTypes::Float Radians) noexcept
                    {
                        return Radians * (180.0f / Pi);
                    }

                    [[nodiscard]] inline KernelTypes::Float CenterAngleAroundZero(KernelTypes::Float Value) noexcept
                    {
                        if (Value < -Pi)
                        {
                            do
                            {
                                Value += TwoPi;
                            } while (Value < -Pi);
                        }
                        else if (Value > Pi)
                        {
                            do
                            {
                                Value -= TwoPi;
                            } while (Value > Pi);
                        }
                        return Value;
                    }

                    template <typename T>
                    [[nodiscard]] constexpr T Clamp(T Value, T Minimum, T Maximum) noexcept
                    {
                        const T LowerClamped = Value < Minimum ? Minimum : Value;
                        return Maximum < LowerClamped ? Maximum : LowerClamped;
                    }

                    template <typename T>
                    [[nodiscard]] constexpr T Min(T A, T B) noexcept
                    {
                        return B < A ? B : A;
                    }

                    template <typename T>
                    [[nodiscard]] constexpr T Max(T A, T B) noexcept
                    {
                        return A < B ? B : A;
                    }

                    [[nodiscard]] constexpr KernelTypes::Float Abs(KernelTypes::Float Value) noexcept
                    {
                        return KernelCompiler::BitCast<KernelTypes::Float>(
                            KernelCompiler::BitCast<KernelTypes::UInt32>(Value) & 0x7fffffffu
                        );
                    }

                    [[nodiscard]] inline KernelTypes::Float Sqrt(KernelTypes::Float Value) noexcept
                    {
                        return KernelCompiler::SquareRoot(Value);
                    }

                    [[nodiscard]] inline KernelTypes::Double Sqrt(KernelTypes::Double Value) noexcept
                    {
                        return KernelCompiler::SquareRoot(Value);
                    }

                    template <typename T>
                    [[nodiscard]] constexpr T Square(T Value) noexcept
                    {
                        return Value * Value;
                    }

                    template <typename T>
                    [[nodiscard]] constexpr T Cubed(T Value) noexcept
                    {
                        return Value * Value * Value;
                    }

                    template <typename T>
                    [[nodiscard]] constexpr T Sign(T Value) noexcept
                    {
                        return Value < static_cast<T>(0) ? static_cast<T>(-1) : static_cast<T>(1);
                    }

                    template <typename T>
                    [[nodiscard]] constexpr KernelTypes::Bool IsPowerOf2(T Value) noexcept
                    {
                        return Value > static_cast<T>(0)
                            && (Value & (Value - static_cast<T>(1))) == static_cast<T>(0);
                    }

                    template <typename T>
                    [[nodiscard]] constexpr T AlignUp(T Value, KernelTypes::UInt64 Alignment) noexcept
                    {
                        KernelAssert::DebugCheck(IsPowerOf2(Alignment), "Alignment is a power of two");
                        return static_cast<T>((static_cast<KernelTypes::UInt64>(Value) + Alignment - 1) & ~(Alignment - 1));
                    }

                    template <typename T>
                    [[nodiscard]] constexpr KernelTypes::Bool IsAligned(T Value, KernelTypes::UInt64 Alignment) noexcept
                    {
                        KernelAssert::DebugCheck(IsPowerOf2(Alignment), "Alignment is a power of two");
                        return (static_cast<KernelTypes::UInt64>(Value) & (Alignment - 1)) == 0;
                    }

                    [[nodiscard]] constexpr KernelTypes::UInt32 CountTrailingZeros(KernelTypes::UInt32 Value) noexcept
                    {
                        return KernelCompiler::CountTrailingZeros(Value);
                    }

                    [[nodiscard]] constexpr KernelTypes::UInt32 CountLeadingZeros(KernelTypes::UInt32 Value) noexcept
                    {
                        return KernelCompiler::CountLeadingZeros(Value);
                    }

                    [[nodiscard]] constexpr KernelTypes::UInt32 CountBits(KernelTypes::UInt32 Value) noexcept
                    {
                        Value = Value - ((Value >> 1) & 0x55555555u);
                        Value = (Value & 0x33333333u) + ((Value >> 2) & 0x33333333u);
                        Value = (Value + (Value >> 4)) & 0x0F0F0F0Fu;
                        return (Value * 0x01010101u) >> 24;
                    }

                    [[nodiscard]] constexpr KernelTypes::UInt32 GetNextPowerOf2(KernelTypes::UInt32 Value) noexcept
                    {
                        if (Value > 0x80000000u)
                            return 0;

                        return Value <= 1 ? 1u : 1u << (32u - CountLeadingZeros(Value - 1));
                    }

                    [[nodiscard]] inline KernelTypes::Float MulRSqrtApproximate(
                        KernelTypes::Float A,
                        KernelTypes::Float B
                    ) noexcept
                    {
                        KernelAssert::DebugCheck(B >= 1.17549435e-38f, "B >= smallest positive normal float");
                        return A / Sqrt(B);
                    }

                    [[nodiscard]] inline KernelTypes::Float DifferenceOfProducts(
                        KernelTypes::Float A,
                        KernelTypes::Float B,
                        KernelTypes::Float C,
                        KernelTypes::Float D
                    ) noexcept
                    {
                        return A * B - C * D;
                    }

                }
            }
        }
    }
}
