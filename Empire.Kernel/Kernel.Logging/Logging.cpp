#include "Empire.Kernel/Kernel.Logging/Logging.hpp"

#include "Empire.Kernel/Kernel.Containers/Containers.String.hpp"
#include "Empire.Kernel/Kernel.Sync/Sync.Atomic.hpp"
#include "Empire.Kernel/Kernel.Sync/Sync.Mutex.hpp"

#include "Empire.Kernel/Kernel.System/System.hpp"

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
                    Sync::Atomic<UInt8> MinimumLevelValue(static_cast<UInt8>(Level::Info));
                    Sync::Mutex OutputMutex;
                    Sink* CurrentSink = nullptr;

                    const Char* GetLevelName(Level Severity) noexcept
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

                    void WriteToStream(Bool ToStandardError, const Char* Text, Size ByteCount)
                    {
                        System::WriteOutput(ToStandardError, Text, ByteCount);
                    }

                    Sink& GetActiveSink()
                    {
                        static ConsoleSink DefaultSink;
                        return CurrentSink != nullptr ? *CurrentSink : DefaultSink;
                    }
                }

                void ConsoleSink::Write(Level Severity, Containers::StringView Line)
                {
                    WriteToStream(Severity >= Level::Warning, Line.GetText(), Line.GetCount());
                }

                void SetMinimumLevel(Level NewMinimumLevel) noexcept
                {
                    MinimumLevelValue.Store(static_cast<UInt8>(NewMinimumLevel), Sync::MemoryOrder::Relaxed);
                }

                Level GetMinimumLevel() noexcept
                {
                    return static_cast<Level>(MinimumLevelValue.Load(Sync::MemoryOrder::Relaxed));
                }

                void SetSink(Sink* NewSink)
                {
                    Sync::LockGuard Lock(OutputMutex);
                    CurrentSink = NewSink;
                }

                void Write(Level Severity, Containers::StringView Message, const SourceLocation& Location)
                {
                    if (Severity == Level::Off || Severity < GetMinimumLevel())
                        return;

                    Containers::String Line;

                    Line.Append("[");
                    Line.Append(GetLevelName(Severity));
                    Line.Append("] ");
                    Line.Append(Message);
                    Line.Append(" (");
                    Line.Append(Location.GetFile());
                    Line.Append(":");
                    Line.Append(Containers::FormatUnsigned(Location.GetLine()));
                    Line.Append(")\n");

                    Sync::LockGuard Lock(OutputMutex);
                    GetActiveSink().Write(Severity, Line.GetView());
                }

                void Write(Level Severity, const Char* Message, const SourceLocation& Location)
                {
                    Write(Severity, Containers::StringView(Message), Location);
                }

                void Trace(const Char* Message, const SourceLocation& Location) { Write(Level::Trace, Message, Location); }
                void Debug(const Char* Message, const SourceLocation& Location) { Write(Level::Debug, Message, Location); }
                void Info(const Char* Message, const SourceLocation& Location) { Write(Level::Info, Message, Location); }
                void Warning(const Char* Message, const SourceLocation& Location) { Write(Level::Warning, Message, Location); }
                void Error(const Char* Message, const SourceLocation& Location) { Write(Level::Error, Message, Location); }
                void Fatal(const Char* Message, const SourceLocation& Location) { Write(Level::Fatal, Message, Location); }
            }
        }
    }
}
