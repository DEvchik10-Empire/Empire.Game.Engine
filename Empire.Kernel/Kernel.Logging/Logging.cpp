#include "Empire.Kernel/Kernel.Logging/Logging.hpp"

#include <atomic>
#include <mutex>

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        using namespace EngineKernel::Types;

        namespace EngineKernel
        {
            namespace Logging
            {
                namespace
                {
                    std::atomic<UInt8> MinimumLevelValue(static_cast<UInt8>(Level::Info));
                    std::mutex OutputMutex;
                    std::ostream* StandardOutput = &Cout;
                    std::ostream* ErrorOutput = &Cerr;

                    const Char* GetLevelName(Level Severity)
                    {
                        switch (Severity)
                        {
                            case Level::Trace: return "TRACE";
                            case Level::Debug: return "DEBUG";
                            case Level::Info: return "INFO";
                            case Level::Warning: return "WARNING";
                            case Level::Error: return "ERROR";
                            case Level::Fatal: return "FATAL";
                            case Level::Off: return "OFF";
                        }

                        return "UNKNOWN";
                    }
                }

                void SetMinimumLevel(Level NewMinimumLevel) noexcept
                {
                    MinimumLevelValue.store(static_cast<UInt8>(NewMinimumLevel), std::memory_order_relaxed);
                }

                Level GetMinimumLevel() noexcept
                {
                    return static_cast<Level>(MinimumLevelValue.load(std::memory_order_relaxed));
                }

                void SetOutput(std::ostream* Output)
                {
                    std::lock_guard Lock(OutputMutex);
                    StandardOutput = Output != nullptr ? Output : &Cout;
                }

                void SetErrorOutput(std::ostream* Output)
                {
                    std::lock_guard Lock(OutputMutex);
                    ErrorOutput = Output != nullptr ? Output : &Cerr;
                }

                void Write(Level Severity, Containers::StringView Message, const std::source_location& Location)
                {
                    if (Severity == Level::Off || Severity < GetMinimumLevel())
                        return;

                    std::lock_guard Lock(OutputMutex);
                    std::ostream* Output = Severity >= Level::Warning ? ErrorOutput : StandardOutput;

                    *Output << '[' << GetLevelName(Severity) << "] ";
                    Output->write(Message.GetText(), static_cast<std::streamsize>(Message.GetCount()));
                    *Output << " (" << Location.file_name() << ':' << Location.line() << ")\n";
                    Output->flush();
                }

                void Write(Level Severity, const Char* Message, const std::source_location& Location)
                {
                    Write(Severity, Containers::StringView(Message), Location);
                }

                void Trace(const Char* Message, const std::source_location& Location) { Write(Level::Trace, Message, Location); }
                void Debug(const Char* Message, const std::source_location& Location) { Write(Level::Debug, Message, Location); }
                void Info(const Char* Message, const std::source_location& Location) { Write(Level::Info, Message, Location); }
                void Warning(const Char* Message, const std::source_location& Location) { Write(Level::Warning, Message, Location); }
                void Error(const Char* Message, const std::source_location& Location) { Write(Level::Error, Message, Location); }
                void Fatal(const Char* Message, const std::source_location& Location) { Write(Level::Fatal, Message, Location); }
            }
        }
    }
}
