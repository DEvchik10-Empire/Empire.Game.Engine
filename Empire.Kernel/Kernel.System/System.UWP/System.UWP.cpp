#include "Empire.Kernel/Kernel.Compiler/Compiler.hpp"
#include "Empire.Kernel/Kernel.System/System.hpp"

#include <windows.h>

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
                    // Деление до сложения: (Microseconds + 999) переполняло UInt32 при больших значениях.
                    Sleep(Microseconds / 1000 + (Microseconds % 1000 != 0 ? 1 : 0));
                }

                [[noreturn]] void Terminate() noexcept
                {
                    KernelCompiler::Trap();
                }
            }
        }
    }
}
