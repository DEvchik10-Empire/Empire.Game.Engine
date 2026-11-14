#include "Empire.Kernel/Kernel.Logging/Logging.hpp"

#include "Empire.Kernel/Kernel.Containers/Containers.String.hpp"
#include "Empire.Kernel/Kernel.Sync/Sync.Atomic.hpp"
#include "Empire.Kernel/Kernel.Sync/Sync.Mutex.hpp"

#include "Empire.Kernel/Kernel.System/System.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        using namespace EmpireKernel::KernelTypes;

        namespace EmpireKernel
        {
            namespace KernelLogging
            {
                namespace
                {
                    KernelSync::Atomic<UInt8> MinimumLevelValue(static_cast<UInt8>(Level::Info));
                    KernelSync::Mutex OutputMutex;
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
                        KernelSystem::WriteOutput(ToStandardError, Text, ByteCount);
                    }

                    // Укорачивает путь файла: после последнего "Empire.Game.Engine/", а без него только имя файла.
                    KernelContainers::StringView ShortenPath(const Char* Path) noexcept
                    {
                        constexpr const Char Marker[] = "Empire.Game.Engine/";
                        constexpr Size MarkerLength = sizeof(Marker) - 1;

                        const Char* Start = Path;
                        const Char* Name = Path;

                        for (const Char* Cursor = Path; *Cursor != '\0'; Cursor++)
                        {
                            if (*Cursor == '/' || *Cursor == '\\')
                                Name = Cursor + 1;

                            Size Matched = 0;

                            while (Matched < MarkerLength && Cursor[Matched] != '\0' && Cursor[Matched] == Marker[Matched])
                                Matched++;

                            if (Matched == MarkerLength)
                                Start = Cursor + MarkerLength;
                        }

                        return KernelContainers::StringView(Start != Path ? Start : Name);
                    }

                    Sink& GetActiveSink()
                    {
                        static ConsoleSink DefaultSink;
                        return CurrentSink != nullptr ? *CurrentSink : DefaultSink;
                    }
                }

                void ConsoleSink::Write(Level Severity, KernelContainers::StringView Line)
                {
                    WriteToStream(Severity >= Level::Warning, Line.GetText(), Line.GetCount());
                }

                void SetMinimumLevel(Level NewMinimumLevel) noexcept
                {
                    MinimumLevelValue.Store(static_cast<UInt8>(NewMinimumLevel), KernelSync::MemoryOrder::Relaxed);
                }

                Level GetMinimumLevel() noexcept
                {
                    return static_cast<Level>(MinimumLevelValue.Load(KernelSync::MemoryOrder::Relaxed));
                }

                void SetSink(Sink* NewSink)
                {
                    KernelSync::LockGuard Lock(OutputMutex);
                    CurrentSink = NewSink;
                }

                void Write(Level Severity, KernelContainers::StringView Message, const SourceLocation& Location)
                {
                    if (Severity == Level::Off || Severity < GetMinimumLevel())
                        return;

                    KernelContainers::String Line;

                    Line.Append("[");
                    Line.Append(GetLevelName(Severity));
                    Line.Append("] ");
                    Line.Append(Message);
                    Line.Append(" (");
                    Line.Append(ShortenPath(Location.GetFile()));
                    Line.Append(":");
                    Line.Append(KernelContainers::FormatUnsigned(Location.GetLine()));
                    Line.Append(")\n");

                    KernelSync::LockGuard Lock(OutputMutex);
                    GetActiveSink().Write(Severity, Line.GetView());
                }

                void Write(Level Severity, const Char* Message, const SourceLocation& Location)
                {
                    Write(Severity, KernelContainers::StringView(Message), Location);
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
