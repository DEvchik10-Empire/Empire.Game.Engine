#include "Empire.Runtime/Runtime.Tests/Tests.Kernel/Kernel.Unit/Unit.Compiler.hpp"

#include "Empire.Engine/Engine.Kernel/Kernel.Compiler/Compiler.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Config/Config.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Time/Time.hpp"

#include <cstdio>

namespace KernelCompiler = DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineKernel::KernelCompiler;
namespace KernelConfig = DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineKernel::KernelConfig;
namespace KernelTime = DEvchik10Empire::EmpireGameEngine::EmpireEngine::EngineKernel::KernelTime;

static int Failures = 0;
static int Checks = 0;

static void Check(bool Condition, const char* Expression, int Line)
{
    ++Checks;
    if (!Condition)
    {
        ++Failures;
        std::printf("  FAIL Unit.Compiler.cpp:%d  %s\n", Line, Expression);
    }
}

int RunCompilerUnitTests()
{
    Failures = 0;
    Checks = 0;

    static_assert(!KernelConfig::UseFMA);
    Check(!KernelConfig::UseFMA, "!KernelConfig::UseFMA", __LINE__);

    const unsigned long long InitialControlWord = KernelCompiler::ReadFPControlWord();
    {
        KernelCompiler::FPFlushDenormals FlushDenormals;
        const unsigned long long FlushedControlWord = KernelCompiler::ReadFPControlWord();
        Check((FlushedControlWord & KernelCompiler::FlushDenormalsMask) == KernelCompiler::FlushDenormalsMask,
            "FlushDenormals mask is set", __LINE__);

        {
            KernelCompiler::FPFlushDenormals NestedFlushDenormals;
            Check((KernelCompiler::ReadFPControlWord() & KernelCompiler::FlushDenormalsMask) == KernelCompiler::FlushDenormalsMask,
                "nested FlushDenormals mask is set", __LINE__);
        }

        Check((KernelCompiler::ReadFPControlWord() & KernelCompiler::FlushDenormalsMask) ==
                (FlushedControlWord & KernelCompiler::FlushDenormalsMask),
            "nested FlushDenormals restores the prior mask", __LINE__);
    }
    Check((KernelCompiler::ReadFPControlWord() & KernelCompiler::FlushDenormalsMask) ==
            (InitialControlWord & KernelCompiler::FlushDenormalsMask),
        "FlushDenormals restores the original mask", __LINE__);

    const unsigned long long FirstTick = KernelTime::GetProcessorTickCount();
    const unsigned long long SecondTick = KernelTime::GetProcessorTickCount();
    if (KernelCompiler::IsX86Architecture ||
        (KernelCompiler::IsArmArchitecture && KernelCompiler::CpuAddressBits == 64))
    {
        Check(FirstTick != 0, "first processor tick is nonzero", __LINE__);
        Check(SecondTick != 0, "second processor tick is nonzero", __LINE__);
    }

    std::printf("Compiler: %d checks, %d failures\n", Checks, Failures);
    return Failures == 0 ? 0 : 1;
}
