#pragma once

// RISC-V Vector extension helpers ported from Jolt 5.7.0 Core/RISCVVector.h.
// All architecture detection and intrinsic headers remain inside Kernel.Compiler.
#if defined(__riscv_vector)

#include <riscv_vector.h>
#include <stdint.h>

namespace DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineKernel::KernelCompiler::RISCV
{
template <unsigned IndexX, unsigned IndexY, unsigned IndexZ, unsigned IndexW>
inline vfloat32m1_t RVVShuffleFloat32x4([[maybe_unused]] vfloat32m1_t inV0, [[maybe_unused]] vfloat32m1_t inV1)
{
	vfloat32m2_t combined = __riscv_vlmul_ext_v_f32m1_f32m2(inV0);
	combined = __riscv_vslideup_vx_f32m2(combined, __riscv_vlmul_ext_v_f32m1_f32m2(inV1), 4, 8);

	const uint32_t indices_raw[4] = { IndexX, IndexY, IndexZ, IndexW };
	const vuint32m1_t v_indices_m1 = __riscv_vle32_v_u32m1(indices_raw, 4);
	const vuint32m2_t v_indices_m2 = __riscv_vlmul_ext_v_u32m1_u32m2(v_indices_m1);

	const vfloat32m2_t gathered_m2 = __riscv_vrgather_vv_f32m2(combined, v_indices_m2, 4);
	return __riscv_vlmul_trunc_v_f32m2_f32m1(gathered_m2);
}

template <>
inline vfloat32m1_t RVVShuffleFloat32x4<0, 1, 2, 3>([[maybe_unused]] vfloat32m1_t inV0, [[maybe_unused]] vfloat32m1_t inV1)
{
	return inV0;
}

template <>
inline vfloat32m1_t RVVShuffleFloat32x4<0, 1, 4, 5>([[maybe_unused]] vfloat32m1_t inV0, [[maybe_unused]] vfloat32m1_t inV1)
{
	vfloat32m1_t result = inV0;
	return __riscv_vslideup_vx_f32m1(result, inV1, 2, 4);
}

template <>
inline vfloat32m1_t RVVShuffleFloat32x4<1, 0, 3, 2>([[maybe_unused]] vfloat32m1_t inV0, [[maybe_unused]] vfloat32m1_t inV1)
{
	// Avoids m2 extension overhead that the default implementation of RVVShuffleFloat32x4 has
	const uint32_t indices_raw[4] = { 1, 0, 3, 2 };
	const vuint32m1_t indices = __riscv_vle32_v_u32m1(indices_raw, 4);
	return __riscv_vrgather_vv_f32m1(inV0, indices, 4);
}

template <>
inline vfloat32m1_t RVVShuffleFloat32x4<2, 3, 0, 1>([[maybe_unused]] vfloat32m1_t inV0, [[maybe_unused]] vfloat32m1_t inV1)
{
	vfloat32m1_t upper = __riscv_vslidedown_vx_f32m1(inV0, 2, 4);
	return __riscv_vslideup_vx_f32m1(upper, inV0, 2, 4);
}

template <>
inline vfloat32m1_t RVVShuffleFloat32x4<2, 3, 6, 7>([[maybe_unused]] vfloat32m1_t inV0, [[maybe_unused]] vfloat32m1_t inV1)
{
	return __riscv_vslidedown_vx_f32m1_tu(inV1, inV0, 2, 2);
}

template <>
inline vfloat32m1_t RVVShuffleFloat32x4<4, 5, 6, 7>([[maybe_unused]] vfloat32m1_t inV0, [[maybe_unused]] vfloat32m1_t inV1)
{
	return inV1;
}

template <>
inline vfloat32m1_t RVVShuffleFloat32x4<0, 2, 4, 6>([[maybe_unused]] vfloat32m1_t inV0, [[maybe_unused]] vfloat32m1_t inV1)
{
	vfloat32m2_t combined = __riscv_vlmul_ext_v_f32m1_f32m2(inV0);
	combined = __riscv_vslideup_vx_f32m2(combined, __riscv_vlmul_ext_v_f32m1_f32m2(inV1), 4, 8);

	vuint64m2_t combined_u64 = __riscv_vreinterpret_v_u32m2_u64m2(__riscv_vreinterpret_v_f32m2_u32m2(combined));

	// vnsrl extracts lower 32 bits from all 4 u64 elements -> [0, 2, 4, 6]
	vuint32m1_t result = __riscv_vnsrl_wx_u32m1(combined_u64, 0, 4);

	return __riscv_vreinterpret_v_u32m1_f32m1(result);
}

template <>
inline vfloat32m1_t RVVShuffleFloat32x4<1, 3, 5, 7>([[maybe_unused]] vfloat32m1_t inV0, [[maybe_unused]] vfloat32m1_t inV1)
{
	vfloat32m2_t combined = __riscv_vlmul_ext_v_f32m1_f32m2(inV0);
	combined = __riscv_vslideup_vx_f32m2(combined, __riscv_vlmul_ext_v_f32m1_f32m2(inV1), 4, 8);

	vuint64m2_t combined_u64 = __riscv_vreinterpret_v_u32m2_u64m2(__riscv_vreinterpret_v_f32m2_u32m2(combined));

	// vnsrl with shift=32 extracts upper 32 bits from all 4 u64 elements -> [1, 3, 5, 7]
	vuint32m1_t result = __riscv_vnsrl_wx_u32m1(combined_u64, 32, 4);

	return __riscv_vreinterpret_v_u32m1_f32m1(result);
}

/// Given InV = (a, b, c, d), computes a deterministic pairwise sum or a fast unordered reduction.
template <bool Deterministic>
inline vfloat32m1_t RVVSumElementsFloat32x4(vfloat32m1_t inV)
{
    if constexpr (Deterministic)
    {
        const vfloat32m1_t shift1 = __riscv_vslidedown_vx_f32m1(inV, 1, 4);
        const vfloat32m1_t sum_pairs = __riscv_vfadd_vv_f32m1(inV, shift1, 4);
        const vfloat32m1_t shift2 = __riscv_vslidedown_vx_f32m1(sum_pairs, 2, 4);
        return __riscv_vfadd_vv_f32m1(sum_pairs, shift2, 4);
    }
    else
    {
        const vfloat32m1_t zeros = __riscv_vfmv_v_f_f32m1(0.0f, 4);
        return __riscv_vfredusum_vs_f32m1_f32m1(inV, zeros, 4);
    }
}

}

#endif // __riscv_vector
