#include "Empire.Kernel/Kernel.System/System.hpp"

#include <android/log.h>
#include <errno.h>
#include <sched.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        using namespace EmpireKernel::KernelTypes;

        namespace EmpireKernel
        {
            namespace KernelSystem
            {
                Size GetPageSize() noexcept
                {
                    static const Size PageSize = static_cast<Size>(sysconf(_SC_PAGESIZE));
                    return PageSize;
                }

                void* ReservePages(Size ByteCount) noexcept
                {
                    void* Pages = mmap(nullptr, ByteCount, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
                    return Pages == MAP_FAILED ? nullptr : Pages;
                }

                void ReleasePages(void* Pages, Size ByteCount) noexcept
                {
                    munmap(Pages, ByteCount);
                }

                // stdout и stderr на Android никуда не видны, поэтому вывод уходит в logcat.
                void WriteOutput(Bool ToError, const Char* Text, Size ByteCount) noexcept
                {
                    Char Buffer[1024];

                    while (ByteCount > 0)
                    {
                        Size Length = ByteCount < sizeof(Buffer) - 1 ? ByteCount : sizeof(Buffer) - 1;

                        for (Size Index = 0; Index < Length; Index++)
                            Buffer[Index] = Text[Index];

                        Text += Length;
                        ByteCount -= Length;

                        if (Length > 0 && Buffer[Length - 1] == '\n')
                            Length--;

                        Buffer[Length] = '\0';
                        __android_log_write(ToError ? ANDROID_LOG_ERROR : ANDROID_LOG_INFO, "Empire", Buffer);
                    }
                }

                void YieldThread() noexcept
                {
                    sched_yield();
                }

                void SleepThread(UInt32 Microseconds) noexcept
                {
                    timespec Duration{};
                    Duration.tv_sec = static_cast<time_t>(Microseconds / 1000000);
                    Duration.tv_nsec = static_cast<long>(Microseconds % 1000000) * 1000;

                    while (clock_nanosleep(CLOCK_MONOTONIC, 0, &Duration, &Duration) == EINTR)
                    {
                    }
                }

                [[noreturn]] void Terminate() noexcept
                {
                    abort();
                }
            }
        }
    }
}
