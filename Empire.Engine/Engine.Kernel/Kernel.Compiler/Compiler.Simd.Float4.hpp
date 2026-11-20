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
                        class Float4
                        {
                        public:
                            [[nodiscard]] static constexpr Float4 Load(
                                float X, float Y, float Z, float W) noexcept
                            {
                                return Float4(X, Y, Z, W);
                            }

                            [[nodiscard]] constexpr Float4 Add(const Float4& Other) const noexcept
                            {
#if defined(__SSE2__) || defined(__x86_64__) || defined(_M_X64) || defined(_M_IX86_FP)
                                if (!KernelCompiler::IsConstantEvaluated())
                                {
                                    const __m128 Result = _mm_add_ps(LoadRegister(), Other.LoadRegister());
                                    return Float4(Extract<0>(Result), Extract<1>(Result), Extract<2>(Result), Extract<3>(Result));
                                }
#elif defined(__ARM_NEON) || defined(__ARM_NEON__) || defined(__aarch64__) || defined(_M_ARM64) || defined(_M_ARM64EC)
                                if (!KernelCompiler::IsConstantEvaluated())
                                {
                                    const float32x4_t Result = vaddq_f32(LoadRegister(), Other.LoadRegister());
                                    return Float4(Extract<0>(Result), Extract<1>(Result), Extract<2>(Result), Extract<3>(Result));
                                }
#endif
                                return Float4(X + Other.X, Y + Other.Y, Z + Other.Z, W + Other.W);
                            }

                            [[nodiscard]] constexpr Float4 Subtract(const Float4& Other) const noexcept
                            {
#if defined(__SSE2__) || defined(__x86_64__) || defined(_M_X64) || defined(_M_IX86_FP)
                                if (!KernelCompiler::IsConstantEvaluated())
                                {
                                    const __m128 Result = _mm_sub_ps(LoadRegister(), Other.LoadRegister());
                                    return Float4(Extract<0>(Result), Extract<1>(Result), Extract<2>(Result), Extract<3>(Result));
                                }
#elif defined(__ARM_NEON) || defined(__ARM_NEON__) || defined(__aarch64__) || defined(_M_ARM64) || defined(_M_ARM64EC)
                                if (!KernelCompiler::IsConstantEvaluated())
                                {
                                    const float32x4_t Result = vsubq_f32(LoadRegister(), Other.LoadRegister());
                                    return Float4(Extract<0>(Result), Extract<1>(Result), Extract<2>(Result), Extract<3>(Result));
                                }
#endif
                                return Float4(X - Other.X, Y - Other.Y, Z - Other.Z, W - Other.W);
                            }

                            [[nodiscard]] constexpr Float4 Multiply(const Float4& Other) const noexcept
                            {
#if defined(__SSE2__) || defined(__x86_64__) || defined(_M_X64) || defined(_M_IX86_FP)
                                if (!KernelCompiler::IsConstantEvaluated())
                                {
                                    const __m128 Result = _mm_mul_ps(LoadRegister(), Other.LoadRegister());
                                    return Float4(Extract<0>(Result), Extract<1>(Result), Extract<2>(Result), Extract<3>(Result));
                                }
#elif defined(__ARM_NEON) || defined(__ARM_NEON__) || defined(__aarch64__) || defined(_M_ARM64) || defined(_M_ARM64EC)
                                if (!KernelCompiler::IsConstantEvaluated())
                                {
                                    const float32x4_t Result = vmulq_f32(LoadRegister(), Other.LoadRegister());
                                    return Float4(Extract<0>(Result), Extract<1>(Result), Extract<2>(Result), Extract<3>(Result));
                                }
#endif
                                return Float4(X * Other.X, Y * Other.Y, Z * Other.Z, W * Other.W);
                            }

                            template <unsigned int Index>
                            [[nodiscard]] constexpr float Get() const noexcept
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

                            [[nodiscard]] constexpr float ReduceSum() const noexcept
                            {
#if defined(__SSE2__) || defined(__x86_64__) || defined(_M_X64) || defined(_M_IX86_FP)
                                if (!KernelCompiler::IsConstantEvaluated())
                                {
                                    const __m128 Value = LoadRegister();
                                    const __m128 PairSums = _mm_add_ps(
                                        Value,
                                        _mm_shuffle_ps(Value, Value, _MM_SHUFFLE(2, 3, 0, 1))
                                    );
                                    return _mm_cvtss_f32(_mm_add_ss(PairSums, _mm_movehl_ps(PairSums, PairSums)));
                                }
#elif defined(__ARM_NEON) || defined(__ARM_NEON__) || defined(__aarch64__) || defined(_M_ARM64) || defined(_M_ARM64EC)
                                if (!KernelCompiler::IsConstantEvaluated())
                                {
                                    const float32x4_t Value = LoadRegister();
                                    const float32x2_t PairSums = vpadd_f32(vget_low_f32(Value), vget_high_f32(Value));
                                    return vget_lane_f32(vpadd_f32(PairSums, PairSums), 0);
                                }
#endif
                                return (X + Y) + (Z + W);
                            }

                        private:
                            constexpr Float4(float X, float Y, float Z, float W) noexcept
                                : X(X), Y(Y), Z(Z), W(W)
                            {
                            }

#if defined(__SSE2__) || defined(__x86_64__) || defined(_M_X64) || defined(_M_IX86_FP)
                            [[nodiscard]] __m128 LoadRegister() const noexcept
                            {
                                return _mm_setr_ps(X, Y, Z, W);
                            }

                            template <unsigned int Index>
                            [[nodiscard]] static float Extract(__m128 Value) noexcept
                            {
                                return _mm_cvtss_f32(_mm_shuffle_ps(Value, Value, _MM_SHUFFLE(Index, Index, Index, Index)));
                            }
#elif defined(__ARM_NEON) || defined(__ARM_NEON__) || defined(__aarch64__) || defined(_M_ARM64) || defined(_M_ARM64EC)
                            [[nodiscard]] float32x4_t LoadRegister() const noexcept
                            {
                                float32x4_t Value = vdupq_n_f32(X);
                                Value = vsetq_lane_f32(Y, Value, 1);
                                Value = vsetq_lane_f32(Z, Value, 2);
                                return vsetq_lane_f32(W, Value, 3);
                            }

                            template <unsigned int Index>
                            [[nodiscard]] static float Extract(float32x4_t Value) noexcept
                            {
                                return vgetq_lane_f32(Value, Index);
                            }
#endif

                            float X;
                            float Y;
                            float Z;
                            float W;
                        };
                    }
                }
            }
        }
    }
}
