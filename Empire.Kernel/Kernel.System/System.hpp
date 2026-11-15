#pragma once

#include "Empire.Kernel/Kernel.Types/Types.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireKernel
        {
            namespace KernelSystem
            {
                [[nodiscard]] KernelTypes::Size GetPageSize() noexcept;

                // Выделяет обнулённые страницы, доступные на чтение и запись. Возвращает nullptr при нехватке памяти.
                [[nodiscard]] void* ReservePages(KernelTypes::Size ByteCount) noexcept;
                void ReleasePages(void* Pages, KernelTypes::Size ByteCount) noexcept;

                void WriteOutput(KernelTypes::Bool ToError, const KernelTypes::Char* Text, KernelTypes::Size ByteCount) noexcept;

                void YieldThread() noexcept;
                void SleepThread(KernelTypes::UInt32 Microseconds) noexcept;

                [[noreturn]] void Terminate() noexcept;
            }
        }
    }
}
