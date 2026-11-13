#include "Empire.Kernel/Kernel.System/System.hpp"

#include <windows.h>

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
                    static const Size PageSize = []
                    {
                        SYSTEM_INFO Info{};
                        GetSystemInfo(&Info);
                        return static_cast<Size>(Info.dwPageSize);
                    }();

                    return PageSize;
                }

                void* ReservePages(Size ByteCount) noexcept
                {
                    return VirtualAlloc(nullptr, ByteCount, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
                }

                void ReleasePages(void* Pages, Size) noexcept
                {
                    VirtualFree(Pages, 0, MEM_RELEASE);
                }

                void WriteOutput(Bool ToError, const Char* Text, Size ByteCount) noexcept
                {
                    HANDLE Handle = GetStdHandle(ToError ? STD_ERROR_HANDLE : STD_OUTPUT_HANDLE);

                    if (Handle == nullptr || Handle == INVALID_HANDLE_VALUE)
                        return;

                    while (ByteCount > 0)
                    {
                        DWORD Written = 0;

                        if (!WriteFile(Handle, Text, static_cast<DWORD>(ByteCount), &Written, nullptr) || Written == 0)
                            return;

                        Text += Written;
                        ByteCount -= static_cast<Size>(Written);
                    }
                }

                void YieldThread() noexcept
                {
                    SwitchToThread();
                }

                void SleepThread(UInt32 Microseconds) noexcept
                {
                    Sleep((Microseconds + 999) / 1000);
                }

                [[noreturn]] void Terminate() noexcept
                {
                    TerminateProcess(GetCurrentProcess(), 3);

                    for (;;)
                    {
                    }
                }
            }
        }
    }
}
