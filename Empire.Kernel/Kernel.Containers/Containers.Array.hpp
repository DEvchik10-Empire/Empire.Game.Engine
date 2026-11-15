#pragma once

#include "Empire.Kernel/Kernel.Types/Types.hpp"

#include <initializer_list>
#include <limits>
#include <memory>
#include <new>
#include <stdexcept>
#include <utility>

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
                            return std::allocator<T>{}.allocate(ItemCount);
                        }

                        static void DeallocateItems(T* MemoryBlock, Types::Size ItemCount) noexcept
                        {
                            if (MemoryBlock != nullptr)
                                std::allocator<T>{}.deallocate(MemoryBlock, ItemCount);
                        }

                        void Reserve(Types::Size NewCapacity)
                        {
                            if (NewCapacity <= Capacity)
                                return;

                            if (NewCapacity > std::numeric_limits<Types::Size>::max() / sizeof(T))
                                throw std::length_error("Array capacity is too large");

                            T* NewItems = AllocateItems(NewCapacity);
                            Types::Size Constructed = 0;

                            try
                            {
                                for (; Constructed < Count; Constructed++)
                                    std::construct_at(
                                        NewItems + Constructed,
                                        std::move_if_noexcept(Items[Constructed])
                                    );
                            }
                            catch (...)
                            {
                                for (Types::Size Index = 0; Index < Constructed; Index++)
                                    std::destroy_at(NewItems + Index);

                                DeallocateItems(NewItems, NewCapacity);
                                throw;
                            }

                            for (Types::Size Index = 0; Index < Count; Index++)
                                std::destroy_at(Items + Index);

                            DeallocateItems(Items, Capacity);
                            Items = NewItems;
                            Capacity = NewCapacity;
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
                                    std::construct_at(Items + Count, Value);
                                    Count++;
                                }
                            }
                            catch (...)
                            {
                                for (Types::Size Index = 0; Index < Count; Index++)
                                    std::destroy_at(Items + Index);

                                DeallocateItems(Items, Capacity);
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
                                    std::construct_at(Items + Count, Other.Items[Count]);
                            }
                            catch (...)
                            {
                                for (Types::Size Index = 0; Index < Count; Index++)
                                    std::destroy_at(Items + Index);

                                DeallocateItems(Items, Capacity);
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
                            DeallocateItems(Items, Capacity);
                        }

                        Array& operator=(const Array& Other)
                        {
                            if (this == &Other)
                                return *this;

                            Array Copy(Other);
                            *this = std::move(Copy);
                            return *this;
                        }

                        Array& operator=(Array&& Other) noexcept
                        {
                            if (this == &Other)
                                return *this;

                            Clear();
                            DeallocateItems(Items, Capacity);

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

                        T& operator[](Types::Size Index)
                        {
                            if (Index >= Count)
                                throw std::out_of_range("Array index is out of range");

                            return Items[Index];
                        }

                        const T& operator[](Types::Size Index) const
                        {
                            if (Index >= Count)
                                throw std::out_of_range("Array index is out of range");

                            return Items[Index];
                        }

                        void Add(const T& Value)
                        {
                            if (Count == Capacity)
                            {
                                T Copy(Value);
                                Reserve(Capacity == 0 ? 4 : Capacity > std::numeric_limits<Types::Size>::max() / 2
                                    ? throw std::length_error("Array capacity is too large")
                                    : Capacity * 2);
                                std::construct_at(Items + Count, std::move(Copy));
                            }
                            else
                            {
                                std::construct_at(Items + Count, Value);
                            }

                            Count++;
                        }

                        void Add(T&& Value)
                        {
                            if (Count == Capacity)
                            {
                                T Copy(std::move(Value));
                                Reserve(Capacity == 0 ? 4 : Capacity > std::numeric_limits<Types::Size>::max() / 2
                                    ? throw std::length_error("Array capacity is too large")
                                    : Capacity * 2);
                                std::construct_at(Items + Count, std::move(Copy));
                            }
                            else
                            {
                                std::construct_at(Items + Count, std::move(Value));
                            }

                            Count++;
                        }

                        void RemoveAt(Types::Size Index)
                        {
                            if (Index >= Count)
                                throw std::out_of_range("Array index is out of range");

                            for (Types::Size Current = Index; Current + 1 < Count; Current++)
                                Items[Current] = std::move(Items[Current + 1]);

                            Count--;
                            std::destroy_at(Items + Count);
                        }

                        void Clear() noexcept
                        {
                            for (Types::Size Index = Count; Index > 0; Index--)
                                std::destroy_at(Items + Index - 1);

                            Count = 0;
                        }
                };
            }
        }
    }
}
