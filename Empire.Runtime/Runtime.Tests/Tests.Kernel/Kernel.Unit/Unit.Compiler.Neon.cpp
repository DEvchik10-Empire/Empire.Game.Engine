#include "Empire.Engine/Engine.Kernel/Kernel.Compiler/Compiler.hpp"
#include <stdint.h>

bool RunCompilerNeonCompileChecks()
{
#if defined(__aarch64__) || defined(_M_ARM64) || defined(_M_ARM64EC)
    static_assert(DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineKernel::KernelCompiler::SupportsARM64NEON);
    static_assert(DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineKernel::KernelCompiler::CpuAddressBits == sizeof(void*) * 8);
    using namespace DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineKernel::KernelCompiler::Neon;

    const float Values[4] = {1.0f, 2.0f, 3.0f, 4.0f};
    const float32x4_t FloatVector = vld1q_f32(Values);
    const uint32x4_t UIntVector = MakeUInt32x4(1, 2, 3, 4);
    const int32x4_t IntVector = MakeInt32x4(1, 2, 3, 4);
    const int8x16_t IntBytes = MakeInt8x16(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
    const uint8x16_t UIntBytes = MakeUInt8x16(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);

    const float32x4_t FloatShuffle = NeonShuffleFloat32x4<0, 1, 2, 3>(FloatVector, FloatVector);
    const float32x4_t FloatFallback = NeonShuffleFloat32x4<3, 1, 2, 0>(FloatVector, FloatVector);
    const uint32x4_t UIntShuffle = NeonShuffleUInt32x4<0, 1, 2, 3>(UIntVector, UIntVector);

    float IdentityOutput[4];
    float FloatOutput[4];
    uint32_t UIntOutput[4];
    int32_t IntOutput[4];
    int8_t IntByteOutput[16];
    uint8_t UIntByteOutput[16];
    vst1q_f32(IdentityOutput, FloatShuffle);
    vst1q_f32(FloatOutput, FloatFallback);
    vst1q_u32(UIntOutput, UIntShuffle);
    vst1q_s32(IntOutput, IntVector);
    vst1q_s8(IntByteOutput, IntBytes);
    vst1q_u8(UIntByteOutput, UIntBytes);

    return IdentityOutput[0] == 1.0f && IdentityOutput[1] == 2.0f && IdentityOutput[2] == 3.0f && IdentityOutput[3] == 4.0f
        && FloatOutput[0] == 4.0f && FloatOutput[1] == 2.0f && FloatOutput[2] == 3.0f && FloatOutput[3] == 1.0f
        && UIntOutput[0] == 1 && UIntOutput[1] == 2 && UIntOutput[2] == 3 && UIntOutput[3] == 4
        && IntOutput[0] == 1 && IntOutput[1] == 2 && IntOutput[2] == 3 && IntOutput[3] == 4
        && IntByteOutput[15] == 15 && UIntByteOutput[15] == 15;
#else
    return true;
#endif
}
