#pragma once

#include "Empire.Engine/Engine.Kernel/Kernel.Containers/Containers.String.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Containers/Containers.StringView.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Memory/Memory.Reference.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Types/Types.Utility.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireEngine
        {
            namespace EngineKernel
            {
                namespace KernelContainers
                {
                    constexpr KernelTypes::UInt64 MixBits(KernelTypes::UInt64 Value) noexcept
                    {
                        Value += 0x9E3779B97F4A7C15ULL;
                        Value = (Value ^ (Value >> 30)) * 0xBF58476D1CE4E5B9ULL;
                        Value = (Value ^ (Value >> 27)) * 0x94D049BB133111EBULL;
                        return Value ^ (Value >> 31);
                    }

                    // Хеш FNV-1a по байтам (перенесён из Jolt, Core/HashCombine.h).
                    // Seed можно передать хеш предыдущей операции, чтобы продолжить цепочку.
                    // https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
                    inline KernelTypes::UInt64 HashBytes(
                        const void* Data,
                        KernelTypes::UInt32 ByteCount,
                        KernelTypes::UInt64 Seed = 0xcbf29ce484222325ULL
                    ) noexcept
                    {
                        KernelTypes::UInt64 Result = Seed;
                        const KernelTypes::UInt8* Bytes = static_cast<const KernelTypes::UInt8*>(Data);

                        for (const KernelTypes::UInt8* Current = Bytes; Current < Bytes + ByteCount; ++Current)
                        {
                            Result ^= static_cast<KernelTypes::UInt64>(*Current);
                            Result *= 0x100000001b3ULL;
                        }

                        return Result;
                    }

                    // Хеш FNV-1a строки с нулевым концом. Работает на этапе компиляции.
                    constexpr KernelTypes::UInt64 HashString(
                        const KernelTypes::Char* Text,
                        KernelTypes::UInt64 Seed = 0xcbf29ce484222325ULL
                    ) noexcept
                    {
                        KernelTypes::UInt64 Result = Seed;

                        for (const KernelTypes::Char* Current = Text; *Current != 0; ++Current)
                        {
                            Result ^= static_cast<KernelTypes::UInt64>(*Current);
                            Result *= 0x100000001b3ULL;
                        }

                        return Result;
                    }

                    // 64-битная хеш-функция Томаса Вана (1997)
                    // http://web.archive.org/web/20071223173210/http://www.concentric.net/~Ttwang/tech/inthash.htm
                    constexpr KernelTypes::UInt64 Hash64(KernelTypes::UInt64 Value) noexcept
                    {
                        KernelTypes::UInt64 Result = Value;
                        Result = (~Result) + (Result << 21);
                        Result = Result ^ (Result >> 24);
                        Result = (Result + (Result << 3)) + (Result << 8);
                        Result = Result ^ (Result >> 14);
                        Result = (Result + (Result << 2)) + (Result << 4);
                        Result = Result ^ (Result >> 28);
                        Result = Result + (Result << 31);
                        return Result;
                    }

                    template <typename T>
                    struct Hash;

                    template <KernelTypes::Integral T>
                    struct Hash<T>
                    {
                        KernelTypes::UInt64 operator()(T Value) const noexcept
                        {
                            return MixBits(static_cast<KernelTypes::UInt64>(Value));
                        }
                    };

                    // Тип сам умеет считать свой хеш: T::GetHash() (так у FastArray и StaticArray)
                    template <typename T>
                    concept HasGetHash = requires(const T& Value) { static_cast<KernelTypes::UInt64>(Value.GetHash()); };

                    template <HasGetHash T>
                    struct Hash<T>
                    {
                        KernelTypes::UInt64 operator()(const T& Value) const noexcept(noexcept(Value.GetHash()))
                        {
                            return Value.GetHash();
                        }
                    };

                    // Минус ноль и ноль должны давать один хеш, потому что они равны
                    template <>
                    struct Hash<float>
                    {
                        KernelTypes::UInt64 operator()(float Value) const noexcept
                        {
                            const float Normalized = Value == 0.0f ? 0.0f : Value;
                            return HashBytes(&Normalized, sizeof(Normalized));
                        }
                    };

                    template <>
                    struct Hash<double>
                    {
                        KernelTypes::UInt64 operator()(double Value) const noexcept
                        {
                            const double Normalized = Value == 0.0 ? 0.0 : Value;
                            return HashBytes(&Normalized, sizeof(Normalized));
                        }
                    };

                    template <typename T>
                    struct Hash<T*>
                    {
                        KernelTypes::UInt64 operator()(const T* Value) const noexcept
                        {
                            return MixBits(static_cast<KernelTypes::UInt64>(reinterpret_cast<KernelTypes::UIntPtr>(Value)));
                        }
                    };

                    template <>
                    struct Hash<StringView>
                    {
                        KernelTypes::UInt64 operator()(StringView Value) const noexcept
                        {
                            KernelTypes::UInt64 Result = 14695981039346656037ULL;

                            for (KernelTypes::Size Index = 0; Index < Value.GetCount(); Index++)
                            {
                                Result ^= static_cast<KernelTypes::UInt64>(static_cast<KernelTypes::UInt8>(Value.GetText()[Index]));
                                Result *= 1099511628211ULL;
                            }

                            return MixBits(Result);
                        }
                    };

                    template <>
                    struct Hash<String>
                    {
                        KernelTypes::UInt64 operator()(const String& Value) const noexcept
                        {
                            return Hash<StringView>{}(Value.GetView());
                        }
                    };
                    // Подмешивает хеш одного значения в Seed. Основа: mx3, https://github.com/jonmaiga/mx3 (Jon Maiga).
                    template <typename T>
                    inline void HashCombine(KernelTypes::UInt64& Seed, const T& Value) noexcept(noexcept(Hash<T>{}(Value)))
                    {
                        constexpr KernelTypes::UInt64 Multiplier = 0xbea225f9eb34556dULL;

                        KernelTypes::UInt64 Mixed = Seed;
                        KernelTypes::UInt64 Item = Hash<T>{}(Value);

                        // mix_stream(Mixed, Item)
                        Item *= Multiplier;
                        Item ^= Item >> 39;
                        Mixed += Item * Multiplier;
                        Mixed *= Multiplier;

                        // mix(Mixed)
                        Mixed ^= Mixed >> 32;
                        Mixed *= Multiplier;
                        Mixed ^= Mixed >> 29;
                        Mixed *= Multiplier;
                        Mixed ^= Mixed >> 32;
                        Mixed *= Multiplier;
                        Mixed ^= Mixed >> 29;

                        Seed = Mixed;
                    }

                    // Хеш нескольких значений сразу. Для своей структуры:
                    //
                    //     template <> struct Hash<SomeKey>
                    //     {
                    //         KernelTypes::UInt64 operator()(const SomeKey& Key) const
                    //         {
                    //             return HashCombineArgs(Key.First, Key.Second, Key.Flag);
                    //         }
                    //     };
                    //
                    // (в Jolt для этого был макрос JPH_MAKE_HASHABLE, здесь он не нужен)
                    template <typename FirstValue, typename... Values>
                    inline KernelTypes::UInt64 HashCombineArgs(const FirstValue& First, const Values&... Rest) noexcept(noexcept(Hash<FirstValue>{}(First)))
                    {
                        // Начальное значение это хеш первого элемента
                        KernelTypes::UInt64 Seed = Hash<FirstValue>{}(First);

                        // Остальные подмешиваются по очереди
                        (HashCombine(Seed, Rest), ...);

                        return Seed;
                    }

                    // Ref и RefConst хешируются по адресу объекта
                    template <typename T>
                    struct Hash<KernelMemory::Ref<T>>
                    {
                        KernelTypes::UInt64 operator()(const KernelMemory::Ref<T>& Value) const noexcept
                        {
                            return Hash<T*>{}(Value.GetPtr());
                        }
                    };

                    template <typename T>
                    struct Hash<KernelMemory::RefConst<T>>
                    {
                        KernelTypes::UInt64 operator()(const KernelMemory::RefConst<T>& Value) const noexcept
                        {
                            return Hash<const T*>{}(Value.GetPtr());
                        }
                    };
                }
            }
        }
    }
}
