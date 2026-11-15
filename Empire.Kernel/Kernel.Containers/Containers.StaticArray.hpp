#pragma once

#include "Empire.Kernel/Kernel.Assert/Assert.hpp"
#include "Empire.Kernel/Kernel.Containers/Containers.Hash.hpp"
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
                // Массив переменной длины на буфере фиксированного размера, без выделения памяти
                // (перенесён из Jolt, Core/StaticArray.h). Элементы живут прямо внутри объекта.
                // Проверки, как у FastArray, только в отладочной сборке.
                template <typename T, KernelTypes::UInt32 N>
                class [[nodiscard]] StaticArray
                {
                    public:
                        using ValueType = T;
                        using SizeType = KernelTypes::UInt32;

                        using ConstIterator = const T*;
                        using Iterator = T*;

                        static constexpr KernelTypes::UInt32 MaxCount = N;

                        // Элементы специально не создаются
                        StaticArray() { }

                        StaticArray(const StaticArray& Other)
                        {
                            while (Count < Other.Count)
                            {
                                KernelTypes::ConstructAt(&Elements[Count], Other[Count]);
                                ++Count;
                            }
                        }

                        ~StaticArray()
                        {
                            DestroyAll();
                        }

                        // Замена списку инициализации (в Kernel нет std::initializer_list): StaticArray<int, 8>::Of(1, 2, 3)
                        template <typename... Values>
                        [[nodiscard]] static StaticArray Of(Values&&... Items)
                        {
                            static_assert(sizeof...(Values) <= N, "StaticArray::Of: элементов больше, чем вмещает массив");

                            StaticArray Result;
                            (Result.EmplaceBack(KernelTypes::Forward<Values>(Items)), ...);
                            return Result;
                        }

                        // Вызывает деструкторы всех элементов, длина становится нулевой
                        void Clear()
                        {
                            DestroyAll();
                            Count = 0;
                        }

                        // Добавляет элемент в конец
                        void PushBack(const T& Value)
                        {
                            KernelAssert::DebugCheck(Count < N, "StaticArray::PushBack: массив заполнен");
                            KernelTypes::ConstructAt(&Elements[Count++], Value);
                        }

                        // Создаёт элемент на месте в конце массива
                        template <typename... Arguments>
                        void EmplaceBack(Arguments&&... Args)
                        {
                            KernelAssert::DebugCheck(Count < N, "StaticArray::EmplaceBack: массив заполнен");
                            KernelTypes::ConstructAt(&Elements[Count++], KernelTypes::Forward<Arguments>(Args)...);
                        }

                        // Убирает последний элемент
                        void PopBack()
                        {
                            KernelAssert::DebugCheck(Count > 0, "StaticArray::PopBack: массив пуст");
                            Elements[--Count].~T();
                        }

                        [[nodiscard]] bool Empty() const { return Count == 0; }
                        [[nodiscard]] SizeType Size() const { return Count; }
                        [[nodiscard]] SizeType GetCapacity() const { return N; }

                        // Меняет длину массива. Новые элементы создаются конструктором по умолчанию
                        // (у тривиальных типов значения не определены).
                        void Resize(SizeType NewSize)
                        {
                            KernelAssert::DebugCheck(NewSize <= N, "StaticArray::Resize: новая длина больше ёмкости");

                            if constexpr (!KernelTypes::IsTriviallyConstructible<T>)
                            {
                                for (T *Element = Data() + Count, *ElementEnd = Data() + NewSize; Element < ElementEnd; ++Element)
                                {
                                    ::new (KernelTypes::PlacementTag{}, static_cast<void*>(Element)) T;
                                }
                            }

                            if constexpr (!KernelTypes::IsTriviallyDestructible<T>)
                            {
                                for (T *Element = Data() + NewSize, *ElementEnd = Data() + Count; Element < ElementEnd; ++Element)
                                {
                                    Element->~T();
                                }
                            }

                            Count = NewSize;
                        }

                        ConstIterator begin() const { return Data(); }
                        ConstIterator end() const { return Data() + Count; }
                        Iterator begin() { return Data(); }
                        Iterator end() { return Data() + Count; }

                        const T* Data() const { return Elements; }
                        T* Data() { return Elements; }

                        T& operator[](SizeType Index)
                        {
                            KernelAssert::DebugCheck(Index < Count, "StaticArray: индекс вне массива");
                            return Elements[Index];
                        }

                        const T& operator[](SizeType Index) const
                        {
                            KernelAssert::DebugCheck(Index < Count, "StaticArray: индекс вне массива");
                            return Elements[Index];
                        }

                        T& At(SizeType Index)
                        {
                            KernelAssert::DebugCheck(Index < Count, "StaticArray::At: индекс вне массива");
                            return Elements[Index];
                        }

                        const T& At(SizeType Index) const
                        {
                            KernelAssert::DebugCheck(Index < Count, "StaticArray::At: индекс вне массива");
                            return Elements[Index];
                        }

                        const T& Front() const
                        {
                            KernelAssert::DebugCheck(Count > 0, "StaticArray::Front: массив пуст");
                            return Elements[0];
                        }

                        T& Front()
                        {
                            KernelAssert::DebugCheck(Count > 0, "StaticArray::Front: массив пуст");
                            return Elements[0];
                        }

                        const T& Back() const
                        {
                            KernelAssert::DebugCheck(Count > 0, "StaticArray::Back: массив пуст");
                            return Elements[Count - 1];
                        }

                        T& Back()
                        {
                            KernelAssert::DebugCheck(Count > 0, "StaticArray::Back: массив пуст");
                            return Elements[Count - 1];
                        }

                        // Убирает один элемент
                        void Erase(ConstIterator Position)
                        {
                            const SizeType Index = static_cast<SizeType>(Position - begin());
                            KernelAssert::DebugCheck(Index < Count, "StaticArray::Erase: индекс вне массива");

                            Elements[Index].~T();
                            MoveDown(Index + 1, Index, Count - Index - 1);
                            --Count;
                        }

                        // Убирает несколько элементов
                        void Erase(ConstIterator Begin, ConstIterator End)
                        {
                            const SizeType Index = static_cast<SizeType>(Begin - begin());
                            const SizeType RemovedCount = static_cast<SizeType>(End - Begin);
                            KernelAssert::DebugCheck(End <= end(), "StaticArray::Erase: конец диапазона вне массива");

                            for (SizeType Offset = 0; Offset < RemovedCount; ++Offset)
                            {
                                Elements[Index + Offset].~T();
                            }

                            MoveDown(Index + RemovedCount, Index, Count - Index - RemovedCount);
                            Count -= RemovedCount;
                        }

                        StaticArray& operator=(const StaticArray& Other)
                        {
                            if (this != &Other)
                            {
                                Clear();

                                while (Count < Other.Count)
                                {
                                    KernelTypes::ConstructAt(&Elements[Count], Other[Count]);
                                    ++Count;
                                }
                            }

                            return *this;
                        }

                        // Присваивание массива другой максимальной длины
                        template <KernelTypes::UInt32 M>
                        StaticArray& operator=(const StaticArray<T, M>& Other)
                        {
                            KernelAssert::DebugCheck(Other.Size() <= N, "StaticArray::operator=: источник длиннее ёмкости");

                            if (static_cast<const void*>(this) != static_cast<const void*>(&Other))
                            {
                                Clear();

                                while (Count < Other.Size())
                                {
                                    KernelTypes::ConstructAt(&Elements[Count], Other[Count]);
                                    ++Count;
                                }
                            }

                            return *this;
                        }

                        bool operator==(const StaticArray& Other) const
                        {
                            if (Count != Other.Count)
                            {
                                return false;
                            }

                            for (SizeType Index = 0; Index < Count; ++Index)
                            {
                                if (!(Elements[Index] == Other.Elements[Index]))
                                {
                                    return false;
                                }
                            }

                            return true;
                        }

                        bool operator!=(const StaticArray& Other) const
                        {
                            return !(*this == Other);
                        }

                        // Хеш массива: сначала длина, потом элементы по порядку
                        [[nodiscard]] KernelTypes::UInt64 GetHash() const
                        {
                            KernelTypes::UInt64 Result = Hash<KernelTypes::UInt32>{}(Count);

                            for (const T *Element = Data(), *ElementEnd = Data() + Count; Element < ElementEnd; ++Element)
                            {
                                HashCombine(Result, *Element);
                            }

                            return Result;
                        }

                    private:
                        void DestroyAll()
                        {
                            if constexpr (!KernelTypes::IsTriviallyDestructible<T>)
                            {
                                for (T *Element = Data(), *ElementEnd = Data() + Count; Element < ElementEnd; ++Element)
                                {
                                    Element->~T();
                                }
                            }
                        }

                        // Сдвигает Amount элементов с Source на Destination (области могут пересекаться).
                        // В Jolt здесь был memmove, который для нетривиальных типов некорректен, поэтому перенос поэлементный.
                        void MoveDown(SizeType Source, SizeType Destination, SizeType Amount)
                        {
                            if (Amount == 0)
                            {
                                return;
                            }

                            if constexpr (KernelTypes::IsTriviallyCopyable<T>)
                            {
                                KernelMemory::MoveBytes(&Elements[Destination], &Elements[Source], Amount * sizeof(T));
                            }
                            else
                            {
                                // Source > Destination, поэтому идём слева направо
                                for (SizeType Offset = 0; Offset < Amount; ++Offset)
                                {
                                    KernelTypes::ConstructAt(&Elements[Destination + Offset], KernelTypes::Move(Elements[Source + Offset]));
                                    Elements[Source + Offset].~T();
                                }
                            }
                        }

                    protected:
                        SizeType Count = 0;

                        // Объединение не даёт компилятору создавать и разрушать Elements сам
                        union
                        {
                            T Elements[N];
                            KernelTypes::UInt8 Dummy;
                        };
                };
            }
        }
    }
}
