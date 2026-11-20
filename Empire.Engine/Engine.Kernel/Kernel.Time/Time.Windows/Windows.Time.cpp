#include "Empire.Engine/Engine.Kernel/Kernel.Time/Time.hpp"

#include "Empire.Engine/Engine.Kernel/Kernel.Compiler/Compiler.hpp"

#include <windows.h>

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
                        if constexpr (KernelCompiler::IsX86Architecture)
                        {
                            return KernelCompiler::ReadProcessorTickCount();
                        }
                        else
                        {
                            LARGE_INTEGER Counter{};
                            if (QueryPerformanceCounter(&Counter) == 0)
                                return 0;
                            return static_cast<KernelTypes::UInt64>(Counter.QuadPart);
                        }
                    }

                    KernelTypes::UInt64 GetMonotonicMicroseconds() noexcept
                    {
                        LARGE_INTEGER Counter{};
                        LARGE_INTEGER Frequency{};
                        if (QueryPerformanceCounter(&Counter) == 0 || QueryPerformanceFrequency(&Frequency) == 0 || Frequency.QuadPart <= 0)
                            return 0;

                        const KernelTypes::UInt64 Ticks = static_cast<KernelTypes::UInt64>(Counter.QuadPart);
                        const KernelTypes::UInt64 Rate = static_cast<KernelTypes::UInt64>(Frequency.QuadPart);
                        return (Ticks / Rate) * 1000000ULL + ((Ticks % Rate) * 1000000ULL) / Rate;
                    }
                }
            }
        }
    }
}
