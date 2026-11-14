#pragma once

#include "Empire.Kernel/Kernel.Assert/Assert.hpp"
#include "Empire.Kernel/Kernel.Memory/Memory.hpp"
#include "Empire.Kernel/Kernel.Types/Types.Utility.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireKernel
        {
            namespace KernelContainers
            {
                template <typename T>
                class Array
                {
                    private:
                        T* Items = nullptr;
                        KernelTypes::Size Count = 0;
                        KernelTypes::Size Capacity = 0;

                        static T* AllocateItems(KernelTypes::Size ItemCount)
                        {
                            if (ItemCount > KernelTypes::MaxValue<KernelTypes::Size>() / sizeof(T))
                                throw KernelAssert::LengthError("Array capacity is too large");

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
                                throw KernelAssert::LengthError("Array capacity is too large");

                            return Capacity * 2;
                        }

                    public:
                        Array() = default;

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

                            try
                            {
                                for (; Count < Other.Count; Count++)
                                    KernelTypes::ConstructAt(Items + Count, Other.Items[Count]);
                            }
                            catch (...)
                            {
                                for (KernelTypes::Size Index = 0; Index < Count; Index++)
                                    KernelTypes::DestroyAt(Items + Index);

                                DeallocateItems(Items);
                                Items = nullptr;
                                Count = 0;
                                Capacity = 0;
                                throw;
                            }
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

                        T* Begin() noexcept { return Items; }
                        T* End() noexcept { return Items + Count; }
                        const T* Begin() const noexcept { return Items; }
                        const T* End() const noexcept { return Items + Count; }

                        // Нижний регистр нужен языку для range-for (for (auto& Item : Array)).
                        T* begin() noexcept { return Items; }
                        T* end() noexcept { return Items + Count; }
                        const T* begin() const noexcept { return Items; }
                        const T* end() const noexcept { return Items + Count; }

                        T& operator[](KernelTypes::Size Index)
                        {
                            if (Index >= Count)
                                throw KernelAssert::OutOfRangeError("Array index is out of range");

                            return Items[Index];
                        }

                        const T& operator[](KernelTypes::Size Index) const
                        {
                            if (Index >= Count)
                                throw KernelAssert::OutOfRangeError("Array index is out of range");

                            return Items[Index];
                        }

                        void Reserve(KernelTypes::Size NewCapacity)
                        {
                            if (NewCapacity <= Capacity)
                                return;

                            T* NewItems = AllocateItems(NewCapacity);
                            KernelTypes::Size Constructed = 0;

                            try
                            {
                                for (; Constructed < Count; Constructed++)
                                    KernelTypes::ConstructAt(NewItems + Constructed, KernelTypes::MoveIfNoexcept(Items[Constructed]));
                            }
                            catch (...)
                            {
                                for (KernelTypes::Size Index = 0; Index < Constructed; Index++)
                                    KernelTypes::DestroyAt(NewItems + Index);

                                DeallocateItems(NewItems);
                                throw;
                            }

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

                        void RemoveAt(KernelTypes::Size Index)
                        {
                            if (Index >= Count)
                                throw KernelAssert::OutOfRangeError("Array index is out of range");

                            for (KernelTypes::Size Current = Index; Current + 1 < Count; Current++)
                                Items[Current] = KernelTypes::Move(Items[Current + 1]);

                            Count--;
                            KernelTypes::DestroyAt(Items + Count);
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
