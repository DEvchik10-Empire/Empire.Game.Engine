#include "Empire.Kernel/Kernel.Assert/Assert.hpp"

#include "Empire.Kernel/Kernel.System/System.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        using namespace EmpireKernel::KernelTypes;

        namespace EmpireKernel
        {
            namespace KernelAssert
            {
                namespace
                {
                    void WriteToStandardError(const Char* Text, Size ByteCount) noexcept
                    {
                        KernelSystem::WriteOutput(true, Text, ByteCount);
                    }

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

                            void Flush() noexcept
                            {
                                WriteToStandardError(Data, Length);
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
