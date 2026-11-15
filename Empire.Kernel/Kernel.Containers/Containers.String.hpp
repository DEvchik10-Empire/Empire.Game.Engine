#pragma once

#include "Empire.Kernel/Kernel.Containers/Containers.StringView.hpp"
#include "Empire.Kernel/Kernel.Types/Types.Utility.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EngineKernel
        {
            namespace Containers
            {
                class String
                {
                    private:
                        Types::Char* Text = nullptr;
                        Types::Size Count = 0;
                        Types::Size Capacity = 0;

                        void Reserve(Types::Size NewCapacity);
                        void AppendRange(const Types::Char* Source, Types::Size SourceCount);

                    public:
                        String() = default;
                        String(const Types::Char* Source);
                        String(StringView Source);
                        String(const String& Other);
                        String(String&& Other) noexcept;
                        ~String();

                        String& operator=(const String& Other);
                        String& operator=(String&& Other) noexcept;

                        Types::Size GetCount() const noexcept;
                        Types::Size GetCapacity() const noexcept;
                        Types::Bool IsEmpty() const noexcept;
                        const Types::Char* GetText() const noexcept;
                        Types::Char* GetData() noexcept;
                        StringView GetView() const noexcept;

                        Types::Char& operator[](Types::Size Index);
                        const Types::Char& operator[](Types::Size Index) const;

                        void Append(const Types::Char* Source);
                        void Append(StringView Source);
                        void Append(const String& Source);
                        void Append(Types::Char Character);
                        void Clear() noexcept;

                        String& operator+=(const Types::Char* Source);
                        String& operator+=(const String& Source);
                        String& operator+=(Types::Char Character);
                };

                String operator+(const String& Left, const String& Right);
                String operator+(const String& Left, const Types::Char* Right);
                String operator+(const Types::Char* Left, const String& Right);

                Types::Bool operator==(const String& Left, const String& Right) noexcept;
                Types::Bool operator!=(const String& Left, const String& Right) noexcept;

                String FormatSigned(Types::Int64 Value);
                String FormatUnsigned(Types::UInt64 Value);
                String FormatFloating(Types::Double Value);

                template <typename T>
                    requires (Types::Integral<T> || Types::FloatingPoint<T>)
                String ToString(T Value)
                {
                    if constexpr (Types::IsSame<Types::RemoveCV<T>, Types::Bool>)
                        return String(Value ? "true" : "false");
                    else if constexpr (Types::FloatingPoint<T>)
                        return FormatFloating(static_cast<Types::Double>(Value));
                    else if constexpr (Types::IsSigned<T>)
                        return FormatSigned(static_cast<Types::Int64>(Value));
                    else
                        return FormatUnsigned(static_cast<Types::UInt64>(Value));
                }
            }
        }
    }
}
