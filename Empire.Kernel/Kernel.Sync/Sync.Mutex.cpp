#include "Empire.Kernel/Kernel.Sync/Sync.Mutex.hpp"

#include "Empire.Kernel/Kernel.Compiler/Compiler.hpp"
#include "Empire.Kernel/Kernel.System/System.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        using namespace EmpireKernel::KernelTypes;

        namespace EmpireKernel
        {
            namespace KernelSync
            {
                void Mutex::Lock() noexcept
                {
                    UInt32 Expected = 0;

                    if (State.CompareExchange(Expected, 1, MemoryOrder::Acquire))
                        return;

                    // Этапы ожидания: короткое вращение с Pause, затем уступка процессора, затем короткий сон (долгое ожидание не жжёт процессор).
                    UInt32 Attempts = 0;
                    const UInt32 SleepMicroseconds = 50;

                    for (;;)
                    {
                        if (State.Load(MemoryOrder::Relaxed) == 0)
                        {
                            Expected = 0;

                            if (State.CompareExchange(Expected, 1, MemoryOrder::Acquire))
                                return;
                        }

                        Attempts++;

                        if (Attempts < 8)
                        {
                            KernelCompiler::Pause();
                        }
                        else if (Attempts < 2048)
                        {
                            KernelSystem::YieldThread();
                        }
                        else
                        {
                            KernelSystem::SleepThread(SleepMicroseconds);
                        }
                    }
                }

                [[nodiscard]] Bool Mutex::TryLock() noexcept
                {
                    UInt32 Expected = 0;
                    return State.CompareExchange(Expected, 1, MemoryOrder::Acquire);
                }

                void Mutex::Unlock() noexcept
                {
                    State.Store(0, MemoryOrder::Release);
                }
            }
        }
    }
}
