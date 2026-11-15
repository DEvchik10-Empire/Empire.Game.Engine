#pragma once

#include "Empire.Kernel/Kernel.Types/Types.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EngineKernel
        {
            namespace System
            {
                [[nodiscard]] Types::Size GetPageSize() noexcept;

                // Выделяет обнулённые страницы, доступные на чтение и запись. Возвращает nullptr при нехватке памяти.
                [[nodiscard]] void* ReservePages(Types::Size ByteCount) noexcept;
                void ReleasePages(void* Pages, Types::Size ByteCount) noexcept;

                void WriteOutput(Types::Bool ToError, const Types::Char* Text, Types::Size ByteCount) noexcept;

                void YieldThread() noexcept;
                void SleepThread(Types::UInt32 Microseconds) noexcept;

                [[noreturn]] void Terminate() noexcept;
            }
        }
    }
}
