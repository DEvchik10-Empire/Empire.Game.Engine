#pragma once

#include "Empire.Kernel/Kernel.Assert/Assert.hpp"
#include "Empire.Kernel/Kernel.Containers/Containers.Hash.hpp"
#include "Empire.Kernel/Kernel.Memory/Memory.hpp"
#include "Empire.Kernel/Kernel.Types/Types.Utility.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EngineKernel
        {
            namespace Containers
            {
                // Открытая адресация, линейный пробинг, ёмкость всегда степень двойки.
                // Указатели на значения (Find) инвалидируются при любой вставке, которая вызывает рост таблицы.
                // Ключи и значения должны иметь noexcept move-конструктор, иначе перестройка таблицы небезопасна.
                template <typename K, typename V, typename HasherType = Hash<K>>
                class HashMap
                {
                    static_assert(
                        Types::IsNothrowMoveConstructible<K> && Types::IsNothrowMoveConstructible<V>,
                        "HashMap requires nothrow move constructible keys and values"
                    );

                    private:
                        static constexpr Types::UInt8 EmptyState = 0;
                        static constexpr Types::UInt8 UsedState = 1;
                        static constexpr Types::UInt8 DeletedState = 2;
                        static constexpr Types::Size NotFound = Types::MaxValue<Types::Size>();

                        Types::UInt8* States = nullptr;
                        K* Keys = nullptr;
                        V* Values = nullptr;
                        Types::Size Count = 0;
                        Types::Size Tombstones = 0;
                        Types::Size Capacity = 0;
                        HasherType Hasher;

                        void ReleaseStorage() noexcept
                        {
                            Memory::Deallocate(States);
                            Memory::Deallocate(Keys);
                            Memory::Deallocate(Values);

                            States = nullptr;
                            Keys = nullptr;
                            Values = nullptr;
                            Capacity = 0;
                        }

                        void Rehash(Types::Size NewCapacity)
                        {
                            if (NewCapacity > Types::MaxValue<Types::Size>() / (sizeof(K) + sizeof(V) + 1))
                                throw Assert::LengthError("HashMap capacity is too large");

                            Types::UInt8* NewStates = nullptr;
                            K* NewKeys = nullptr;
                            V* NewValues = nullptr;

                            try
                            {
                                NewStates = static_cast<Types::UInt8*>(Memory::Allocate(NewCapacity));
                                Memory::FillBytes(NewStates, EmptyState, NewCapacity);
                                NewKeys = static_cast<K*>(Memory::Allocate(NewCapacity * sizeof(K), alignof(K)));
                                NewValues = static_cast<V*>(Memory::Allocate(NewCapacity * sizeof(V), alignof(V)));
                            }
                            catch (...)
                            {
                                Memory::Deallocate(NewStates);
                                Memory::Deallocate(NewKeys);
                                Memory::Deallocate(NewValues);
                                throw;
                            }

                            const Types::Size Mask = NewCapacity - 1;

                            for (Types::Size Old = 0; Old < Capacity; Old++)
                            {
                                if (States[Old] != UsedState)
                                    continue;

                                Types::Size Index = static_cast<Types::Size>(Hasher(Keys[Old])) & Mask;

                                while (NewStates[Index] == UsedState)
                                    Index = (Index + 1) & Mask;

                                Types::ConstructAt(NewKeys + Index, Types::Move(Keys[Old]));
                                Types::ConstructAt(NewValues + Index, Types::Move(Values[Old]));
                                NewStates[Index] = UsedState;

                                Types::DestroyAt(Keys + Old);
                                Types::DestroyAt(Values + Old);
                            }

                            ReleaseStorage();

                            States = NewStates;
                            Keys = NewKeys;
                            Values = NewValues;
                            Capacity = NewCapacity;
                            Tombstones = 0;
                        }

                        void GrowIfNeeded()
                        {
                            if (Capacity == 0)
                            {
                                Rehash(8);
                                return;
                            }

                            if ((Count + Tombstones + 1) * 10 <= Capacity * 7)
                                return;

                            Types::Size NewCapacity = Capacity;

                            if ((Count + 1) * 10 > Capacity * 4)
                            {
                                if (Capacity > Types::MaxValue<Types::Size>() / 4)
                                    throw Assert::LengthError("HashMap capacity is too large");

                                NewCapacity = Capacity * 2;
                            }

                            Rehash(NewCapacity);
                        }

                        Types::Size FindIndex(const K& Key) const
                        {
                            if (Capacity == 0)
                                return NotFound;

                            const Types::Size Mask = Capacity - 1;
                            Types::Size Index = static_cast<Types::Size>(Hasher(Key)) & Mask;

                            for (Types::Size Probe = 0; Probe < Capacity; Probe++)
                            {
                                if (States[Index] == EmptyState)
                                    return NotFound;

                                if (States[Index] == UsedState && Keys[Index] == Key)
                                    return Index;

                                Index = (Index + 1) & Mask;
                            }

                            return NotFound;
                        }

                        Types::Bool Place(const K& Key, V&& Value, Types::Bool Overwrite)
                        {
                            const Types::Size Existing = FindIndex(Key);

                            if (Existing != NotFound)
                            {
                                if (Overwrite)
                                    Values[Existing] = Types::Move(Value);

                                return false;
                            }

                            GrowIfNeeded();

                            const Types::Size Mask = Capacity - 1;
                            Types::Size Index = static_cast<Types::Size>(Hasher(Key)) & Mask;

                            while (States[Index] == UsedState)
                                Index = (Index + 1) & Mask;

                            Types::ConstructAt(Keys + Index, Key);

                            try
                            {
                                Types::ConstructAt(Values + Index, Types::Move(Value));
                            }
                            catch (...)
                            {
                                Types::DestroyAt(Keys + Index);
                                throw;
                            }

                            if (States[Index] == DeletedState)
                                Tombstones--;

                            States[Index] = UsedState;
                            Count++;
                            return true;
                        }

                    public:
                        HashMap() = default;

                        HashMap(const HashMap&) = delete;
                        HashMap& operator=(const HashMap&) = delete;

                        HashMap(HashMap&& Other) noexcept
                            : States(Other.States),
                              Keys(Other.Keys),
                              Values(Other.Values),
                              Count(Other.Count),
                              Tombstones(Other.Tombstones),
                              Capacity(Other.Capacity)
                        {
                            Other.States = nullptr;
                            Other.Keys = nullptr;
                            Other.Values = nullptr;
                            Other.Count = 0;
                            Other.Tombstones = 0;
                            Other.Capacity = 0;
                        }

                        HashMap& operator=(HashMap&& Other) noexcept
                        {
                            if (this == &Other)
                                return *this;

                            Clear();
                            ReleaseStorage();

                            States = Other.States;
                            Keys = Other.Keys;
                            Values = Other.Values;
                            Count = Other.Count;
                            Tombstones = Other.Tombstones;
                            Capacity = Other.Capacity;

                            Other.States = nullptr;
                            Other.Keys = nullptr;
                            Other.Values = nullptr;
                            Other.Count = 0;
                            Other.Tombstones = 0;
                            Other.Capacity = 0;

                            return *this;
                        }

                        ~HashMap()
                        {
                            Clear();
                            ReleaseStorage();
                        }

                        Types::Size GetCount() const noexcept { return Count; }
                        Types::Bool IsEmpty() const noexcept { return Count == 0; }

                        // Возвращает true, если ключ был добавлен, и false, если он уже существовал (значение не меняется).
                        Types::Bool Insert(const K& Key, V Value)
                        {
                            return Place(Key, Types::Move(Value), false);
                        }

                        // Добавляет ключ или заменяет значение существующего.
                        void Set(const K& Key, V Value)
                        {
                            Place(Key, Types::Move(Value), true);
                        }

                        V* Find(const K& Key)
                        {
                            const Types::Size Index = FindIndex(Key);
                            return Index == NotFound ? nullptr : Values + Index;
                        }

                        const V* Find(const K& Key) const
                        {
                            const Types::Size Index = FindIndex(Key);
                            return Index == NotFound ? nullptr : Values + Index;
                        }

                        Types::Bool Contains(const K& Key) const
                        {
                            return FindIndex(Key) != NotFound;
                        }

                        Types::Bool Remove(const K& Key)
                        {
                            const Types::Size Index = FindIndex(Key);

                            if (Index == NotFound)
                                return false;

                            Types::DestroyAt(Keys + Index);
                            Types::DestroyAt(Values + Index);
                            States[Index] = DeletedState;
                            Count--;
                            Tombstones++;
                            return true;
                        }

                        void Clear() noexcept
                        {
                            for (Types::Size Index = 0; Index < Capacity; Index++)
                            {
                                if (States[Index] == UsedState)
                                {
                                    Types::DestroyAt(Keys + Index);
                                    Types::DestroyAt(Values + Index);
                                }

                                States[Index] = EmptyState;
                            }

                            Count = 0;
                            Tombstones = 0;
                        }

                        // Visitor(const K&, V&) -> Bool; возврат false останавливает обход.
                        template <typename Visitor>
                        void ForEach(Visitor&& Visit)
                        {
                            for (Types::Size Index = 0; Index < Capacity; Index++)
                            {
                                if (States[Index] == UsedState && !Visit(Keys[Index], Values[Index]))
                                    return;
                            }
                        }

                        template <typename Visitor>
                        void ForEach(Visitor&& Visit) const
                        {
                            for (Types::Size Index = 0; Index < Capacity; Index++)
                            {
                                if (States[Index] == UsedState && !Visit(Keys[Index], Values[Index]))
                                    return;
                            }
                        }
                };
            }
        }
    }
}
