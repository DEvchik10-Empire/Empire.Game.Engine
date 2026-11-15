#pragma once

#include "Empire.Kernel/Kernel.Containers/Containers.StringView.hpp"
#include "Empire.Kernel/Kernel.Types/Types.hpp"

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

                // Приёмник записей. Вызывается под внутренним мьютексом логгера, поэтому Write не должен вызывать Logging.
                class Sink
                {
                    public:
                        virtual ~Sink() = default;

                        virtual void Write(Level Severity, Containers::StringView Line) = 0;
                };

                // Приёмник по умолчанию: Warning и выше идут в stderr, остальное в stdout.
                class ConsoleSink final : public Sink
                {
                    public:
                        void Write(Level Severity, Containers::StringView Line) override;
                };

                void SetMinimumLevel(Level MinimumLevel) noexcept;
                Level GetMinimumLevel() noexcept;

                // nullptr возвращает приёмник по умолчанию. Логгер не владеет переданным приёмником.
                void SetSink(Sink* NewSink);

                void Write(
                    Level Severity,
                    Containers::StringView Message,
                    const Types::SourceLocation& Location = Types::SourceLocation::Current()
                );

                void Write(
                    Level Severity,
                    const Types::Char* Message,
                    const Types::SourceLocation& Location = Types::SourceLocation::Current()
                );

                void Trace(const Types::Char* Message, const Types::SourceLocation& Location = Types::SourceLocation::Current());
                void Debug(const Types::Char* Message, const Types::SourceLocation& Location = Types::SourceLocation::Current());
                void Info(const Types::Char* Message, const Types::SourceLocation& Location = Types::SourceLocation::Current());
                void Warning(const Types::Char* Message, const Types::SourceLocation& Location = Types::SourceLocation::Current());
                void Error(const Types::Char* Message, const Types::SourceLocation& Location = Types::SourceLocation::Current());
                void Fatal(const Types::Char* Message, const Types::SourceLocation& Location = Types::SourceLocation::Current());
            }
        }
    }
}
