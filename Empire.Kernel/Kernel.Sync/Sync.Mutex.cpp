#include "Empire.Kernel/Kernel.Sync/Sync.Mutex.hpp"

#if defined(_WIN32)
    #if !defined(WIN32_LEAN_AND_MEAN)
        #define WIN32_LEAN_AND_MEAN
    #endif
    #if !defined(NOMINMAX)
        #define NOMINMAX
    #endif
    #include <windows.h>
#else
    #include <sched.h>
    #include <time.h>
#endif

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        using namespace EngineKernel::Types;

        namespace EngineKernel
        {
            namespace Sync
            {
                namespace
                {
                    void PauseProcessor() noexcept
                    {
                #if defined(__x86_64__) || defined(__i386__)
                        __builtin_ia32_pause();
                #elif defined(__aarch64__) || defined(__arm__)
                        __asm__ volatile("yield");
                #endif
                    }

                    void YieldThread() noexcept
                    {
                #if defined(_WIN32)
                        SwitchToThread();
                #else
                        sched_yield();
                #endif
                    }

                    void SleepBriefly() noexcept
                    {
                #if defined(_WIN32)
                        Sleep(1);
                #else
                        timespec Duration{};
                        Duration.tv_sec = 0;
                        Duration.tv_nsec = 100000;
                        nanosleep(&Duration, nullptr);
                #endif
                    }
                }

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

                        if (Attempts < 64)
                            PauseProcessor();
                        else if (Attempts < 2048)
                            YieldThread();
                        else
                            SleepBriefly();
                    }
                }

                Bool Mutex::TryLock() noexcept
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
