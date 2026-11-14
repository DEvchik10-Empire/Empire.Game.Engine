#pragma once

#include "Empire.Kernel/Kernel.Containers/Containers.StringView.hpp"
#include "Empire.Kernel/Kernel.Types/Types.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireKernel
        {
            namespace KernelLogging
            {
                enum class Level : KernelTypes::UInt8
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
                // Line это готовая строка вида "[INFO] текст (путь/файл.cpp:12)\n" с завершающим переводом строки;
                // путь укорочен: часть после последнего "Empire.Game.Engine/", а если её нет, то только имя файла.
                class Sink
                {
                    public:
                        virtual ~Sink() = default;

                        virtual void Write(Level Severity, KernelContainers::StringView Line) = 0;
                };

                // Приёмник по умолчанию: Warning и выше идут в stderr, остальное в stdout.
                class ConsoleSink final : public Sink
                {
                    public:
                        void Write(Level Severity, KernelContainers::StringView Line) override;
                };

                void SetMinimumLevel(Level MinimumLevel) noexcept;
                Level GetMinimumLevel() noexcept;

                // nullptr возвращает приёмник по умолчанию. Логгер не владеет переданным приёмником.
                void SetSink(Sink* NewSink);

                void Write(
                    Level Severity,
                    KernelContainers::StringView Message,
                    const KernelTypes::SourceLocation& Location = KernelTypes::SourceLocation::Current()
                );

                void Write(
                    Level Severity,
                    const KernelTypes::Char* Message,
                    const KernelTypes::SourceLocation& Location = KernelTypes::SourceLocation::Current()
                );

                void Trace(const KernelTypes::Char* Message, const KernelTypes::SourceLocation& Location = KernelTypes::SourceLocation::Current());
                void Debug(const KernelTypes::Char* Message, const KernelTypes::SourceLocation& Location = KernelTypes::SourceLocation::Current());
                void Info(const KernelTypes::Char* Message, const KernelTypes::SourceLocation& Location = KernelTypes::SourceLocation::Current());
                void Warning(const KernelTypes::Char* Message, const KernelTypes::SourceLocation& Location = KernelTypes::SourceLocation::Current());
                void Error(const KernelTypes::Char* Message, const KernelTypes::SourceLocation& Location = KernelTypes::SourceLocation::Current());
                void Fatal(const KernelTypes::Char* Message, const KernelTypes::SourceLocation& Location = KernelTypes::SourceLocation::Current());
            }
        }
    }
}
