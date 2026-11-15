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
                        GetNativeSystemInfo(&Info);
                        return static_cast<Size>(Info.dwPageSize);
                    }();

                    return PageSize;
                }

                // В UWP обычный VirtualAlloc запрещён, вместо него VirtualAllocFromApp.
                void* ReservePages(Size ByteCount) noexcept
                {
                    return VirtualAllocFromApp(nullptr, ByteCount, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
                }

                void ReleasePages(void* Pages, Size) noexcept
                {
                    VirtualFree(Pages, 0, MEM_RELEASE);
                }

                // У UWP-приложения нет консоли, вывод идёт в отладчик.
                void WriteOutput(Bool, const Char* Text, Size ByteCount) noexcept
                {
                    Char Buffer[1024];

                    while (ByteCount > 0)
                    {
                        const Size Length = ByteCount < sizeof(Buffer) - 1 ? ByteCount : sizeof(Buffer) - 1;

                        for (Size Index = 0; Index < Length; Index++)
                            Buffer[Index] = Text[Index];

                        Buffer[Length] = '\0';
                        OutputDebugStringA(Buffer);

                        Text += Length;
                        ByteCount -= Length;
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
                    __builtin_trap();
                }
            }
        }
    }
}
