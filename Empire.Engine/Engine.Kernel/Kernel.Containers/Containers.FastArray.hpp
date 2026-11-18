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
                    // Простая замена std::vector для горячих путей (перенесена из Jolt, Core/Array.h).
                    //
                    // Отличия от Array этого же модуля:
                    // - память не обнуляется (при загрузке больших мешей обнуление давало много лишних обращений к страницам);
                    // - индексы и пустота проверяются только в отладочной сборке (DebugCheck), исключения не бросаются;
                    // - итераторы это обычные указатели;
                    // - нет безопасности при исключениях в конструкторах элементов;
                    // - реализованы не все функции std::vector.
                    //
                    // begin() и end() записаны строчными, потому что их требует цикл range-for.
                    template <typename T>
                    class [[nodiscard]] FastArray
                    {
                        public:
                            using ValueType = T;
                            using SizeType = KernelTypes::Size;
                            using DifferenceType = decltype(static_cast<T*>(nullptr) - static_cast<T*>(nullptr));
                            using Pointer = T*;
                            using ConstPointer = const T*;
                            using Reference = T&;
                            using ConstReference = const T&;

                            using ConstIterator = const T*;
                            using Iterator = T*;

                            // Итератор, который идёт по массиву в обратном порядке
                            class ReverseIterator
                            {
                                public:
                                    ReverseIterator() = default;
                                    explicit ReverseIterator(T* Value) : Current(Value) { }

                                    ReverseIterator(const ReverseIterator&) = default;
                                    ReverseIterator& operator=(const ReverseIterator&) = default;

                                    bool operator==(const ReverseIterator& Other) const { return Current == Other.Current; }
                                    bool operator!=(const ReverseIterator& Other) const { return Current != Other.Current; }

                                    ReverseIterator& operator++() { --Current; return *this; }
                                    ReverseIterator operator++(int) { return ReverseIterator(Current--); }
                                    ReverseIterator& operator--() { ++Current; return *this; }
                                    ReverseIterator operator--(int) { return ReverseIterator(Current++); }

                                    ReverseIterator operator+(int Value) const { return ReverseIterator(Current - Value); }
                                    ReverseIterator operator-(int Value) const { return ReverseIterator(Current + Value); }

                                    ReverseIterator& operator+=(int Value) { Current -= Value; return *this; }
                                    ReverseIterator& operator-=(int Value) { Current += Value; return *this; }

                                    T& operator*() const { return *Current; }
                                    T* operator->() const { return Current; }

                                private:
                                    friend class FastArray;

                                    T* Current = nullptr;
                            };

                            // Константная версия обратного итератора
                            class ConstReverseIterator
                            {
                                public:
                                    ConstReverseIterator() = default;
                                    explicit ConstReverseIterator(const T* Value) : Current(Value) { }

                                    ConstReverseIterator(const ConstReverseIterator&) = default;
                                    ConstReverseIterator(const ReverseIterator& Other) : Current(Other.Current) { }
                                    ConstReverseIterator& operator=(const ConstReverseIterator&) = default;
                                    ConstReverseIterator& operator=(const ReverseIterator& Other) { Current = Other.Current; return *this; }

                                    bool operator==(const ConstReverseIterator& Other) const { return Current == Other.Current; }
                                    bool operator!=(const ConstReverseIterator& Other) const { return Current != Other.Current; }

                                    ConstReverseIterator& operator++() { --Current; return *this; }
                                    ConstReverseIterator operator++(int) { return ConstReverseIterator(Current--); }
                                    ConstReverseIterator& operator--() { ++Current; return *this; }
                                    ConstReverseIterator operator--(int) { return ConstReverseIterator(Current++); }

                                    ConstReverseIterator operator+(int Value) const { return ConstReverseIterator(Current - Value); }
                                    ConstReverseIterator operator-(int Value) const { return ConstReverseIterator(Current + Value); }

                                    ConstReverseIterator& operator+=(int Value) { Current -= Value; return *this; }
                                    ConstReverseIterator& operator-=(int Value) { Current += Value; return *this; }

                                    const T& operator*() const { return *Current; }
                                    const T* operator->() const { return Current; }

                                private:
                                    const T* Current = nullptr;
                            };

                        private:
                            // Выравнивание блока: не меньше стандартного для Kernel.Memory
                            static constexpr KernelTypes::Size BlockAlignment =
                                alignof(T) > KernelMemory::DefaultAlignment ? alignof(T) : KernelMemory::DefaultAlignment;

                            static T* AllocateBlock(SizeType Capacity)
                            {
                                return static_cast<T*>(KernelMemory::Allocate(Capacity * sizeof(T), BlockAlignment));
                            }

                            // Переносит элементы из одного места в другое. Области могут пересекаться.
                            void Move(Pointer Destination, Pointer Source, SizeType Count)
                            {
                                if constexpr (KernelTypes::IsTriviallyCopyable<T>)
                                {
                                    KernelMemory::MoveBytes(Destination, Source, Count * sizeof(T));
                                }
                                else
                                {
                                    if (Destination < Source)
                                    {
                                        for (T* DestinationEnd = Destination + Count; Destination < DestinationEnd; ++Destination, ++Source)
                                        {
                                            ::new (KernelTypes::PlacementTag{}, static_cast<void*>(Destination)) T(KernelTypes::Move(*Source));
                                            Source->~T();
                                        }
                                    }
                                    else
                                    {
                                        for (T *Target = Destination + Count - 1, *Origin = Source + Count - 1; Target >= Destination; --Target, --Origin)
                                        {
                                            ::new (KernelTypes::PlacementTag{}, static_cast<void*>(Target)) T(KernelTypes::Move(*Origin));
                                            Origin->~T();
                                        }
                                    }
                                }
                            }

                            // Меняет размер блока памяти на NewCapacity
                            void Reallocate(SizeType NewCapacity)
                            {
                                KernelAssert::DebugCheck(NewCapacity > 0 && NewCapacity >= Count, "FastArray::Reallocate: ёмкость меньше размера");

                                Pointer Block;

                                if constexpr (KernelTypes::IsTriviallyCopyable<T> && alignof(T) <= KernelMemory::DefaultAlignment)
                                {
                                    // Блок можно переносить побайтно, поэтому просим у аллокатора расширить его на месте
                                    Block = Items == nullptr
                                        ? AllocateBlock(NewCapacity)
                                        : static_cast<T*>(KernelMemory::Reallocate(Items, NewCapacity * sizeof(T)));
                                }
                                else
                                {
                                    // Копируем данные в новое место
                                    Block = AllocateBlock(NewCapacity);

                                    if (Items != nullptr)
                                    {
                                        Move(Block, Items, Count);
                                        KernelMemory::Deallocate(Items);
                                    }
                                }

                                Items = Block;
                                Capacity = NewCapacity;
                            }

                            // Вызывает деструкторы элементов [Start, End - 1]
                            void Destruct(SizeType Start, SizeType End)
                            {
                                if constexpr (!KernelTypes::IsTriviallyDestructible<T>)
                                {
                                    if (Start < End)
                                    {
                                        for (T *Element = Items + Start, *ElementEnd = Items + End; Element < ElementEnd; ++Element)
                                        {
                                            Element->~T();
                                        }
                                    }
                                }
                            }

                            // Увеличивает массив минимум на Amount элементов
                            void Grow(SizeType Amount = 1)
                            {
                                const SizeType MinimumSize = Count + Amount;

                                if (MinimumSize > Capacity)
                                {
                                    Reserve(KernelTypes::Max(MinimumSize, Capacity * 2));
                                }
                            }

                            // Освобождает память
                            void Release()
                            {
                                KernelMemory::Deallocate(Items);
                                Items = nullptr;
                                Capacity = 0;
                            }

                            // Удаляет все элементы и освобождает память
                            void Destroy()
                            {
                                if (Items != nullptr)
                                {
                                    Clear();
                                    Release();
                                }
                            }

                        public:
                            // Резервирует место в массиве
                            void Reserve(SizeType NewCapacity)
                            {
                                if (Capacity < NewCapacity)
                                {
                                    Reallocate(NewCapacity);
                                }
                            }

                            // Меняет длину массива. Новые элементы создаются конструктором по умолчанию
                            // (у тривиальных типов значения не определены).
                            void Resize(SizeType NewSize)
                            {
                                Destruct(NewSize, Count);
                                Reserve(NewSize);

                                if constexpr (!KernelTypes::IsTriviallyConstructible<T>)
                                {
                                    for (T *Element = Items + Count, *ElementEnd = Items + NewSize; Element < ElementEnd; ++Element)
                                    {
                                        ::new (KernelTypes::PlacementTag{}, static_cast<void*>(Element)) T;
                                    }
                                }

                                Count = NewSize;
                            }

                            // Меняет длину массива и заполняет новые элементы значением Value
                            void Resize(SizeType NewSize, const T& Value)
                            {
                                KernelAssert::DebugCheck(&Value < Items || &Value >= Items + Count, "FastArray::Resize: нельзя передавать элемент этого же массива");

                                Destruct(NewSize, Count);
                                Reserve(NewSize);

                                for (T *Element = Items + Count, *ElementEnd = Items + NewSize; Element < ElementEnd; ++Element)
                                {
                                    KernelTypes::ConstructAt(Element, Value);
                                }

                                Count = NewSize;
                            }

                            // Вызывает деструкторы всех элементов, длина становится нулевой
                            void Clear()
                            {
                                Destruct(0, Count);
                                Count = 0;
                            }

                            // Заменяет содержимое элементами Begin .. End
                            template <typename InputIterator>
                            void Assign(InputIterator Begin, InputIterator End)
                            {
                                Clear();
                                Reserve(static_cast<SizeType>(End - Begin));

                                for (InputIterator Element = Begin; Element != End; ++Element)
                                {
                                    KernelTypes::ConstructAt(&Items[Count++], *Element);
                                }
                            }

                            FastArray() = default;

                            // Массив из Length элементов, созданных конструктором по умолчанию
                            explicit FastArray(SizeType Length)
                            {
                                Resize(Length);
                            }

                            // Массив из Length копий Value
                            FastArray(SizeType Length, const T& Value)
                            {
                                Resize(Length, Value);
                            }

                            FastArray(ConstIterator Begin, ConstIterator End)
                            {
                                Assign(Begin, End);
                            }

                            FastArray(const FastArray& Other)
                            {
                                Assign(Other.begin(), Other.end());
                            }

                            FastArray(FastArray&& Other) noexcept
                                : Count(Other.Count), Capacity(Other.Capacity), Items(Other.Items)
                            {
                                Other.Count = 0;
                                Other.Capacity = 0;
                                Other.Items = nullptr;
                            }

                            ~FastArray()
                            {
                                Destroy();
                            }

                            // Замена списку инициализации (в Kernel нет std::initializer_list): FastArray<int>::Of(1, 2, 3)
                            template <typename... Values>
                            [[nodiscard]] static FastArray Of(Values&&... Elements)
                            {
                                FastArray Result;
                                Result.Reserve(sizeof...(Values));
                                (Result.EmplaceBack(KernelTypes::Forward<Values>(Elements)), ...);
                                return Result;
                            }

                            // Добавляет элемент в конец
                            void PushBack(const T& Value)
                            {
                                KernelAssert::DebugCheck(&Value < Items || &Value >= Items + Count, "FastArray::PushBack: нельзя передавать элемент этого же массива");

                                Grow();
                                KernelTypes::ConstructAt(Items + Count++, Value);
                            }

                            void PushBack(T&& Value)
                            {
                                Grow();
                                KernelTypes::ConstructAt(Items + Count++, KernelTypes::Move(Value));
                            }

                            // Создаёт элемент на месте в конце массива
                            template <typename... Arguments>
                            T& EmplaceBack(Arguments&&... Args)
                            {
                                Grow();

                                T* Element = Items + Count++;
                                KernelTypes::ConstructAt(Element, KernelTypes::Forward<Arguments>(Args)...);
                                return *Element;
                            }

                            // Убирает последний элемент
                            void PopBack()
                            {
                                KernelAssert::DebugCheck(Count > 0, "FastArray::PopBack: массив пуст");
                                Items[--Count].~T();
                            }

                            [[nodiscard]] bool Empty() const { return Count == 0; }
                            [[nodiscard]] SizeType Size() const { return Count; }
                            [[nodiscard]] SizeType GetCapacity() const { return Capacity; }

                            // Уменьшает ёмкость до размера
                            void ShrinkToFit()
                            {
                                if (Items != nullptr)
                                {
                                    if (Count == 0)
                                    {
                                        Release();
                                    }
                                    else if (Capacity > Count)
                                    {
                                        Reallocate(Count);
                                    }
                                }
                            }

                            // Меняет местами содержимое двух массивов
                            void Swap(FastArray& Other) noexcept
                            {
                                KernelTypes::Swap(Count, Other.Count);
                                KernelTypes::Swap(Capacity, Other.Capacity);
                                KernelTypes::Swap(Items, Other.Items);
                            }

                            // Вставляет элементы Begin .. End перед Position
                            template <typename InputIterator>
                            void Insert(ConstIterator Position, InputIterator Begin, InputIterator End)
                            {
                                const SizeType NewElementCount = static_cast<SizeType>(End - Begin);

                                if (NewElementCount > 0)
                                {
                                    // После Grow() Position может стать недействительным
                                    const SizeType FirstElement = static_cast<SizeType>(Position - Items);

                                    Grow(NewElementCount);

                                    T* ElementBegin = Items + FirstElement;
                                    T* ElementEnd = ElementBegin + NewElementCount;
                                    Move(ElementEnd, ElementBegin, Count - FirstElement);

                                    for (T* Element = ElementBegin; Element < ElementEnd; ++Element, ++Begin)
                                    {
                                        KernelTypes::ConstructAt(Element, *Begin);
                                    }

                                    Count += NewElementCount;
                                }
                            }

                            void Insert(ConstIterator Position, const T& Value)
                            {
                                KernelAssert::DebugCheck(&Value < Items || &Value >= Items + Count, "FastArray::Insert: нельзя передавать элемент этого же массива");

                                // После Grow() Position может стать недействительным
                                const SizeType FirstElement = static_cast<SizeType>(Position - Items);

                                Grow();

                                T* Element = Items + FirstElement;
                                Move(Element + 1, Element, Count - FirstElement);

                                KernelTypes::ConstructAt(Element, Value);
                                Count++;
                            }

                            // Убирает один элемент
                            Iterator Erase(ConstIterator Position)
                            {
                                const SizeType Index = static_cast<SizeType>(Position - begin());
                                KernelAssert::DebugCheck(Index < Count, "FastArray::Erase: индекс вне массива");

                                Items[Index].~T();

                                if (Index + 1 < Count)
                                {
                                    Move(Items + Index, Items + Index + 1, Count - Index - 1);
                                }

                                --Count;
                                return const_cast<Iterator>(Position);
                            }

                            // Убирает несколько элементов
                            Iterator Erase(ConstIterator Begin, ConstIterator End)
                            {
                                const SizeType Index = static_cast<SizeType>(Begin - begin());
                                const SizeType RemovedCount = static_cast<SizeType>(End - Begin);
                                KernelAssert::DebugCheck(End <= this->end(), "FastArray::Erase: конец диапазона вне массива");

                                Destruct(Index, Index + RemovedCount);

                                if (Index + RemovedCount < Count)
                                {
                                    Move(Items + Index, Items + Index + RemovedCount, Count - Index - RemovedCount);
                                }

                                Count -= RemovedCount;
                                return const_cast<Iterator>(Begin);
                            }

                            ConstIterator begin() const { return Items; }
                            ConstIterator end() const { return Items + Count; }
                            ConstReverseIterator rbegin() const { return ConstReverseIterator(Items + Count - 1); }
                            ConstReverseIterator rend() const { return ConstReverseIterator(Items - 1); }
                            ConstIterator cbegin() const { return begin(); }
                            ConstIterator cend() const { return end(); }
                            ConstReverseIterator crbegin() const { return rbegin(); }
                            ConstReverseIterator crend() const { return rend(); }

                            Iterator begin() { return Items; }
                            Iterator end() { return Items + Count; }
                            ReverseIterator rbegin() { return ReverseIterator(Items + Count - 1); }
                            ReverseIterator rend() { return ReverseIterator(Items - 1); }

                            const T* Data() const { return Items; }
                            T* Data() { return Items; }

                            T& operator[](SizeType Index)
                            {
                                KernelAssert::DebugCheck(Index < Count, "FastArray: индекс вне массива");
                                return Items[Index];
                            }

                            const T& operator[](SizeType Index) const
                            {
                                KernelAssert::DebugCheck(Index < Count, "FastArray: индекс вне массива");
                                return Items[Index];
                            }

                            T& At(SizeType Index)
                            {
                                KernelAssert::DebugCheck(Index < Count, "FastArray::At: индекс вне массива");
                                return Items[Index];
                            }

                            const T& At(SizeType Index) const
                            {
                                KernelAssert::DebugCheck(Index < Count, "FastArray::At: индекс вне массива");
                                return Items[Index];
                            }

                            const T& Front() const
                            {
                                KernelAssert::DebugCheck(Count > 0, "FastArray::Front: массив пуст");
                                return Items[0];
                            }

                            T& Front()
                            {
                                KernelAssert::DebugCheck(Count > 0, "FastArray::Front: массив пуст");
                                return Items[0];
                            }

                            const T& Back() const
                            {
                                KernelAssert::DebugCheck(Count > 0, "FastArray::Back: массив пуст");
                                return Items[Count - 1];
                            }

                            T& Back()
                            {
                                KernelAssert::DebugCheck(Count > 0, "FastArray::Back: массив пуст");
                                return Items[Count - 1];
                            }

                            FastArray& operator=(const FastArray& Other)
                            {
                                if (this != &Other)
                                {
                                    Assign(Other.begin(), Other.end());
                                }

                                return *this;
                            }

                            FastArray& operator=(FastArray&& Other) noexcept
                            {
                                if (this != &Other)
                                {
                                    Destroy();

                                    Count = Other.Count;
                                    Capacity = Other.Capacity;
                                    Items = Other.Items;

                                    Other.Count = 0;
                                    Other.Capacity = 0;
                                    Other.Items = nullptr;
                                }

                                return *this;
                            }

                            bool operator==(const FastArray& Other) const
                            {
                                if (Count != Other.Count)
                                {
                                    return false;
                                }

                                for (SizeType Index = 0; Index < Count; ++Index)
                                {
                                    if (!(Items[Index] == Other.Items[Index]))
                                    {
                                        return false;
                                    }
                                }

                                return true;
                            }

                            bool operator!=(const FastArray& Other) const
                            {
                                return !(*this == Other);
                            }

                            // Хеш массива: сначала длина, потом элементы по порядку
                            [[nodiscard]] KernelTypes::UInt64 GetHash() const
                            {
                                KernelTypes::UInt64 Result = Hash<KernelTypes::UInt32>{}(static_cast<KernelTypes::UInt32>(Size()));

                                for (const T *Element = Items, *ElementEnd = Items + Count; Element < ElementEnd; ++Element)
                                {
                                    HashCombine(Result, *Element);
                                }

                                return Result;
                            }

                        private:
                            SizeType Count = 0;
                            SizeType Capacity = 0;
                            T* Items = nullptr;
                    };
                }
            }
        }
    }
}
