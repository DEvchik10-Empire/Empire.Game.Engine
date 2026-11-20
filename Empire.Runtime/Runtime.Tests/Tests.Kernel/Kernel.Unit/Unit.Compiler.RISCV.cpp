#include "Empire.Engine/Engine.Kernel/Kernel.Compiler/Compiler.hpp"

bool RunCompilerRISCVCompileChecks()
{
#if defined(__riscv_vector)
    static_assert(DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineKernel::KernelCompiler::IsRISCVArchitecture);
    static_assert(DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineKernel::KernelCompiler::SupportsRISCVVector);
    static_assert(DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineKernel::KernelCompiler::CpuAddressBits == sizeof(void*) * 8);
    using namespace DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineKernel::KernelCompiler::RISCV;

    const float Values[4] = {1.0f, 2.0f, 3.0f, 4.0f};
    const vfloat32m1_t Vector = __riscv_vle32_v_f32m1(Values, 4);
    const vfloat32m1_t GenericShuffle = RVVShuffleFloat32x4<3, 1, 2, 0>(Vector, Vector);
    const vfloat32m1_t SpecializedShuffle = RVVShuffleFloat32x4<0, 1, 2, 3>(Vector, Vector);
    const vfloat32m1_t DeterministicSum = RVVSumElementsFloat32x4<true>(Vector);
    const vfloat32m1_t FastSum = RVVSumElementsFloat32x4<false>(Vector);

    float GenericOutput[4];
    float SpecializedOutput[4];
    float DeterministicOutput[4];
    float FastOutput[4];
    __riscv_vse32_v_f32m1(GenericOutput, GenericShuffle, 4);
    __riscv_vse32_v_f32m1(SpecializedOutput, SpecializedShuffle, 4);
    __riscv_vse32_v_f32m1(DeterministicOutput, DeterministicSum, 4);
    __riscv_vse32_v_f32m1(FastOutput, FastSum, 4);

    return GenericOutput[0] == 4.0f && GenericOutput[1] == 2.0f && GenericOutput[2] == 3.0f && GenericOutput[3] == 1.0f
        && SpecializedOutput[0] == 1.0f && SpecializedOutput[1] == 2.0f && SpecializedOutput[2] == 3.0f && SpecializedOutput[3] == 4.0f
        && DeterministicOutput[0] == 10.0f && FastOutput[0] == 10.0f;
#else
    return true;
#endif
}
