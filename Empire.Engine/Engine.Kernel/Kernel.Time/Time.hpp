#pragma once

#include "Empire.Engine/Engine.Kernel/Kernel.Types/Types.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireEngine
        {
            namespace EngineKernel
            {
                namespace KernelTime
                {
                    // Приблизительный счётчик CPU cycles; на неподдерживаемых CPU возвращает 0.
                    [[nodiscard]] KernelTypes::UInt64 GetProcessorTickCount() noexcept;

                    // Монотонное время в микросекундах; не зависит от перевода системных часов.
                    [[nodiscard]] KernelTypes::UInt64 GetMonotonicMicroseconds() noexcept;
                }
            }
        }
    }
}
