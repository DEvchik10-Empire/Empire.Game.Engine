#pragma once

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EngineKernel
        {
            namespace Types
            {
                using Int8 = signed char;
                using Int16 = short;
                using Int32 = int;
                using Int64 = long long;

                using UInt8 = unsigned char;
                using UInt16 = unsigned short;
                using UInt32 = unsigned int;
                using UInt64 = unsigned long long;

                using Float = float;
                using Double = double;

                using Size = decltype(sizeof(0));
                using SignedSize = decltype(static_cast<char*>(nullptr) - static_cast<char*>(nullptr));
                using UIntPtr = Size;
                using NullPointer = decltype(nullptr);

                enum class Byte : UInt8
                {
                };

                using Char = char;
                using Char16 = char16_t;
                using Char32 = char32_t;
                using Bool = bool;

                static_assert(sizeof(Int8) == 1 && sizeof(UInt8) == 1, "Int8 must be 1 byte");
                static_assert(sizeof(Int16) == 2 && sizeof(UInt16) == 2, "Int16 must be 2 bytes");
                static_assert(sizeof(Int32) == 4 && sizeof(UInt32) == 4, "Int32 must be 4 bytes");
                static_assert(sizeof(Int64) == 8 && sizeof(UInt64) == 8, "Int64 must be 8 bytes");
                static_assert(sizeof(Float) == 4 && sizeof(Double) == 8, "Float must be 4 bytes and Double 8 bytes");
                static_assert(sizeof(Size) == sizeof(void*), "Size must match the pointer width");

                class SourceLocation
                {
                    private:
                        const Char* FileName = "";
                        const Char* FunctionName = "";
                        UInt32 LineNumber = 0;

                    public:
                        constexpr SourceLocation() noexcept = default;

                        constexpr SourceLocation(const Char* File, const Char* Function, UInt32 Line) noexcept
                            : FileName(File), FunctionName(Function), LineNumber(Line)
                        {
                        }

                        static constexpr SourceLocation Current(
                            const Char* File = __builtin_FILE(),
                            const Char* Function = __builtin_FUNCTION(),
                            UInt32 Line = static_cast<UInt32>(__builtin_LINE())
                        ) noexcept
                        {
                            return SourceLocation(File, Function, Line);
                        }

                        constexpr const Char* GetFile() const noexcept { return FileName; }
                        constexpr const Char* GetFunction() const noexcept { return FunctionName; }
                        constexpr UInt32 GetLine() const noexcept { return LineNumber; }
                };
            }
        }
    }
}
