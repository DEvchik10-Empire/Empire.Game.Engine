#include "Empire.Kernel/Kernel.Sync/Sync.Mutex.hpp"

#include "Empire.Kernel/Kernel.System/System.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        using namespace EngineKernel::Types;

        namespace EngineKernel
        {
            namespace Sync
            {
                void Mutex::Lock() noexcept
                {
                    UInt32 Expected = 0;

                    if (State.CompareExchange(Expected, 1, MemoryOrder::Acquire))
                        return;

                    UInt32 Attempts = 0;

                    for (;;)
                    {
                        if (State.Load(MemoryOrder::Relaxed) == 0)
                        {
                            Expected = 0;

                            if (State.CompareExchange(Expected, 1, MemoryOrder::Acquire))
                                return;
                        }

                        Attempts++;

                        if (Attempts < 2048)
                            System::YieldThread();
                        else
                            System::SleepThread(100);
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
