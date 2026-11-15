#pragma once

#include "Empire.Kernel/Kernel.Assert/Assert.hpp"
#include "Empire.Kernel/Kernel.Memory/Memory.hpp"
#include "Empire.Kernel/Kernel.Types/Types.Utility.hpp"

#include <initializer_list>

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EngineKernel
        {
            namespace Containers
            {
                template <typename T>
                class Array
                {
                    private:
                        T* Items = nullptr;
                        Types::Size Count = 0;
                        Types::Size Capacity = 0;

                        static T* AllocateItems(Types::Size ItemCount)
                        {
                            if (ItemCount > Types::MaxValue<Types::Size>() / sizeof(T))
                                throw Assert::LengthError("Array capacity is too large");

                            return static_cast<T*>(Memory::Allocate(ItemCount * sizeof(T), alignof(T)));
                        }

                        static void DeallocateItems(T* MemoryBlock) noexcept
                        {
                            Memory::Deallocate(MemoryBlock);
                        }

                        Types::Size GetGrownCapacity() const
                        {
                            if (Capacity == 0)
                                return 4;

                            if (Capacity > Types::MaxValue<Types::Size>() / 2)
                                throw Assert::LengthError("Array capacity is too large");

                            return Capacity * 2;
                        }

                    public:
                        Array() = default;

                        Array(std::initializer_list<T> Values)
                        {
                            if (Values.size() == 0)
                                return;

                            Items = AllocateItems(Values.size());
                            Capacity = Values.size();

                            try
                            {
                                for (const T& Value : Values)
                                {
                                    Types::ConstructAt(Items + Count, Value);
                                    Count++;
                                }
                            }
                            catch (...)
                            {
                                for (Types::Size Index = 0; Index < Count; Index++)
                                    Types::DestroyAt(Items + Index);

                                DeallocateItems(Items);
                                Items = nullptr;
                                Count = 0;
                                Capacity = 0;
                                throw;
                            }
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
                                    Types::ConstructAt(Items + Count, Other.Items[Count]);
                            }
                            catch (...)
                            {
                                for (Types::Size Index = 0; Index < Count; Index++)
                                    Types::DestroyAt(Items + Index);

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
                            *this = Types::Move(Copy);
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

                        Types::Size GetCount() const noexcept { return Count; }
                        Types::Size GetCapacity() const noexcept { return Capacity; }
                        Types::Bool IsEmpty() const noexcept { return Count == 0; }
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

                        T& operator[](Types::Size Index)
                        {
                            if (Index >= Count)
                                throw Assert::OutOfRangeError("Array index is out of range");

                            return Items[Index];
                        }

                        const T& operator[](Types::Size Index) const
                        {
                            if (Index >= Count)
                                throw Assert::OutOfRangeError("Array index is out of range");

                            return Items[Index];
                        }

                        void Reserve(Types::Size NewCapacity)
                        {
                            if (NewCapacity <= Capacity)
                                return;

                            T* NewItems = AllocateItems(NewCapacity);
                            Types::Size Constructed = 0;

                            try
                            {
                                for (; Constructed < Count; Constructed++)
                                    Types::ConstructAt(NewItems + Constructed, Types::MoveIfNoexcept(Items[Constructed]));
                            }
                            catch (...)
                            {
                                for (Types::Size Index = 0; Index < Constructed; Index++)
                                    Types::DestroyAt(NewItems + Index);

                                DeallocateItems(NewItems);
                                throw;
                            }

                            for (Types::Size Index = 0; Index < Count; Index++)
                                Types::DestroyAt(Items + Index);

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
                                Types::ConstructAt(Items + Count, Types::Move(Copy));
                            }
                            else
                            {
                                Types::ConstructAt(Items + Count, Value);
                            }

                            Count++;
                        }

                        void Add(T&& Value)
                        {
                            if (Count == Capacity)
                            {
                                T Copy(Types::Move(Value));
                                Reserve(GetGrownCapacity());
                                Types::ConstructAt(Items + Count, Types::Move(Copy));
                            }
                            else
                            {
                                Types::ConstructAt(Items + Count, Types::Move(Value));
                            }

                            Count++;
                        }

                        void RemoveAt(Types::Size Index)
                        {
                            if (Index >= Count)
                                throw Assert::OutOfRangeError("Array index is out of range");

                            for (Types::Size Current = Index; Current + 1 < Count; Current++)
                                Items[Current] = Types::Move(Items[Current + 1]);

                            Count--;
                            Types::DestroyAt(Items + Count);
                        }

                        void Clear() noexcept
                        {
                            for (Types::Size Index = Count; Index > 0; Index--)
                                Types::DestroyAt(Items + Index - 1);

                            Count = 0;
                        }
                };
            }
        }
    }
}
