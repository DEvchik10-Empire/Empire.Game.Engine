#pragma once

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
                namespace MathScalar
                {
                    using namespace EngineKernel;

                    using HalfFloat = KernelTypes::UInt16;

                    inline constexpr HalfFloat HalfFloatMaximum = 0x7bff;
                    inline constexpr HalfFloat HalfFloatMaximumNegative = 0xfbff;
                    inline constexpr HalfFloat HalfFloatInfinity = 0x7c00;
                    inline constexpr HalfFloat HalfFloatInfinityNegative = 0xfc00;
                    inline constexpr HalfFloat HalfFloatQuietNaN = 0x7e00;
                    inline constexpr HalfFloat HalfFloatQuietNaNNegative = 0xfe00;

                    enum class HalfFloatRoundingMode : KernelTypes::UInt8
                    {
                        TowardNegativeInfinity,
                        TowardPositiveInfinity,
                        ToNearest
                    };

                    namespace HalfFloatConversion
                    {
                        template <HalfFloatRoundingMode RoundingMode>
                        [[nodiscard]] constexpr HalfFloat FromFloatFallback(KernelTypes::Float Value) noexcept
                        {
                            constexpr KernelTypes::Int32 FloatExponentBias = 127;
                            constexpr KernelTypes::Int32 HalfExponentBias = 15;
                            constexpr KernelTypes::Int32 FloatExponentMask = 0xff;
                            constexpr KernelTypes::Int32 HalfExponentMask = 0x1f;
                            constexpr KernelTypes::Int32 FloatMantissaBits = 23;
                            constexpr KernelTypes::Int32 HalfMantissaBits = 10;
                            constexpr KernelTypes::UInt32 FloatMantissaMask = (1u << FloatMantissaBits) - 1;
                            constexpr KernelTypes::UInt32 FloatExponentAndMantissaMask =
                                FloatMantissaMask | (static_cast<KernelTypes::UInt32>(FloatExponentMask) << FloatMantissaBits);

                            constexpr KernelTypes::UInt32 FloatBitsSignPosition = 31;
                            constexpr KernelTypes::UInt32 HalfBitsSignPosition = 15;
                            constexpr KernelTypes::UInt32 HalfExponentPosition = 10;

                            const KernelTypes::UInt32 Bits = KernelCompiler::BitCast<KernelTypes::UInt32>(Value);
                            const KernelTypes::UInt32 Exponent = (Bits >> FloatMantissaBits) & FloatExponentMask;
                            KernelTypes::UInt32 Mantissa = Bits & FloatMantissaMask;
                            const HalfFloat Sign = static_cast<HalfFloat>(
                                (Bits >> (FloatBitsSignPosition - HalfBitsSignPosition)) & (1u << HalfBitsSignPosition)
                            );

                            if (Exponent == FloatExponentMask)
                                return static_cast<HalfFloat>(Sign | (Mantissa == 0 ? HalfFloatInfinity : HalfFloatQuietNaN));

                            const KernelTypes::Int32 RebiasedExponent =
                                static_cast<KernelTypes::Int32>(Exponent) - FloatExponentBias + HalfExponentBias;

                            if (RebiasedExponent >= HalfExponentMask)
                            {
                                constexpr KernelTypes::Bool IsNearest = RoundingMode == HalfFloatRoundingMode::ToNearest;
                                constexpr KernelTypes::Bool RoundsTowardPositive =
                                    RoundingMode == HalfFloatRoundingMode::TowardPositiveInfinity;
                                const KernelTypes::Bool RoundToInfinity =
                                    IsNearest || ((Sign == 0) == RoundsTowardPositive);
                                return static_cast<HalfFloat>(Sign | (RoundToInfinity ? HalfFloatInfinity : HalfFloatMaximum));
                            }

                            if (RebiasedExponent < -HalfMantissaBits)
                            {
                                const KernelTypes::Bool RoundsAwayFromZero =
                                    (RoundingMode == HalfFloatRoundingMode::TowardNegativeInfinity
                                        && Sign != 0)
                                    || (RoundingMode == HalfFloatRoundingMode::TowardPositiveInfinity
                                        && Sign == 0);
                                const KernelTypes::Bool RoundToSmallest =
                                    RoundsAwayFromZero && (Bits & FloatExponentAndMantissaMask) != 0;
                                return static_cast<HalfFloat>(Sign | (RoundToSmallest ? 1 : 0));
                            }

                            KernelTypes::UInt32 HalfExponent = 0;
                            KernelTypes::Int32 Shift = 0;
                            if (RebiasedExponent <= 0)
                            {
                                Mantissa |= 1u << FloatMantissaBits;
                                Shift = FloatMantissaBits - HalfMantissaBits + 1 - RebiasedExponent;
                            }
                            else
                            {
                                HalfExponent = static_cast<KernelTypes::UInt32>(RebiasedExponent) << HalfExponentPosition;
                                Shift = FloatMantissaBits - HalfMantissaBits;
                            }

                            const HalfFloat HalfMantissa = static_cast<HalfFloat>(Mantissa >> Shift);
                            HalfFloat Result = static_cast<HalfFloat>(Sign | HalfExponent | HalfMantissa);
                            const KernelTypes::UInt32 Remainder = Mantissa & ((1u << Shift) - 1);

                            if constexpr (RoundingMode == HalfFloatRoundingMode::ToNearest)
                            {
                                const KernelTypes::UInt32 Threshold = 1u << (Shift - 1);
                                if (Remainder > Threshold || (Remainder == Threshold && (HalfMantissa & 1) != 0))
                                    ++Result;
                            }
                            else
                            {
                                constexpr KernelTypes::Bool RoundsTowardPositive =
                                    RoundingMode == HalfFloatRoundingMode::TowardPositiveInfinity;
                                const KernelTypes::Bool RoundUp = ((Sign == 0) == RoundsTowardPositive) && Remainder != 0;
                                if (RoundUp)
                                    ++Result;
                            }
                            return Result;
                        }

                        template <HalfFloatRoundingMode RoundingMode>
                        [[nodiscard]] constexpr HalfFloat FromFloat(KernelTypes::Float Value) noexcept
                        {
                            return FromFloatFallback<RoundingMode>(Value);
                        }

                        [[nodiscard]] constexpr KernelTypes::Float ToFloatFallback(HalfFloat Value) noexcept
                        {
                            constexpr KernelTypes::UInt32 HalfSignMask = 0x8000;
                            constexpr KernelTypes::UInt32 HalfExponentMask = 0x1f;
                            constexpr KernelTypes::UInt32 HalfMantissaMask = 0x03ff;
                            constexpr KernelTypes::UInt32 FloatExponentBiasDifference = 112;

                            const KernelTypes::UInt32 Sign =
                                (static_cast<KernelTypes::UInt32>(Value) & HalfSignMask) << 16;
                            KernelTypes::UInt32 Exponent = (Value >> 10) & HalfExponentMask;
                            KernelTypes::UInt32 Mantissa = Value & HalfMantissaMask;

                            if (Exponent == 0)
                            {
                                if (Mantissa == 0)
                                    return KernelCompiler::BitCast<KernelTypes::Float>(Sign);

                                KernelTypes::Int32 DenormalizedExponent = 1;
                                while ((Mantissa & 0x400u) == 0)
                                {
                                    Mantissa <<= 1;
                                    --DenormalizedExponent;
                                }
                                Mantissa &= HalfMantissaMask;
                                Exponent = static_cast<KernelTypes::UInt32>(DenormalizedExponent + FloatExponentBiasDifference);
                            }
                            else if (Exponent == HalfExponentMask)
                            {
                                Exponent = 0xff;
                            }
                            else
                            {
                                Exponent += static_cast<KernelTypes::UInt32>(FloatExponentBiasDifference);
                            }

                            const KernelTypes::UInt32 Bits =
                                Sign | (Exponent << 23) | (Mantissa << 13);
                            return KernelCompiler::BitCast<KernelTypes::Float>(Bits);
                        }

                        [[nodiscard]] constexpr KernelTypes::Float ToFloat(HalfFloat Value) noexcept
                        {
                            return ToFloatFallback(Value);
                        }
                    }
                }
            }
        }
    }
}
