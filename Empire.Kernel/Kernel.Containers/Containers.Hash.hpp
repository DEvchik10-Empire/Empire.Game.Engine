#pragma once

#include "Empire.Kernel/Kernel.Containers/Containers.String.hpp"
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
                constexpr Types::UInt64 MixBits(Types::UInt64 Value) noexcept
                {
                    Value += 0x9E3779B97F4A7C15ULL;
                    Value = (Value ^ (Value >> 30)) * 0xBF58476D1CE4E5B9ULL;
                    Value = (Value ^ (Value >> 27)) * 0x94D049BB133111EBULL;
                    return Value ^ (Value >> 31);
                }

                template <typename T>
                struct Hash;

                template <Types::Integral T>
                struct Hash<T>
                {
                    Types::UInt64 operator()(T Value) const noexcept
                    {
                        return MixBits(static_cast<Types::UInt64>(Value));
                    }
                };

                template <typename T>
                struct Hash<T*>
                {
                    Types::UInt64 operator()(const T* Value) const noexcept
                    {
                        return MixBits(static_cast<Types::UInt64>(reinterpret_cast<Types::UIntPtr>(Value)));
                    }
                };

                template <>
                struct Hash<StringView>
                {
                    Types::UInt64 operator()(StringView Value) const noexcept
                    {
                        Types::UInt64 Result = 14695981039346656037ULL;

                        for (Types::Size Index = 0; Index < Value.GetCount(); Index++)
                        {
                            Result ^= static_cast<Types::UInt64>(static_cast<Types::UInt8>(Value.GetText()[Index]));
                            Result *= 1099511628211ULL;
                        }

                        return MixBits(Result);
                    }
                };

                template <>
                struct Hash<String>
                {
                    Types::UInt64 operator()(const String& Value) const noexcept
                    {
                        return Hash<StringView>{}(Value.GetView());
                    }
                };
            }
        }
    }
}
