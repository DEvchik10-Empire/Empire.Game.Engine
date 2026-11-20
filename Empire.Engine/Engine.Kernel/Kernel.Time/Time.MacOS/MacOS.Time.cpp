#include "Empire.Engine/Engine.Kernel/Kernel.Time/Time.hpp"

#include "Empire.Engine/Engine.Kernel/Kernel.Compiler/Compiler.hpp"

#include <mach/mach_time.h>

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
                        static mach_timebase_info_data_t Timebase = []()
                        {
                            mach_timebase_info_data_t Value{};
                            mach_timebase_info(&Value);
                            return Value;
                        }();
                        if (Timebase.denom == 0)
                            return 0;

                        const KernelTypes::UInt64 Ticks = mach_absolute_time();
                        const KernelTypes::UInt64 Nanoseconds = (Ticks / Timebase.denom) * Timebase.numer +
                            ((Ticks % Timebase.denom) * Timebase.numer) / Timebase.denom;
                        return Nanoseconds / 1000ULL;
                    }
                }
            }
        }
    }
}
