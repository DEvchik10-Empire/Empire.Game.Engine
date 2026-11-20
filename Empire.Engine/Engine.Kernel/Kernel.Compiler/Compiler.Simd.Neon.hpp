#pragma once

// ARM64 NEON compatibility helpers ported from Jolt 5.7.0 Core/ARMNeon.h.
// Intrinsics and architecture detection stay inside Kernel.Compiler.
#if defined(__aarch64__) || defined(_M_ARM64) || defined(_M_ARM64EC)

#if defined(_MSC_VER)
    #include <arm64_neon.h>
#else
    #include <arm_neon.h>
#endif
#include <stdint.h>

namespace DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineKernel::KernelCompiler::Neon
{
    inline int32x4_t MakeInt32x4(int32_t V1, int32_t V2, int32_t V3, int32_t V4) noexcept
    {
        const int32_t Values[4] = {V1, V2, V3, V4};
        return vld1q_s32(Values);
    }

    inline uint32x4_t MakeUInt32x4(uint32_t V1, uint32_t V2, uint32_t V3, uint32_t V4) noexcept
    {
        const uint32_t Values[4] = {V1, V2, V3, V4};
        return vld1q_u32(Values);
    }

    inline int8x16_t MakeInt8x16(
        int8_t V1, int8_t V2, int8_t V3, int8_t V4,
        int8_t V5, int8_t V6, int8_t V7, int8_t V8,
        int8_t V9, int8_t V10, int8_t V11, int8_t V12,
        int8_t V13, int8_t V14, int8_t V15, int8_t V16
    ) noexcept
    {
        const int8_t Values[16] = {V1, V2, V3, V4, V5, V6, V7, V8, V9, V10, V11, V12, V13, V14, V15, V16};
        return vld1q_s8(Values);
    }

    inline uint8x16_t MakeUInt8x16(
        uint8_t V1, uint8_t V2, uint8_t V3, uint8_t V4,
        uint8_t V5, uint8_t V6, uint8_t V7, uint8_t V8,
        uint8_t V9, uint8_t V10, uint8_t V11, uint8_t V12,
        uint8_t V13, uint8_t V14, uint8_t V15, uint8_t V16
    ) noexcept
    {
        const uint8_t Values[16] = {V1, V2, V3, V4, V5, V6, V7, V8, V9, V10, V11, V12, V13, V14, V15, V16};
        return vld1q_u8(Values);
    }

#if defined(_MSC_VER) || (defined(__GNUC__) && !defined(__clang__) && __GNUC__ < 12)
	// Generic shuffle vector template
	template <unsigned I1, unsigned I2, unsigned I3, unsigned I4>
	inline float32x4_t NeonShuffleFloat32x4([[maybe_unused]] float32x4_t inV1, [[maybe_unused]] float32x4_t inV2)
	{
		float32x2_t lo = vcopy_laneq_f32(vdup_n_f32(0), 0, I1 >= 4? inV2 : inV1, I1 & 0b11);
		lo = vcopy_laneq_f32(lo, 1, I2 >= 4? inV2 : inV1, I2 & 0b11);

		float32x2_t hi = vcopy_laneq_f32(vdup_n_f32(0), 0, I3 >= 4? inV2 : inV1, I3 & 0b11);
		hi = vcopy_laneq_f32(hi, 1, I4 >= 4? inV2 : inV1, I4 & 0b11);

		return vcombine_f32(lo, hi);
	}

	// Specializations
	template <>
	inline float32x4_t NeonShuffleFloat32x4<0, 0, 0, 0>([[maybe_unused]] float32x4_t inV1, [[maybe_unused]] float32x4_t inV2)
	{
		return vdupq_laneq_f32(inV1, 0);
	}

	template <>
	inline float32x4_t NeonShuffleFloat32x4<0, 1, 0, 0>([[maybe_unused]] float32x4_t inV1, [[maybe_unused]] float32x4_t inV2)
	{
		return vcombine_f32(vget_low_f32(inV1), vdup_lane_f32(vget_low_f32(inV1), 0));
	}

	template <>
	inline float32x4_t NeonShuffleFloat32x4<0, 1, 2, 2>([[maybe_unused]] float32x4_t inV1, [[maybe_unused]] float32x4_t inV2)
	{
		return vcopyq_laneq_f32(inV1, 3, inV1, 2);
	}

	template <>
	inline float32x4_t NeonShuffleFloat32x4<0, 1, 2, 3>([[maybe_unused]] float32x4_t inV1, [[maybe_unused]] float32x4_t inV2)
	{
		return inV1;
	}

	template <>
	inline float32x4_t NeonShuffleFloat32x4<0, 1, 3, 2>([[maybe_unused]] float32x4_t inV1, [[maybe_unused]] float32x4_t inV2)
	{
		return vcombine_f32(vget_low_f32(inV1), vrev64_f32(vget_high_f32(inV1)));
	}

