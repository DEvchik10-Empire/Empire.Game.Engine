#include "Empire.Kernel/Kernel.Assert/Assert.hpp"

#include <cstdlib>

#if defined(_WIN32)
    #if !defined(WIN32_LEAN_AND_MEAN)
        #define WIN32_LEAN_AND_MEAN
    #endif
    #if !defined(NOMINMAX)
        #define NOMINMAX
    #endif
    #include <windows.h>
#else
    #include <unistd.h>
#endif

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        using namespace EngineKernel::Types;

        namespace EngineKernel
        {
            namespace Assert
            {
                namespace
                {
                    void WriteToStandardError(const Char* Text, Size ByteCount) noexcept
                    {
                #if defined(_WIN32)
                        HANDLE Handle = GetStdHandle(STD_ERROR_HANDLE);
                        DWORD Written = 0;
                        WriteFile(Handle, Text, static_cast<DWORD>(ByteCount), &Written, nullptr);
                #else
                        while (ByteCount > 0)
                        {
                            const auto Result = write(2, Text, ByteCount);

                            if (Result <= 0)
                                return;

                            Text += Result;
                            ByteCount -= static_cast<Size>(Result);
                        }
                #endif
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

                    std::abort();
                }
            }
        }
    }
}
