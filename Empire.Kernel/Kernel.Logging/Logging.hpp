#pragma once

#include "Empire.Kernel/Kernel.Containers/Containers.StringView.hpp"
#include "Empire.Kernel/Kernel.Types/Types.hpp"

#include <iostream>
#include <source_location>

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EngineKernel
        {
            namespace Logging
            {
                enum class Level : Types::UInt8
                {
                    Trace,
                    Debug,
                    Info,
                    Warning,
                    Error,
                    Fatal,
                    Off
                };

                inline std::istream& Cin = std::cin;
                inline std::ostream& Cout = std::cout;
                inline std::ostream& Cerr = std::cerr;

                void SetMinimumLevel(Level MinimumLevel) noexcept;
                Level GetMinimumLevel() noexcept;
                void SetOutput(std::ostream* Output);
                void SetErrorOutput(std::ostream* Output);

                void Write(
                    Level Severity,
                    Containers::StringView Message,
                    const std::source_location& Location = std::source_location::current()
                );

                void Write(
                    Level Severity,
                    const Types::Char* Message,
                    const std::source_location& Location = std::source_location::current()
                );

                void Trace(const Types::Char* Message, const std::source_location& Location = std::source_location::current());
                void Debug(const Types::Char* Message, const std::source_location& Location = std::source_location::current());
                void Info(const Types::Char* Message, const std::source_location& Location = std::source_location::current());
                void Warning(const Types::Char* Message, const std::source_location& Location = std::source_location::current());
                void Error(const Types::Char* Message, const std::source_location& Location = std::source_location::current());
                void Fatal(const Types::Char* Message, const std::source_location& Location = std::source_location::current());
            }
        }
    }
}
