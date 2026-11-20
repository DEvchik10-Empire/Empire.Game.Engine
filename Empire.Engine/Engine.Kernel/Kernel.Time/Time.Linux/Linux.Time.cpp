#include "Empire.Engine/Engine.Kernel/Kernel.Time/Time.hpp"

#include "Empire.Engine/Engine.Kernel/Kernel.Compiler/Compiler.hpp"

#include <time.h>

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireEngine
        {
            namespace EngineKernel
            {
                namespace KernelTime
                {
                    KernelTypes::UInt64 GetProcessorTickCount() noexcept
                    {
                        return KernelCompiler::ReadProcessorTickCount();
                    }

                    KernelTypes::UInt64 GetMonotonicMicroseconds() noexcept
                    {
                        timespec Value{};
                        if (clock_gettime(CLOCK_MONOTONIC, &Value) != 0)
                            return 0;
                        return static_cast<KernelTypes::UInt64>(Value.tv_sec) * 1000000ULL +
                            static_cast<KernelTypes::UInt64>(Value.tv_nsec) / 1000ULL;
                    }
                }
            }
        }
    }
}