	template <>
	inline float32x4_t NeonShuffleFloat32x4<0, 1, 3, 3>([[maybe_unused]] float32x4_t inV1, [[maybe_unused]] float32x4_t inV2)
	{
		return vcopyq_laneq_f32(inV1, 2, inV1, 3);
	}

	template <>
	inline float32x4_t NeonShuffleFloat32x4<0, 1, 4, 5>([[maybe_unused]] float32x4_t inV1, [[maybe_unused]] float32x4_t inV2)
	{
		return vreinterpretq_f32_f64(vzip1q_f64(vreinterpretq_f64_f32(inV1), vreinterpretq_f64_f32(inV2)));
	}

	template <>
	inline float32x4_t NeonShuffleFloat32x4<0, 2, 1, 1>([[maybe_unused]] float32x4_t inV1, [[maybe_unused]] float32x4_t inV2)
	{
		return vuzp1q_f32(inV1, vdupq_laneq_f32(inV1, 1));
	}

	template <>
	inline float32x4_t NeonShuffleFloat32x4<0, 2, 1, 3>([[maybe_unused]] float32x4_t inV1, [[maybe_unused]] float32x4_t inV2)
	{
		return vuzp1q_f32(inV1, vrev64q_f32(inV1));
	}

	template <>
	inline float32x4_t NeonShuffleFloat32x4<0, 2, 2, 2>([[maybe_unused]] float32x4_t inV1, [[maybe_unused]] float32x4_t inV2)
	{
		return vuzp1q_f32(inV1, vdupq_laneq_f32(inV1, 2));
	}

	template <>
	inline float32x4_t NeonShuffleFloat32x4<0, 2, 2, 3>([[maybe_unused]] float32x4_t inV1, [[maybe_unused]] float32x4_t inV2)
	{
		return vcopyq_laneq_f32(inV1, 1, inV1, 2);
	}

	template <>
	inline float32x4_t NeonShuffleFloat32x4<0, 2, 3, 2>([[maybe_unused]] float32x4_t inV1, [[maybe_unused]] float32x4_t inV2)
	{
		return vcopyq_laneq_f32(vuzp1q_f32(inV1, inV1), 2, inV1, 3);
	}

	template <>
	inline float32x4_t NeonShuffleFloat32x4<0, 2, 3, 3>([[maybe_unused]] float32x4_t inV1, [[maybe_unused]] float32x4_t inV2)
	{
		return vuzp1q_f32(inV1, vdupq_laneq_f32(inV1, 3));
	}

	template <>
	inline float32x4_t NeonShuffleFloat32x4<0, 2, 4, 6>([[maybe_unused]] float32x4_t inV1, [[maybe_unused]] float32x4_t inV2)
	{
		return vuzp1q_f32(inV1, inV2);
	}

	template <>
	inline float32x4_t NeonShuffleFloat32x4<0, 3, 1, 2>([[maybe_unused]] float32x4_t inV1, [[maybe_unused]] float32x4_t inV2)
	{
		return vzip1q_f32(inV1, vextq_f32(inV1, vdupq_laneq_f32(inV1, 2), 3));
	}

	template <>
	inline float32x4_t NeonShuffleFloat32x4<1, 0, 0, 0>([[maybe_unused]] float32x4_t inV1, [[maybe_unused]] float32x4_t inV2)
	{
		return vcombine_f32(vrev64_f32(vget_low_f32(inV1)), vdup_lane_f32(vget_low_f32(inV1), 0));
	}

	template <>
	inline float32x4_t NeonShuffleFloat32x4<1, 0, 0, 2>([[maybe_unused]] float32x4_t inV1, [[maybe_unused]] float32x4_t inV2)
	{
		return vcombine_f32(vrev64_f32(vget_low_f32(inV1)), vzip1_f32(vget_low_f32(inV1), vget_high_f32(inV1)));
	}

	template <>
	inline float32x4_t NeonShuffleFloat32x4<1, 0, 3, 2>([[maybe_unused]] float32x4_t inV1, [[maybe_unused]] float32x4_t inV2)
	{
		return vrev64q_f32(inV1);
	}

	template <>
	inline float32x4_t NeonShuffleFloat32x4<1, 1, 1, 1>([[maybe_unused]] float32x4_t inV1, [[maybe_unused]] float32x4_t inV2)
	{
		return vdupq_laneq_f32(inV1, 1);
	}

	template <>
	inline float32x4_t NeonShuffleFloat32x4<1, 1, 2, 2>([[maybe_unused]] float32x4_t inV1, [[maybe_unused]] float32x4_t inV2)
	{
		float32x4_t t = vextq_f32(inV1, inV1, 1);
		return vzip1q_f32(t, t);
	}

