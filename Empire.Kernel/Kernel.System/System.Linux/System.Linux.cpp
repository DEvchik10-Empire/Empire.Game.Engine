#include "Empire.Kernel/Kernel.System/System.hpp"

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
        using namespace EngineKernel::Types;

        namespace EngineKernel
        {
            namespace System
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

                void WriteOutput(Bool ToError, const Char* Text, Size ByteCount) noexcept
                {
                    const int Descriptor = ToError ? STDERR_FILENO : STDOUT_FILENO;

                    while (ByteCount > 0)
                    {
                        const auto Written = write(Descriptor, Text, ByteCount);

                        if (Written < 0)
                        {
                            if (errno == EINTR)
                                continue;

                            return;
                        }

                        if (Written == 0)
                            return;

                        Text += Written;
                        ByteCount -= static_cast<Size>(Written);
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
