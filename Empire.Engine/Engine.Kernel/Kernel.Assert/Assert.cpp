#include "Empire.Engine/Engine.Kernel/Kernel.Assert/Assert.hpp"

#include "Empire.Engine/Engine.Kernel/Kernel.System/System.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        using namespace EmpireEngine::EngineKernel::KernelTypes;

        namespace EmpireEngine
        {
            namespace EngineKernel
            {
                namespace KernelAssert
                {
                    namespace
                    {
                        TraceFunction CurrentTraceFunction = nullptr;
                        AssertFailedFunction CurrentAssertFailedFunction = nullptr;

                        class MessageBuffer
                        {
                            private:
                                Char Data[1024];
                                Size Length = 0;

                            public:
                                void Append(const Char* Text) noexcept
                                {
                                    if (Text == nullptr)
                                        return;

                                    for (Size Index = 0; Text[Index] != '\0' && Length + 1 < sizeof(Data); Index++)
                                        Data[Length++] = Text[Index];
                                }

                                void AppendNumber(UInt32 Value) noexcept
                                {
                                    Char Digits[16];
                                    Size Position = sizeof(Digits);

                                    do
                                    {
                                        Digits[--Position] = static_cast<Char>('0' + Value % 10);
                                        Value /= 10;
                                    }
                                    while (Value != 0);

                                    for (; Position < sizeof(Digits) && Length + 1 < sizeof(Data); Position++)
                                        Data[Length++] = Digits[Position];
                                }

                                void Flush(Bool ToStandardError = true) noexcept
                                {
                                    KernelSystem::WriteOutput(ToStandardError, Data, Length);
                                }
                        };
                    }

                    Error::Error(const Char* Message, const SourceLocation& Location) noexcept
                        : LocationValue(Location)
                    {
                        Size Length = 0;

                        if (Message != nullptr)
                        {
                            for (; Message[Length] != '\0' && Length + 1 < sizeof(MessageText); Length++)
                                MessageText[Length] = Message[Length];
                        }

                        MessageText[Length] = '\0';
                    }

                    const Char* Error::GetKind() const noexcept
                    {
                        return "Error";
                    }

                    const Char* Error::GetText() const noexcept
                    {
                        return MessageText;
                    }

                    const SourceLocation& Error::GetLocation() const noexcept
                    {
                        return LocationValue;
                    }

                    const Char* LogicError::GetKind() const noexcept
                    {
                        return "LogicError";
                    }

                    const Char* InvalidArgumentError::GetKind() const noexcept
                    {
                        return "InvalidArgumentError";
                    }

                    const Char* OutOfRangeError::GetKind() const noexcept
                    {
                        return "OutOfRangeError";
                    }

                    const Char* LengthError::GetKind() const noexcept
                    {
                        return "LengthError";
                    }

                    const Char* OverflowError::GetKind() const noexcept
                    {
                        return "OverflowError";
                    }

                    const Char* AllocationError::GetKind() const noexcept
                    {
                        return "AllocationError";
                    }

                    void SetTraceFunction(TraceFunction Callback) noexcept
                    {
                        CurrentTraceFunction = Callback;
                    }

                    void SetAssertFailedFunction(AssertFailedFunction Callback) noexcept
                    {
                        CurrentAssertFailedFunction = Callback;
                    }

                    void Trace(const Char* Message, const SourceLocation& Location)
                    {
                        TraceFunction Callback = CurrentTraceFunction;
                        if (Callback != nullptr)
                        {
                            Callback(Message, Location);
                            return;
                        }

                        MessageBuffer Buffer;
                        Buffer.Append("[TRACE] ");
                        Buffer.Append(Message);
                        Buffer.Append("\n");
                        Buffer.Flush(false);
                    }

                    Bool ReportAssertFailure(
                        const Char* Expression,
                        const Char* Message,
                        const SourceLocation& Location
                    )
                    {
                        AssertFailedFunction Callback = CurrentAssertFailedFunction;
                        if (Callback != nullptr)
                            return Callback(Expression, Message, Location);

                        MessageBuffer Buffer;
                        Buffer.Append("Assertion failed: ");
                        Buffer.Append(Expression);
                        if (Message != nullptr && Message[0] != '\0')
                        {
                            Buffer.Append(" - ");
                            Buffer.Append(Message);
                        }
                        Buffer.Append("\nFile: ");
                        Buffer.Append(Location.GetFile());
                        Buffer.Append("\nFunction: ");
                        Buffer.Append(Location.GetFunction());
                        Buffer.Append("\nLine: ");
                        Buffer.AppendNumber(Location.GetLine());
                        Buffer.Append("\n");
                        Buffer.Flush();
                        return true;
                    }

                    void Check(Bool Condition, const Char* Message, const SourceLocation& Location)
                    {
                        if (!Condition)
                            Fail(Message, Location);
                    }

                    [[noreturn]] void Fail(const Char* Message, const SourceLocation& Location)
                    {
                        MessageBuffer Buffer;

                        Buffer.Append("Empire assertion failed: ");
                        Buffer.Append(Message != nullptr ? Message : "No message");
                        Buffer.Append("\nFile: ");
                        Buffer.Append(Location.GetFile());
                        Buffer.Append("\nFunction: ");
                        Buffer.Append(Location.GetFunction());
                        Buffer.Append("\nLine: ");
                        Buffer.AppendNumber(Location.GetLine());
                        Buffer.Append("\n");
                        Buffer.Flush();

                        KernelSystem::Terminate();
                    }
                }
            }
        }
    }
}