	template <>
	inline float32x4_t NeonShuffleFloat32x4<1, 1, 3, 3>([[maybe_unused]] float32x4_t inV1, [[maybe_unused]] float32x4_t inV2)
	{
		return vtrn2q_f32(inV1, inV1);
	}

	// Used extensively by cross product
	template <>
	inline float32x4_t NeonShuffleFloat32x4<1, 2, 0, 0>([[maybe_unused]] float32x4_t inV1, [[maybe_unused]] float32x4_t inV2)
	{
		return vcopyq_laneq_f32(vextq_f32(inV1, inV1, 1), 2, inV1, 0);
	}

	template <>
	inline float32x4_t NeonShuffleFloat32x4<1, 2, 0, 1>([[maybe_unused]] float32x4_t inV1, [[maybe_unused]] float32x4_t inV2)
	{
		return vextq_f32(vextq_f32(inV1, inV1, 3), inV1, 2);
	}

	template <>
	inline float32x4_t NeonShuffleFloat32x4<1, 2, 0, 2>([[maybe_unused]] float32x4_t inV1, [[maybe_unused]] float32x4_t inV2)
	{
		return vcopyq_laneq_f32(vuzp1q_f32(inV1, inV1), 0, inV1, 1);
	}

	template <>
	inline float32x4_t NeonShuffleFloat32x4<1, 2, 2, 2>([[maybe_unused]] float32x4_t inV1, [[maybe_unused]] float32x4_t inV2)
	{
		return vcombine_f32(vext_f32(vget_low_f32(inV1), vget_high_f32(inV1), 1), vdup_lane_f32(vget_high_f32(inV1), 0));
	}

	template <>
	inline float32x4_t NeonShuffleFloat32x4<1, 2, 3, 2>([[maybe_unused]] float32x4_t inV1, [[maybe_unused]] float32x4_t inV2)
	{
		return vextq_f32(inV1, vdupq_laneq_f32(inV1, 2), 1);
	}

	template <>
	inline float32x4_t NeonShuffleFloat32x4<1, 2, 3, 3>([[maybe_unused]] float32x4_t inV1, [[maybe_unused]] float32x4_t inV2)
	{
		return vextq_f32(inV1, vdupq_laneq_f32(inV1, 3), 1);
	}

	template <>
	inline float32x4_t NeonShuffleFloat32x4<1, 3, 0, 2>([[maybe_unused]] float32x4_t inV1, [[maybe_unused]] float32x4_t inV2)
	{
		return vuzp2q_f32(inV1, vrev64q_f32(inV1));
	}

	template <>
	inline float32x4_t NeonShuffleFloat32x4<1, 3, 5, 7>([[maybe_unused]] float32x4_t inV1, [[maybe_unused]] float32x4_t inV2)
	{
		return vuzp2q_f32(inV1, inV2);
	}

	template <>
	inline float32x4_t NeonShuffleFloat32x4<2, 0, 1, 1>([[maybe_unused]] float32x4_t inV1, [[maybe_unused]] float32x4_t inV2)
	{
		return vcopyq_laneq_f32(vzip1q_f32(inV1, inV1), 0, inV1, 2);
	}

	template <>
	inline float32x4_t NeonShuffleFloat32x4<2, 0, 1, 2>([[maybe_unused]] float32x4_t inV1, [[maybe_unused]] float32x4_t inV2)
	{
		return vextq_f32(vuzp1q_f32(inV1, inV1), inV1, 3);
	}

	template <>
	inline float32x4_t NeonShuffleFloat32x4<2, 1, 0, 0>([[maybe_unused]] float32x4_t inV1, [[maybe_unused]] float32x4_t inV2)
	{
		float32x4_t t = vextq_f32(vuzp1q_f32(inV1, inV1), inV1, 3);
		return vuzp1q_f32(t, vuzp1q_f32(inV1, inV1));
	}

	template <>
	inline float32x4_t NeonShuffleFloat32x4<2, 1, 0, 3>([[maybe_unused]] float32x4_t inV1, [[maybe_unused]] float32x4_t inV2)
	{
		float32x4_t t = vrev64q_f32(inV1);
		return vextq_f32(t, t, 3);
	}

	template <>
	inline float32x4_t NeonShuffleFloat32x4<2, 2, 1, 0>([[maybe_unused]] float32x4_t inV1, [[maybe_unused]] float32x4_t inV2)
	{
		return vextq_f32(vtrn1q_f32(inV1, inV1), vrev64q_f32(inV1), 2);
	}

	template <>
	inline float32x4_t NeonShuffleFloat32x4<2, 2, 1, 1>([[maybe_unused]] float32x4_t inV1, [[maybe_unused]] float32x4_t inV2)
	{
		float32x4_t t = vcopyq_laneq_f32(inV1, 3, inV1, 1);
		return vzip2q_f32(t, t);
	}

