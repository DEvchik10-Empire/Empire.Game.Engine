#pragma once

#include "Empire.Engine/Engine.Kernel/Kernel.Assert/Assert.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Memory/Memory.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Containers/Containers.Hash.hpp"
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
                    template <typename T>
                    class Array
                    {
                        public:
                            using ValueType = T;
                            using SizeType = KernelTypes::Size;
                            using DifferenceType = decltype(static_cast<T*>(nullptr) - static_cast<T*>(nullptr));
                            using Iterator = T*;
                            using ConstIterator = const T*;

                            class ReverseIterator
                            {
                                public:
                                    ReverseIterator() = default;
                                    explicit ReverseIterator(T* Value) noexcept : Current(Value) { }
                                    T& operator*() const { return *(Current - 1); }
                                    T* operator->() const { return Current - 1; }
                                    [[nodiscard]] T* GetBase() const noexcept { return Current; }
                                    ReverseIterator& operator++() noexcept { --Current; return *this; }
                                    ReverseIterator operator++(int) noexcept { ReverseIterator Result(*this); --Current; return Result; }
                                    ReverseIterator& operator--() noexcept { ++Current; return *this; }
                                    ReverseIterator operator--(int) noexcept { ReverseIterator Result(*this); ++Current; return Result; }
                                    ReverseIterator operator+(DifferenceType Offset) const noexcept { return ReverseIterator(Current == nullptr ? nullptr : Current - Offset); }
                                    friend ReverseIterator operator+(DifferenceType Offset, const ReverseIterator& Value) noexcept { return Value + Offset; }
                                    ReverseIterator operator-(DifferenceType Offset) const noexcept { return ReverseIterator(Current == nullptr ? nullptr : Current + Offset); }
                                    ReverseIterator& operator+=(DifferenceType Offset) noexcept { if (Current != nullptr) Current -= Offset; return *this; }
                                    ReverseIterator& operator-=(DifferenceType Offset) noexcept { if (Current != nullptr) Current += Offset; return *this; }
                                    DifferenceType operator-(const ReverseIterator& Other) const noexcept { return Current == Other.Current ? 0 : Other.Current - Current; }
                                    T& operator[](DifferenceType Offset) const { return *(*this + Offset); }
                                    [[nodiscard]] bool operator==(const ReverseIterator& Other) const noexcept { return Current == Other.Current; }
                                    [[nodiscard]] bool operator!=(const ReverseIterator& Other) const noexcept { return Current != Other.Current; }
                                    [[nodiscard]] bool operator<(const ReverseIterator& Other) const noexcept { return Current != Other.Current && Current > Other.Current; }
                                    [[nodiscard]] bool operator<=(const ReverseIterator& Other) const noexcept { return *this == Other || *this < Other; }
                                    [[nodiscard]] bool operator>(const ReverseIterator& Other) const noexcept { return Other < *this; }
                                    [[nodiscard]] bool operator>=(const ReverseIterator& Other) const noexcept { return *this == Other || *this > Other; }
                                private:
                                    T* Current = nullptr;
                            };

                            class ConstReverseIterator
                            {
                                public:
                                    ConstReverseIterator() = default;
                                    explicit ConstReverseIterator(const T* Value) noexcept : Current(Value) { }
                                    ConstReverseIterator(const ReverseIterator& Value) noexcept : Current(Value.GetBase()) { }
                                    const T& operator*() const { return *(Current - 1); }
                                    const T* operator->() const { return Current - 1; }
                                    ConstReverseIterator& operator++() noexcept { --Current; return *this; }
                                    ConstReverseIterator operator++(int) noexcept { ConstReverseIterator Result(*this); --Current; return Result; }
                                    ConstReverseIterator& operator--() noexcept { ++Current; return *this; }
                                    ConstReverseIterator operator--(int) noexcept { ConstReverseIterator Result(*this); ++Current; return Result; }
                                    ConstReverseIterator operator+(DifferenceType Offset) const noexcept { return ConstReverseIterator(Current == nullptr ? nullptr : Current - Offset); }
                                    friend ConstReverseIterator operator+(DifferenceType Offset, const ConstReverseIterator& Value) noexcept { return Value + Offset; }
                                    ConstReverseIterator operator-(DifferenceType Offset) const noexcept { return ConstReverseIterator(Current == nullptr ? nullptr : Current + Offset); }
                                    ConstReverseIterator& operator+=(DifferenceType Offset) noexcept { if (Current != nullptr) Current -= Offset; return *this; }
                                    ConstReverseIterator& operator-=(DifferenceType Offset) noexcept { if (Current != nullptr) Current += Offset; return *this; }
                                    DifferenceType operator-(const ConstReverseIterator& Other) const noexcept { return Current == Other.Current ? 0 : Other.Current - Current; }
                                    const T& operator[](DifferenceType Offset) const { return *(*this + Offset); }
                                    [[nodiscard]] bool operator==(const ConstReverseIterator& Other) const noexcept { return Current == Other.Current; }
                                    [[nodiscard]] bool operator!=(const ConstReverseIterator& Other) const noexcept { return Current != Other.Current; }
                                    [[nodiscard]] bool operator<(const ConstReverseIterator& Other) const noexcept { return Current != Other.Current && Current > Other.Current; }
                                    [[nodiscard]] bool operator<=(const ConstReverseIterator& Other) const noexcept { return *this == Other || *this < Other; }
                                    [[nodiscard]] bool operator>(const ConstReverseIterator& Other) const noexcept { return Other < *this; }
                                    [[nodiscard]] bool operator>=(const ConstReverseIterator& Other) const noexcept { return *this == Other || *this > Other; }
                                private:
                                    const T* Current = nullptr;
                            };

                        private:
                            T* Items = nullptr;
                            KernelTypes::Size Count = 0;
                            KernelTypes::Size Capacity = 0;

                            static T* AllocateItems(KernelTypes::Size ItemCount)
                            {
                                if (ItemCount > KernelTypes::MaxValue<KernelTypes::Size>() / sizeof(T))
                                    KernelAssert::Fail("Array capacity is too large");

                                return static_cast<T*>(KernelMemory::Allocate(ItemCount * sizeof(T), alignof(T)));
                            }

                            static void DeallocateItems(T* MemoryBlock) noexcept
                            {
                                KernelMemory::Deallocate(MemoryBlock);
                            }

                            KernelTypes::Size GetGrownCapacity() const
                            {
                                if (Capacity == 0)
                                    return 4;

                                if (Capacity > KernelTypes::MaxValue<KernelTypes::Size>() / 2)
                                    KernelAssert::Fail("Array capacity is too large");

                                return Capacity * 2;
                            }

                        public:
                            Array() = default;

                            explicit Array(KernelTypes::Size InitialCount)
                            {
                                Resize(InitialCount);
                            }

                            Array(KernelTypes::Size InitialCount, const T& InitialValue)
                            {
                                Resize(InitialCount, InitialValue);
                            }

                            template <typename InputIterator>
                                requires (!KernelTypes::Integral<InputIterator>)
                            Array(InputIterator First, InputIterator Last)
                            {
                                Assign(First, Last);
                            }

                            // Array<int>::Of(1, 2, 3): замена списку инициализации, не требующая std::initializer_list.
                            template <typename... Values>
                                requires (sizeof...(Values) > 0)
                            [[nodiscard]] static Array Of(Values&&... Items)
                            {
                                Array Result;
                                Result.Reserve(sizeof...(Values));
                                (Result.Add(KernelTypes::Forward<Values>(Items)), ...);
                                return Result;
                            }

                            Array(const Array& Other)
                            {
                                if (Other.Count == 0)
                                    return;

                                Items = AllocateItems(Other.Count);
                                Capacity = Other.Count;


                                    for (; Count < Other.Count; Count++)
                                        KernelTypes::ConstructAt(Items + Count, Other.Items[Count]);

                            }

                            Array(Array&& Other) noexcept
                                : Items(Other.Items), Count(Other.Count), Capacity(Other.Capacity)
                            {
                                Other.Items = nullptr;
                                Other.Count = 0;
                                Other.Capacity = 0;
                            }

                            ~Array()
                            {
                                Clear();
                                DeallocateItems(Items);
                            }

                            Array& operator=(const Array& Other)
                            {
                                if (this == &Other)
                                    return *this;

                                Array Copy(Other);
                                *this = KernelTypes::Move(Copy);
                                return *this;
                            }

                            Array& operator=(Array&& Other) noexcept
                            {
                                if (this == &Other)
                                    return *this;

                                Clear();
                                DeallocateItems(Items);

                                Items = Other.Items;
                                Count = Other.Count;
                                Capacity = Other.Capacity;

                                Other.Items = nullptr;
                                Other.Count = 0;
                                Other.Capacity = 0;

                                return *this;
                            }

                            KernelTypes::Size GetCount() const noexcept { return Count; }
                            KernelTypes::Size GetCapacity() const noexcept { return Capacity; }
                            KernelTypes::Bool IsEmpty() const noexcept { return Count == 0; }
                            T* GetData() noexcept { return Items; }
                            const T* GetData() const noexcept { return Items; }

                            T& Front()
                            {
                                KernelAssert::DebugCheck(Count > 0, "Array::Front: массив пуст");
                                return Items[0];
                            }

                            const T& Front() const
                            {
                                KernelAssert::DebugCheck(Count > 0, "Array::Front: массив пуст");
                                return Items[0];
                            }

                            T& Back()
                            {
                                KernelAssert::DebugCheck(Count > 0, "Array::Back: массив пуст");
                                return Items[Count - 1];
                            }

                            const T& Back() const
                            {
                                KernelAssert::DebugCheck(Count > 0, "Array::Back: массив пуст");
                                return Items[Count - 1];
                            }

                            T* Begin() noexcept { return Items; }
                            T* End() noexcept { return Count == 0 ? Items : Items + Count; }
                            const T* Begin() const noexcept { return Items; }
                            const T* End() const noexcept { return Count == 0 ? Items : Items + Count; }

                            // Нижний регистр нужен языку для range-for (for (auto& Item : Array)).
                            T* begin() noexcept { return Items; }
                            T* end() noexcept { return Count == 0 ? Items : Items + Count; }
                            const T* begin() const noexcept { return Items; }
                            const T* end() const noexcept { return Count == 0 ? Items : Items + Count; }

                            T& operator[](KernelTypes::Size Index)
                            {
                                if (Index >= Count)
                                    KernelAssert::Fail("Array index is out of range");

                                return Items[Index];
                            }

                            const T& operator[](KernelTypes::Size Index) const
                            {
                                if (Index >= Count)
                                    KernelAssert::Fail("Array index is out of range");

                                return Items[Index];
                            }

                            void Reserve(KernelTypes::Size NewCapacity)
                            {
                                if (NewCapacity <= Capacity)
                                    return;

                                T* NewItems = AllocateItems(NewCapacity);
                                KernelTypes::Size Constructed = 0;


                                    for (; Constructed < Count; Constructed++)
                                        KernelTypes::ConstructAt(NewItems + Constructed, KernelTypes::MoveIfNoexcept(Items[Constructed]));


                                for (KernelTypes::Size Index = 0; Index < Count; Index++)
                                    KernelTypes::DestroyAt(Items + Index);

                                DeallocateItems(Items);
                                Items = NewItems;
                                Capacity = NewCapacity;
                            }

                            void Add(const T& Value)
                            {
                                if (Count == Capacity)
                                {
                                    T Copy(Value);
                                    Reserve(GetGrownCapacity());
                                    KernelTypes::ConstructAt(Items + Count, KernelTypes::Move(Copy));
                                }
                                else
                                {
                                    KernelTypes::ConstructAt(Items + Count, Value);
                                }

                                Count++;
                            }

                            void Add(T&& Value)
                            {
                                if (Count == Capacity)
                                {
                                    T Copy(KernelTypes::Move(Value));
                                    Reserve(GetGrownCapacity());
                                    KernelTypes::ConstructAt(Items + Count, KernelTypes::Move(Copy));
                                }
                                else
                                {
                                    KernelTypes::ConstructAt(Items + Count, KernelTypes::Move(Value));
                                }

                                Count++;
                            }

                            template <typename... Arguments>
                            T& EmplaceBack(Arguments&&... Args)
                            {
                                if (Count == Capacity)
                                    Reserve(GetGrownCapacity());

                                T* Added = KernelTypes::ConstructAt(Items + Count, KernelTypes::Forward<Arguments>(Args)...);
                                Count++;
                                return *Added;
                            }

                            void PopBack()
                            {
                                KernelAssert::DebugCheck(Count > 0, "Array::PopBack: массив пуст");
                                --Count;
                                KernelTypes::DestroyAt(Items + Count);
                            }

                            void Resize(KernelTypes::Size NewCount)
                            {
                                if (NewCount < Count)
                                {
                                    while (Count > NewCount)
                                        PopBack();
                                    return;
                                }

                                if (NewCount > Capacity)
                                    Reserve(NewCount);

                                while (Count < NewCount)
                                {
                                    KernelTypes::ConstructAt(Items + Count);
                                    ++Count;
                                }
                            }

                            void Resize(KernelTypes::Size NewCount, const T& Value)
                            {
                                if (NewCount < Count)
                                {
                                    while (Count > NewCount)
                                        PopBack();
                                    return;
                                }

                                if (NewCount == Count)
                                    return;

                                T ValueCopy(Value);
                                if (NewCount > Capacity)
                                    Reserve(NewCount);

                                while (Count < NewCount)
                                {
                                    KernelTypes::ConstructAt(Items + Count, ValueCopy);
                                    ++Count;
                                }
                            }

                            T* Find(const T& Value)
                            {
                                for (KernelTypes::Size Index = 0; Index < Count; ++Index)
                                {
                                    if (Items[Index] == Value)
                                        return Items + Index;
                                }

                                return nullptr;
                            }

                            const T* Find(const T& Value) const
                            {
                                for (KernelTypes::Size Index = 0; Index < Count; ++Index)
                                {
                                    if (Items[Index] == Value)
                                        return Items + Index;
                                }

                                return nullptr;
                            }

                            void RemoveAt(KernelTypes::Size Index)
                            {
                                if (Index >= Count)
                                    KernelAssert::Fail("Array index is out of range");

                                KernelTypes::DestroyAt(Items + Index);
                                for (KernelTypes::Size Current = Index; Current + 1 < Count; ++Current)
                                {
                                    KernelTypes::ConstructAt(Items + Current, KernelTypes::Move(Items[Current + 1]));
                                    KernelTypes::DestroyAt(Items + Current + 1);
                                }
                                --Count;
                            }

                            void Erase(KernelTypes::Size Index)
                            {
                                RemoveAt(Index);
                            }

                            // Integral indices must win over the pointer overload for null
                            // pointer constants such as Erase(0).
                            template <KernelTypes::Integral IndexType>
                            void Erase(IndexType Index)
                            {
                                if constexpr (KernelTypes::IsSigned<IndexType>)
                                {
                                    if (Index < 0)
                                        KernelAssert::Fail("Array index is out of range");
                                }

                                if constexpr (sizeof(IndexType) > sizeof(KernelTypes::Size))
                                {
                                    if (Index > static_cast<IndexType>(KernelTypes::MaxValue<KernelTypes::Size>()))
                                        KernelAssert::Fail("Array index is out of range");
                                }

                                RemoveAt(static_cast<KernelTypes::Size>(Index));
                            }

                            void Insert(KernelTypes::Size Index, const T& Value)
                            {
                                KernelAssert::DebugCheck(Index <= Count, "Array::Insert: индекс вне диапазона");
                                T ValueCopy(Value);

                                if (Index == Count)
                                {
                                    Add(KernelTypes::Move(ValueCopy));
                                    return;
                                }

                                if (Count == Capacity)
                                    Reserve(GetGrownCapacity());

                                KernelTypes::ConstructAt(Items + Count, KernelTypes::Move(Items[Count - 1]));
                                KernelTypes::DestroyAt(Items + Count - 1);
                                ++Count;
                                for (KernelTypes::Size Current = Count - 2; Current > Index; --Current)
                                {
                                    KernelTypes::ConstructAt(Items + Current, KernelTypes::Move(Items[Current - 1]));
                                    KernelTypes::DestroyAt(Items + Current - 1);
                                }
                                // Слот Index уже разрушен соответствующим сдвигом; повторно вызывать деструктор нельзя.
                                KernelTypes::ConstructAt(Items + Index, KernelTypes::Move(ValueCopy));
                            }

                            void Insert(KernelTypes::Size Index, T&& Value)
                            {
                                KernelAssert::DebugCheck(Index <= Count, "Array::Insert: индекс вне диапазона");
                                T ValueCopy(KernelTypes::Move(Value));

                                if (Index == Count)
                                {
                                    Add(KernelTypes::Move(ValueCopy));
                                    return;
                                }

                                if (Count == Capacity)
                                    Reserve(GetGrownCapacity());

                                KernelTypes::ConstructAt(Items + Count, KernelTypes::Move(Items[Count - 1]));
                                KernelTypes::DestroyAt(Items + Count - 1);
                                ++Count;
                                for (KernelTypes::Size Current = Count - 2; Current > Index; --Current)
                                {
                                    KernelTypes::ConstructAt(Items + Current, KernelTypes::Move(Items[Current - 1]));
                                    KernelTypes::DestroyAt(Items + Current - 1);
                                }
                                // Слот Index уже разрушен соответствующим сдвигом; повторно вызывать деструктор нельзя.
                                KernelTypes::ConstructAt(Items + Index, KernelTypes::Move(ValueCopy));
                            }

                            // Удалить диапазон элементов и вернуть позицию, с которой сдвинуты оставшиеся элементы.
                            Iterator Erase(ConstIterator First, ConstIterator Last)
                            {
                                if (Items == nullptr)
                                {
                                    KernelAssert::DebugCheck(First == nullptr && Last == nullptr, "Array::Erase: неверный диапазон пустого массива");
                                    return nullptr;
                                }
                                KernelAssert::DebugCheck(First >= Begin() && First <= Last && Last <= End(), "Array::Erase: неверный диапазон");
                                if (First == Last)
                                    return const_cast<Iterator>(First);
                                const KernelTypes::Size Index = static_cast<KernelTypes::Size>(First - Begin());
                                const KernelTypes::Size RemoveCount = static_cast<KernelTypes::Size>(Last - First);
                                for (KernelTypes::Size Offset = 0; Offset < RemoveCount; ++Offset)
                                    RemoveAt(Index);
                                return Begin() + Index;
                            }

                            Iterator Erase(ConstIterator Position)
                            {
                                return Erase(Position, Position + 1);
                            }

                            template <typename InputIterator>
                            void Insert(ConstIterator Position, InputIterator First, InputIterator Last)
                            {
                                if (Items == nullptr)
                                    KernelAssert::DebugCheck(Position == nullptr, "Array::Insert: позиция вне пустого массива");
                                else
                                    KernelAssert::DebugCheck(Position >= Begin() && Position <= End(), "Array::Insert: позиция вне массива");
                                Array<T> Values;
                                for (; First != Last; ++First)
                                    Values.Add(*First);
                                if (Values.IsEmpty())
                                    return;

                                const KernelTypes::Size Index = Items == nullptr
                                    ? 0
                                    : static_cast<KernelTypes::Size>(Position - Items);
                                if (Values.Count > KernelTypes::MaxValue<KernelTypes::Size>() - Count)
                                    KernelAssert::Fail("Array size is too large");
                                const KernelTypes::Size NewCount = Count + Values.Count;
                                if (NewCount > Capacity)
                                    Reserve(NewCount);

                                const KernelTypes::Size OldCount = Count;
                                for (KernelTypes::Size Current = OldCount; Current > Index; --Current)
                                {
                                    KernelTypes::ConstructAt(Items + Current + Values.Count - 1, KernelTypes::Move(Items[Current - 1]));
                                    KernelTypes::DestroyAt(Items + Current - 1);
                                }
                                for (KernelTypes::Size Offset = 0; Offset < Values.Count; ++Offset)
                                    KernelTypes::ConstructAt(Items + Index + Offset, KernelTypes::Move(Values.Items[Offset]));
                                Count = NewCount;
                            }

                            template <typename InputIterator>
                            void Assign(InputIterator First, InputIterator Last)
                            {
                                Array<T> Values;
                                for (; First != Last; ++First)
                                    Values.Add(*First);
                                *this = KernelTypes::Move(Values);
                            }

                            void ShrinkToFit()
                            {
                                if (Count == Capacity)
                                    return;
                                if (Count == 0)
                                {
                                    DeallocateItems(Items);
                                    Items = nullptr;
                                    Capacity = 0;
                                    return;
                                }

                                T* NewItems = AllocateItems(Count);
                                KernelTypes::Size Constructed = 0;

                                    for (; Constructed < Count; ++Constructed)
                                        KernelTypes::ConstructAt(NewItems + Constructed, KernelTypes::MoveIfNoexcept(Items[Constructed]));

                                for (KernelTypes::Size Index = 0; Index < Count; ++Index)
                                    KernelTypes::DestroyAt(Items + Index);
                                DeallocateItems(Items);
                                Items = NewItems;
                                Capacity = Count;
                            }

                            void Swap(Array& Other) noexcept
                            {
                                KernelTypes::Swap(Items, Other.Items);
                                KernelTypes::Swap(Count, Other.Count);
                                KernelTypes::Swap(Capacity, Other.Capacity);
                            }

                            [[nodiscard]] ReverseIterator rbegin() noexcept { return ReverseIterator(Count == 0 ? Items : Items + Count); }
                            [[nodiscard]] ReverseIterator rend() noexcept { return ReverseIterator(Items); }
                            [[nodiscard]] ConstReverseIterator rbegin() const noexcept { return ConstReverseIterator(Count == 0 ? Items : Items + Count); }
                            [[nodiscard]] ConstReverseIterator rend() const noexcept { return ConstReverseIterator(Items); }
                            [[nodiscard]] ConstReverseIterator crbegin() const noexcept { return rbegin(); }
                            [[nodiscard]] ConstReverseIterator crend() const noexcept { return rend(); }
                            [[nodiscard]] const T* cbegin() const noexcept { return Begin(); }
                            [[nodiscard]] const T* cend() const noexcept { return End(); }
                            [[nodiscard]] T* data() noexcept { return Items; }
                            [[nodiscard]] const T* data() const noexcept { return Items; }
                            [[nodiscard]] T& At(KernelTypes::Size Index) { return (*this)[Index]; }
                            [[nodiscard]] const T& At(KernelTypes::Size Index) const { return (*this)[Index]; }
                            [[nodiscard]] KernelTypes::Size size() const noexcept { return Count; }
                            [[nodiscard]] KernelTypes::Size capacity() const noexcept { return Capacity; }
                            [[nodiscard]] KernelTypes::Bool empty() const noexcept { return Count == 0; }

                            [[nodiscard]] KernelTypes::Bool operator==(const Array& Other) const
                            {
                                if (Count != Other.Count)
                                    return false;
                                for (KernelTypes::Size Index = 0; Index < Count; ++Index)
                                    if (!(Items[Index] == Other.Items[Index]))
                                        return false;
                                return true;
                            }

                            [[nodiscard]] KernelTypes::Bool operator!=(const Array& Other) const { return !(*this == Other); }

                            [[nodiscard]] KernelTypes::UInt64 GetHash() const
                            {
                                KernelTypes::UInt64 Result = Hash<KernelTypes::UInt32>{}(static_cast<KernelTypes::UInt32>(Count));
                                for (KernelTypes::Size Index = 0; Index < Count; ++Index)
                                    HashCombine(Result, Items[Index]);
                                return Result;
                            }

                            void Clear() noexcept
                            {
                                for (KernelTypes::Size Index = Count; Index > 0; Index--)
                                    KernelTypes::DestroyAt(Items + Index - 1);

                                Count = 0;
                            }
                    };
                }
            }
        }
    }
}
