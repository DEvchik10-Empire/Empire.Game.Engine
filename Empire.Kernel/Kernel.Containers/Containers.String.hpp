#pragma once

#include "Empire.Kernel/Kernel.Types/Types.hpp"

#include <charconv>
#include <stdexcept>
#include <system_error>
#include <type_traits>

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

                    public:
                        String() = default;
                        String(const Types::Char* Source);
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

                        Types::Char& operator[](Types::Size Index);
                        const Types::Char& operator[](Types::Size Index) const;

                        void Append(const Types::Char* Source);
                        void Append(const String& Source);
                        void Clear() noexcept;
                };

                template <typename T>
                String ToString(T Value)
                {
                    static_assert(
                        std::is_arithmetic_v<T>,
                        "ToString supports arithmetic types"
                    );

                    if constexpr (std::is_same_v<T, Types::Bool>)
                    {
                        return String(Value ? "true" : "false");
                    }
                    else
                    {
                        Types::Char Buffer[128]{};
                        const auto Result = std::to_chars(Buffer, Buffer + sizeof(Buffer) - 1, Value);

                        if (Result.ec != std::errc{})
                            throw std::runtime_error("Failed to convert value to String");

                        *Result.ptr = '\0';
                        return String(Buffer);
                    }
                }
            }
        }
    }
}