	template <>
	inline float32x4_t NeonShuffleFloat32x4<2, 2, 1, 2>([[maybe_unused]] float32x4_t inV1, [[maybe_unused]] float32x4_t inV2)
	{
		return vcopyq_laneq_f32(vdupq_laneq_f32(inV1, 2), 2, inV1, 1);
	}

	template <>
	inline float32x4_t NeonShuffleFloat32x4<2, 2, 2, 2>([[maybe_unused]] float32x4_t inV1, [[maybe_unused]] float32x4_t inV2)
	{
		return vdupq_laneq_f32(inV1, 2);
	}

	template <>
	inline float32x4_t NeonShuffleFloat32x4<2, 3, 0, 1>([[maybe_unused]] float32x4_t inV1, [[maybe_unused]] float32x4_t inV2)
	{
		return vcombine_f32(vget_high_f32(inV1), vget_low_f32(inV1));
	}

	template <>
	inline float32x4_t NeonShuffleFloat32x4<2, 3, 1, 2>([[maybe_unused]] float32x4_t inV1, [[maybe_unused]] float32x4_t inV2)
	{
		return vextq_f32(inV1, vextq_f32(inV1, inV1, 1), 2);
	}

	template <>
	inline float32x4_t NeonShuffleFloat32x4<2, 3, 2, 2>([[maybe_unused]] float32x4_t inV1, [[maybe_unused]] float32x4_t inV2)
	{
		return vextq_f32(inV1, vdupq_laneq_f32(inV1, 2), 2);
	}

	template <>
	inline float32x4_t NeonShuffleFloat32x4<2, 3, 2, 3>([[maybe_unused]] float32x4_t inV1, [[maybe_unused]] float32x4_t inV2)
	{
		return vreinterpretq_f32_f64(vdupq_laneq_f64(vreinterpretq_f64_f32(inV1), 1));
	}

	template <>
	inline float32x4_t NeonShuffleFloat32x4<2, 3, 6, 7>([[maybe_unused]] float32x4_t inV1, [[maybe_unused]] float32x4_t inV2)
	{
		return vreinterpretq_f32_f64(vzip2q_f64(vreinterpretq_f64_f32(inV1), vreinterpretq_f64_f32(inV2)));
	}

	template <>
	inline float32x4_t NeonShuffleFloat32x4<3, 0, 1, 2>([[maybe_unused]] float32x4_t inV1, [[maybe_unused]] float32x4_t inV2)
	{
		return vextq_f32(inV1, inV1, 3);
	}

	template <>
	inline float32x4_t NeonShuffleFloat32x4<3, 0, 3, 2>([[maybe_unused]] float32x4_t inV1, [[maybe_unused]] float32x4_t inV2)
	{
		return vtrn1q_f32(vdupq_laneq_f32(inV1, 3), inV1);
	}

	template <>
	inline float32x4_t NeonShuffleFloat32x4<3, 2, 1, 0>([[maybe_unused]] float32x4_t inV1, [[maybe_unused]] float32x4_t inV2)
	{
		float32x4_t t = vrev64q_f32(inV1);
		return vextq_f32(t, t, 2);
	}

	template <>
	inline float32x4_t NeonShuffleFloat32x4<3, 2, 3, 2>([[maybe_unused]] float32x4_t inV1, [[maybe_unused]] float32x4_t inV2)
	{
		float32x2_t zy = vrev64_f32(vget_high_f32(inV1));
		return vcombine_f32(zy, zy);
	}

	template <>
	inline float32x4_t NeonShuffleFloat32x4<3, 3, 3, 3>([[maybe_unused]] float32x4_t inV1, [[maybe_unused]] float32x4_t inV2)
	{
		return vdupq_laneq_f32(inV1, 3);
	}

#else
    template <unsigned I1, unsigned I2, unsigned I3, unsigned I4>
    inline float32x4_t NeonShuffleFloat32x4([[maybe_unused]] float32x4_t V1, [[maybe_unused]] float32x4_t V2) noexcept
    {
        return __builtin_shufflevector(V1, V2, I1, I2, I3, I4);
    }
#endif

    template <unsigned I1, unsigned I2, unsigned I3, unsigned I4>
    inline uint32x4_t NeonShuffleUInt32x4(uint32x4_t V1, uint32x4_t V2) noexcept
    {
#if defined(_MSC_VER) || (defined(__GNUC__) && !defined(__clang__) && __GNUC__ < 12)
        return vreinterpretq_u32_f32(NeonShuffleFloat32x4<I1, I2, I3, I4>(
            vreinterpretq_f32_u32(V1), vreinterpretq_f32_u32(V2)
        ));
#else
        return __builtin_shufflevector(V1, V2, I1, I2, I3, I4);
#endif
    }
}

#endif // ARM64 NEON
