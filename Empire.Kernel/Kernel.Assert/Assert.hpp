#pragma once

#include <source_location>
#include <stdexcept>

#include "Empire.Kernel/Kernel.Types/Types.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EngineKernel
        {
            namespace Assert
            {
                using Error = std::runtime_error;

                void Check(
                    Types::Bool Condition,
                    const Types::Char* Message,
                    const std::source_location& Location = std::source_location::current()
                );

                [[noreturn]] void Fail(
                    const Types::Char* Message,
                    const std::source_location& Location = std::source_location::current()
                );
            }
        }
    }
}
