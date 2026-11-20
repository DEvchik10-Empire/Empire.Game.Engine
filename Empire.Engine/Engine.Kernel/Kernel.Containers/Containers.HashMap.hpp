#pragma once

#include "Empire.Engine/Engine.Kernel/Kernel.Assert/Assert.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Containers/Containers.Hash.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Memory/Memory.hpp"
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
                    // Открытая адресация, линейный пробинг, ёмкость всегда степень двойки.
                    // Указатели на значения (Find) инвалидируются при любой вставке, которая вызывает рост таблицы.
                    // Ключи и значения должны иметь noexcept move-конструктор, иначе перестройка таблицы небезопасна.
                    template <typename K, typename V, typename HasherType = Hash<K>>
                    class HashMap
                    {
                        static_assert(
                            KernelTypes::IsNothrowMoveConstructible<K> && KernelTypes::IsNothrowMoveConstructible<V>,
                            "HashMap requires nothrow move constructible keys and values"
                        );

                        private:
                            static constexpr KernelTypes::UInt8 EmptyState = 0;
                            static constexpr KernelTypes::UInt8 UsedState = 1;
                            static constexpr KernelTypes::UInt8 DeletedState = 2;
                            static constexpr KernelTypes::Size NotFound = KernelTypes::MaxValue<KernelTypes::Size>();

                            KernelTypes::UInt8* States = nullptr;
                            K* Keys = nullptr;
                            V* Values = nullptr;
                            KernelTypes::Size Count = 0;
                            KernelTypes::Size Tombstones = 0;
                            KernelTypes::Size Capacity = 0;
                            HasherType Hasher;

                            void ReleaseStorage() noexcept
                            {
                                KernelMemory::Deallocate(States);
                                KernelMemory::Deallocate(Keys);
                                KernelMemory::Deallocate(Values);

                                States = nullptr;
                                Keys = nullptr;
                                Values = nullptr;
                                Capacity = 0;
                            }

                            void Rehash(KernelTypes::Size NewCapacity)
                            {
                                if (NewCapacity > KernelTypes::MaxValue<KernelTypes::Size>() / (sizeof(K) + sizeof(V) + 1))
                                    KernelAssert::Fail("HashMap capacity is too large");

                                KernelTypes::UInt8* NewStates = nullptr;
                                K* NewKeys = nullptr;
                                V* NewValues = nullptr;


                                    NewStates = static_cast<KernelTypes::UInt8*>(KernelMemory::Allocate(NewCapacity));
                                    KernelMemory::FillBytes(NewStates, EmptyState, NewCapacity);
                                    NewKeys = static_cast<K*>(KernelMemory::Allocate(NewCapacity * sizeof(K), alignof(K)));
                                    NewValues = static_cast<V*>(KernelMemory::Allocate(NewCapacity * sizeof(V), alignof(V)));


                                const KernelTypes::Size Mask = NewCapacity - 1;

                                for (KernelTypes::Size Old = 0; Old < Capacity; Old++)
                                {
                                    if (States[Old] != UsedState)
                                        continue;

                                    KernelTypes::Size Index = static_cast<KernelTypes::Size>(Hasher(Keys[Old])) & Mask;

                                    while (NewStates[Index] == UsedState)
                                        Index = (Index + 1) & Mask;

                                    KernelTypes::ConstructAt(NewKeys + Index, KernelTypes::Move(Keys[Old]));
                                    KernelTypes::ConstructAt(NewValues + Index, KernelTypes::Move(Values[Old]));
                                    NewStates[Index] = UsedState;

                                    KernelTypes::DestroyAt(Keys + Old);
                                    KernelTypes::DestroyAt(Values + Old);
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

                                KernelTypes::Size NewCapacity = Capacity;

                                if ((Count + 1) * 10 > Capacity * 4)
                                {
                                    if (Capacity > KernelTypes::MaxValue<KernelTypes::Size>() / 4)
                                        KernelAssert::Fail("HashMap capacity is too large");

                                    NewCapacity = Capacity * 2;
                                }

                                Rehash(NewCapacity);
                            }

                            KernelTypes::Size FindIndex(const K& Key) const
                            {
                                if (Capacity == 0)
                                    return NotFound;

                                const KernelTypes::Size Mask = Capacity - 1;
                                KernelTypes::Size Index = static_cast<KernelTypes::Size>(Hasher(Key)) & Mask;

                                for (KernelTypes::Size Probe = 0; Probe < Capacity; Probe++)
                                {
                                    if (States[Index] == EmptyState)
                                        return NotFound;

                                    if (States[Index] == UsedState && Keys[Index] == Key)
                                        return Index;

                                    Index = (Index + 1) & Mask;
                                }

                                return NotFound;
                            }

                            KernelTypes::Bool Place(const K& Key, V&& Value, KernelTypes::Bool Overwrite)
                            {
                                const KernelTypes::Size Existing = FindIndex(Key);

                                if (Existing != NotFound)
                                {
                                    if (Overwrite)
                                        Values[Existing] = KernelTypes::Move(Value);

                                    return false;
                                }

                                GrowIfNeeded();

                                const KernelTypes::Size Mask = Capacity - 1;
                                KernelTypes::Size Index = static_cast<KernelTypes::Size>(Hasher(Key)) & Mask;

                                while (States[Index] == UsedState)
                                    Index = (Index + 1) & Mask;

                                KernelTypes::ConstructAt(Keys + Index, Key);


                                    KernelTypes::ConstructAt(Values + Index, KernelTypes::Move(Value));


                                if (States[Index] == DeletedState)
                                    Tombstones--;

                                States[Index] = UsedState;
                                Count++;
                                return true;
                            }

                        public:
                            struct EntryRef
                            {
                                const K& First;
                                V& Second;
                            };

                            struct ConstEntryRef
                            {
                                const K& First;
                                const V& Second;
                            };

                            class Iterator
                            {
                                private:
                                    HashMap* Owner = nullptr;
                                    KernelTypes::Size Index = 0;

                                    void SkipEmpty() noexcept
                                    {
                                        while (Owner != nullptr && Index < Owner->Capacity && Owner->States[Index] != UsedState)
                                            ++Index;
                                    }

                                public:
                                    Iterator() = default;
                                    Iterator(HashMap* Map, KernelTypes::Bool AtEnd) noexcept : Owner(Map), Index(AtEnd ? Map->Capacity : 0)
                                    {
                                        if (!AtEnd)
                                            SkipEmpty();
                                    }
                                    EntryRef operator*() const noexcept { return {Owner->Keys[Index], Owner->Values[Index]}; }
                                    Iterator& operator++() noexcept { ++Index; SkipEmpty(); return *this; }
                                    Iterator operator++(int) noexcept { Iterator Previous(*this); ++(*this); return Previous; }
                                    [[nodiscard]] KernelTypes::Bool operator==(const Iterator& Other) const noexcept { return Owner == Other.Owner && Index == Other.Index; }
                                    [[nodiscard]] KernelTypes::Bool operator!=(const Iterator& Other) const noexcept { return !(*this == Other); }
                            };

                            class ConstIterator
                            {
                                private:
                                    const HashMap* Owner = nullptr;
                                    KernelTypes::Size Index = 0;

                                    void SkipEmpty() noexcept
                                    {
                                        while (Owner != nullptr && Index < Owner->Capacity && Owner->States[Index] != UsedState)
                                            ++Index;
                                    }

                                public:
                                    ConstIterator() = default;
                                    ConstIterator(const HashMap* Map, KernelTypes::Bool AtEnd) noexcept : Owner(Map), Index(AtEnd ? Map->Capacity : 0)
                                    {
                                        if (!AtEnd)
                                            SkipEmpty();
                                    }
                                    ConstEntryRef operator*() const noexcept { return {Owner->Keys[Index], Owner->Values[Index]}; }
                                    ConstIterator& operator++() noexcept { ++Index; SkipEmpty(); return *this; }
                                    ConstIterator operator++(int) noexcept { ConstIterator Previous(*this); ++(*this); return Previous; }
                                    [[nodiscard]] KernelTypes::Bool operator==(const ConstIterator& Other) const noexcept { return Owner == Other.Owner && Index == Other.Index; }
                                    [[nodiscard]] KernelTypes::Bool operator!=(const ConstIterator& Other) const noexcept { return !(*this == Other); }
                            };

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

                            KernelTypes::Size GetCount() const noexcept { return Count; }
                            KernelTypes::Bool IsEmpty() const noexcept { return Count == 0; }
                            KernelTypes::Size GetCapacity() const noexcept { return Capacity; }
                            Iterator begin() noexcept { return Iterator(this, false); }
                            Iterator end() noexcept { return Iterator(this, true); }
                            ConstIterator begin() const noexcept { return ConstIterator(this, false); }
                            ConstIterator end() const noexcept { return ConstIterator(this, true); }
                            ConstIterator cbegin() const noexcept { return begin(); }
                            ConstIterator cend() const noexcept { return end(); }

                            void Reserve(KernelTypes::Size DesiredCount)
                            {
                                KernelTypes::Size NewCapacity = Capacity == 0 ? 8 : Capacity;
                                const auto MaximumCount = [](KernelTypes::Size Value) noexcept
                                {
                                    return (Value / 10) * 7 + ((Value % 10) * 7) / 10;
                                };

                                while (DesiredCount > MaximumCount(NewCapacity))
                                {
                                    if (NewCapacity > KernelTypes::MaxValue<KernelTypes::Size>() / 2)
                                        KernelAssert::Fail("HashMap capacity is too large");
                                    NewCapacity *= 2;
                                }

                                if (NewCapacity != Capacity || Tombstones > Count)
                                    Rehash(NewCapacity);
                            }

                            // Возвращает true, если ключ был добавлен, и false, если он уже существовал (значение не меняется).
                            KernelTypes::Bool Insert(const K& Key, V Value)
                            {
                                return Place(Key, KernelTypes::Move(Value), false);
                            }

                            // Добавляет ключ или заменяет значение существующего.
                            void Set(const K& Key, V Value)
                            {
                                Place(Key, KernelTypes::Move(Value), true);
                            }

                            template <typename... Arguments>
                            V& TryEmplace(const K& Key, Arguments&&... Args)
                            {
                                if (V* Existing = Find(Key))
                                    return *Existing;
                                Place(Key, V(KernelTypes::Forward<Arguments>(Args)...), false);
                                return *Find(Key);
                            }

                            V& operator[](const K& Key)
                                requires requires { V{}; }
                            {
                                return TryEmplace(Key);
                            }

                            V* Find(const K& Key)
                            {
                                const KernelTypes::Size Index = FindIndex(Key);
                                return Index == NotFound ? nullptr : Values + Index;
                            }

                            const V* Find(const K& Key) const
                            {
                                const KernelTypes::Size Index = FindIndex(Key);
                                return Index == NotFound ? nullptr : Values + Index;
                            }

                            KernelTypes::Bool Contains(const K& Key) const
                            {
                                return FindIndex(Key) != NotFound;
                            }

                            KernelTypes::Bool Remove(const K& Key)
                            {
                                const KernelTypes::Size Index = FindIndex(Key);

                                if (Index == NotFound)
                                    return false;

                                KernelTypes::DestroyAt(Keys + Index);
                                KernelTypes::DestroyAt(Values + Index);
                                States[Index] = DeletedState;
                                Count--;
                                Tombstones++;
                                return true;
                            }

                            void Clear() noexcept
                            {
                                for (KernelTypes::Size Index = 0; Index < Capacity; Index++)
                                {
                                    if (States[Index] == UsedState)
                                    {
                                        KernelTypes::DestroyAt(Keys + Index);
                                        KernelTypes::DestroyAt(Values + Index);
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
                                for (KernelTypes::Size Index = 0; Index < Capacity; Index++)
                                {
                                    if (States[Index] == UsedState && !Visit(Keys[Index], Values[Index]))
                                        return;
                                }
                            }

                            template <typename Visitor>
                            void ForEach(Visitor&& Visit) const
                            {
                                for (KernelTypes::Size Index = 0; Index < Capacity; Index++)
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
}
