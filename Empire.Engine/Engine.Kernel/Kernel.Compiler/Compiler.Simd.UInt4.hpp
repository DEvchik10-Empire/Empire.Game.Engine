#pragma once

#include "Empire.Engine/Engine.Kernel/Kernel.Compiler/Compiler.hpp"

#if defined(__SSE2__) || defined(__x86_64__) || defined(_M_X64) || defined(_M_IX86_FP)
#include <immintrin.h>
#elif defined(__ARM_NEON) || defined(__ARM_NEON__) || defined(__aarch64__) || defined(_M_ARM64) || defined(_M_ARM64EC)
#if defined(_MSC_VER)
#include <arm64_neon.h>
#else
#include <arm_neon.h>
#endif
#endif

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireEngine
        {
            namespace EngineKernel
            {
                namespace KernelCompiler
                {
                    namespace Simd
                    {
                        class UInt4
                        {
                        public:
                            [[nodiscard]] static constexpr UInt4 Load(
                                unsigned int X, unsigned int Y, unsigned int Z, unsigned int W) noexcept
                            {
                                return UInt4(X, Y, Z, W);
                            }

                            [[nodiscard]] constexpr UInt4 Add(const UInt4& Other) const noexcept
                            {
#if defined(__SSE2__) || defined(__x86_64__) || defined(_M_X64) || defined(_M_IX86_FP)
                                if (!KernelCompiler::IsConstantEvaluated())
                                {
                                    const __m128i Result = _mm_add_epi32(LoadRegister(), Other.LoadRegister());
                                    return UInt4(Extract<0>(Result), Extract<1>(Result), Extract<2>(Result), Extract<3>(Result));
                                }
#elif defined(__ARM_NEON) || defined(__ARM_NEON__) || defined(__aarch64__) || defined(_M_ARM64) || defined(_M_ARM64EC)
                                if (!KernelCompiler::IsConstantEvaluated())
                                {
                                    const uint32x4_t Result = vaddq_u32(LoadRegister(), Other.LoadRegister());
                                    return UInt4(Extract<0>(Result), Extract<1>(Result), Extract<2>(Result), Extract<3>(Result));
                                }
#endif
                                return UInt4(X + Other.X, Y + Other.Y, Z + Other.Z, W + Other.W);
                            }

                            [[nodiscard]] constexpr UInt4 Subtract(const UInt4& Other) const noexcept
                            {
#if defined(__SSE2__) || defined(__x86_64__) || defined(_M_X64) || defined(_M_IX86_FP)
                                if (!KernelCompiler::IsConstantEvaluated())
                                {
                                    const __m128i Result = _mm_sub_epi32(LoadRegister(), Other.LoadRegister());
                                    return UInt4(Extract<0>(Result), Extract<1>(Result), Extract<2>(Result), Extract<3>(Result));
                                }
#elif defined(__ARM_NEON) || defined(__ARM_NEON__) || defined(__aarch64__) || defined(_M_ARM64) || defined(_M_ARM64EC)
                                if (!KernelCompiler::IsConstantEvaluated())
                                {
                                    const uint32x4_t Result = vsubq_u32(LoadRegister(), Other.LoadRegister());
                                    return UInt4(Extract<0>(Result), Extract<1>(Result), Extract<2>(Result), Extract<3>(Result));
                                }
#endif
                                return UInt4(X - Other.X, Y - Other.Y, Z - Other.Z, W - Other.W);
                            }

                            template <unsigned int Index>
                            [[nodiscard]] constexpr unsigned int Get() const noexcept
                            {
                                static_assert(Index < 4);
                                if constexpr (Index == 0)
                                    return X;
                                else if constexpr (Index == 1)
                                    return Y;
                                else if constexpr (Index == 2)
                                    return Z;
                                else
                                    return W;
                            }

                        private:
                            constexpr UInt4(unsigned int X, unsigned int Y, unsigned int Z, unsigned int W) noexcept
                                : X(X), Y(Y), Z(Z), W(W)
                            {
                            }

#if defined(__SSE2__) || defined(__x86_64__) || defined(_M_X64) || defined(_M_IX86_FP)
                            [[nodiscard]] __m128i LoadRegister() const noexcept
                            {
                                return _mm_setr_epi32(
                                    KernelCompiler::BitCast<int>(X),
                                    KernelCompiler::BitCast<int>(Y),
                                    KernelCompiler::BitCast<int>(Z),
                                    KernelCompiler::BitCast<int>(W)
                                );
                            }

                            template <unsigned int Index>
                            [[nodiscard]] static unsigned int Extract(__m128i Value) noexcept
                            {
                                const __m128i Shuffled = _mm_shuffle_epi32(Value, _MM_SHUFFLE(Index, Index, Index, Index));
                                return KernelCompiler::BitCast<unsigned int>(_mm_cvtsi128_si32(Shuffled));
                            }
#elif defined(__ARM_NEON) || defined(__ARM_NEON__) || defined(__aarch64__) || defined(_M_ARM64) || defined(_M_ARM64EC)
                            [[nodiscard]] uint32x4_t LoadRegister() const noexcept
                            {
                                uint32x4_t Value = vdupq_n_u32(X);
                                Value = vsetq_lane_u32(Y, Value, 1);
                                Value = vsetq_lane_u32(Z, Value, 2);
                                return vsetq_lane_u32(W, Value, 3);
                            }

                            template <unsigned int Index>
                            [[nodiscard]] static unsigned int Extract(uint32x4_t Value) noexcept
                            {
                                return vgetq_lane_u32(Value, Index);
                            }
#endif

                            unsigned int X;
                            unsigned int Y;
                            unsigned int Z;
                            unsigned int W;
                        };
                    }
                }
            }
        }
    }
}
