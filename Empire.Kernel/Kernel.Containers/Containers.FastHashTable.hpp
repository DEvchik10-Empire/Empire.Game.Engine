#pragma once

#include "Empire.Kernel/Kernel.Algorithms/Algorithms.Compare.hpp"
#include "Empire.Kernel/Kernel.Assert/Assert.hpp"
#include "Empire.Kernel/Kernel.Compiler/Compiler.hpp"
#include "Empire.Kernel/Kernel.Containers/Containers.Hash.hpp"
#include "Empire.Kernel/Kernel.Memory/Memory.hpp"
#include "Empire.Kernel/Kernel.Types/Types.Pair.hpp"
#include "Empire.Kernel/Kernel.Types/Types.Utility.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireKernel
        {
            namespace KernelContainers
            {
                // Основа для FastHashMap и FastHashSet (перенесена из Jolt, Core/HashTable.h).
                // Открытая адресация, корзины проверяются группами по 16 штук за одну SIMD-операцию.
                // Основано на докладе CppCon 2017, Matt Kulukundis "Designing a Fast, Efficient, Cache-friendly Hash Table".
                // https://www.youtube.com/watch?v=ncHmEUmJZf4
                //
                // Устройство: после массива корзин лежат контрольные байты (по одному на корзину) и ещё 15 байт,
                // которые повторяют первые 15, чтобы группу из 16 байт можно было читать с любого места без перехода через край.
                // Контрольный байт: 0 пустая, 0x7f удалена, 0x80 | 7 младших бит хеша занята.
                //
                // Порядок элементов при обходе не определён. Допустимая загрузка 7/8.
                template <typename Key, typename KeyValue, typename HashTableDetail, typename HashType, typename KeyEqual>
                class FastHashTable
                {
                    public:
                        using ValueType = KeyValue;
                        using SizeType = KernelTypes::UInt32;
                        using DifferenceType = decltype(static_cast<KeyValue*>(nullptr) - static_cast<KeyValue*>(nullptr));

                    private:
                        // Общая часть итераторов
                        template <typename Table, typename IteratorType>
                        class IteratorBase
                        {
                            public:
                                using DifferenceType = typename Table::DifferenceType;
                                using ValueType = typename Table::ValueType;

                                IteratorBase(const IteratorBase&) = default;
                                IteratorBase& operator=(const IteratorBase&) = default;

                                // Итератор на начало таблицы
                                explicit IteratorBase(Table* TablePointer)
                                    : OwnerTable(TablePointer), Index(0)
                                {
                                    while (Index < OwnerTable->MaxSize && (OwnerTable->Control[Index] & BucketUsed) == 0)
                                    {
                                        ++Index;
                                    }
                                }

                                // Итератор на конкретную корзину
                                IteratorBase(Table* TablePointer, SizeType BucketIndex)
                                    : OwnerTable(TablePointer), Index(BucketIndex)
                                {
                                }

                                IteratorType& operator++()
                                {
                                    KernelAssert::DebugCheck(IsValid(), "FastHashTable: инкремент недействительного итератора");

                                    do
                                    {
                                        ++Index;
                                    }
                                    while (Index < OwnerTable->MaxSize && (OwnerTable->Control[Index] & BucketUsed) == 0);

                                    return static_cast<IteratorType&>(*this);
                                }

                                IteratorType operator++(int)
                                {
                                    IteratorType Result(OwnerTable, Index);
                                    ++(*this);
                                    return Result;
                                }

                                const KeyValue& operator*() const
                                {
                                    KernelAssert::DebugCheck(IsValid(), "FastHashTable: разыменование недействительного итератора");
                                    return OwnerTable->Data[Index];
                                }

                                const KeyValue* operator->() const
                                {
                                    KernelAssert::DebugCheck(IsValid(), "FastHashTable: разыменование недействительного итератора");
                                    return OwnerTable->Data + Index;
                                }

                                bool operator==(const IteratorType& Other) const
                                {
                                    return Index == Other.Index && OwnerTable == Other.OwnerTable;
                                }

                                bool operator!=(const IteratorType& Other) const
                                {
                                    return !(*this == Other);
                                }

                                // Указывает ли итератор на занятую корзину
                                bool IsValid() const
                                {
                                    return Index < OwnerTable->MaxSize && (OwnerTable->Control[Index] & BucketUsed) != 0;
                                }

                                Table* OwnerTable;
                                SizeType Index;
                        };

                        // Сколько элементов помещается в таблицу с таким числом корзин
                        static constexpr SizeType GetMaxLoad(SizeType BucketCount)
                        {
                            return static_cast<SizeType>((MaxLoadFactorNumerator * BucketCount) / MaxLoadFactorDenominator);
                        }

                        // Записать контрольный байт корзины
                        void SetControlValue(SizeType BucketIndex, KernelTypes::UInt8 Value)
                        {
                            KernelAssert::DebugCheck(BucketIndex < MaxSize, "FastHashTable::SetControlValue: индекс за пределами таблицы");
                            Control[BucketIndex] = Value;

                            // Первые 15 байт дублируются в 15 байт после MaxSize. Это равносильно
                            //   if (BucketIndex < 15) Control[BucketIndex + MaxSize] = Value; else Control[BucketIndex] = Value;
                            // но без ветвления (при BucketIndex >= 15 лишняя запись безвредна).
                            Control[((BucketIndex - 15) & (MaxSize - 1)) + 15] = Value;
                        }

                        // Индекс корзины и контрольный байт для ключа
                        void GetIndexAndControlValue(const Key& KeyToHash, SizeType& OutIndex, KernelTypes::UInt8& OutControl) const
                        {
                            KernelTypes::UInt64 HashValue = HashType{}(KeyToHash);

                            OutIndex = static_cast<SizeType>(HashValue >> 7) & (MaxSize - 1);
                            OutControl = static_cast<KernelTypes::UInt8>(BucketUsed | static_cast<KernelTypes::UInt8>(HashValue));
                        }

                        // Выделить память под таблицу
                        void AllocateTable(SizeType NewMaxSize)
                        {
                            KernelAssert::DebugCheck(Data == nullptr, "FastHashTable::AllocateTable: таблица уже выделена");

                            MaxSize = NewMaxSize;
                            LoadLeft = GetMaxLoad(NewMaxSize);

                            // +15 байт под повтор первых 15 контрольных байт
                            KernelTypes::Size RequiredSize = static_cast<KernelTypes::Size>(MaxSize) * (sizeof(KeyValue) + 1) + 15;
                            Data = static_cast<KeyValue*>(KernelMemory::Allocate(RequiredSize, AllocationAlignment));
                            Control = reinterpret_cast<KernelTypes::UInt8*>(Data + MaxSize);
                        }

                        // Скопировать содержимое другой таблицы
                        void CopyTable(const FastHashTable& Other)
                        {
                            if (Other.Empty())
                            {
                                return;
                            }

                            AllocateTable(Other.MaxSize);

                            KernelMemory::CopyBytes(Control, Other.Control, MaxSize + 15);

                            SizeType Index = 0;
                            for (const KernelTypes::UInt8* Current = Control, *End = Control + MaxSize; Current != End; ++Current, ++Index)
                            {
                                if (*Current & BucketUsed)
                                {
                                    KernelTypes::ConstructAt(Data + Index, Other.Data[Index]);
                                }
                            }

                            Count = Other.Count;
                        }

                        // Перестроить таблицу под новый размер
                        void GrowTable(SizeType NewMaxSize)
                        {
                            // Старую таблицу откладываем во временные переменные
                            SizeType OldMaxSize = MaxSize;
                            KeyValue* OldData = Data;
                            const KernelTypes::UInt8* OldControl = Control;
                            Data = nullptr;
                            Control = nullptr;
                            Count = 0;
                            MaxSize = 0;
                            LoadLeft = 0;

                            AllocateTable(NewMaxSize);

                            KernelMemory::FillBytes(Control, BucketEmpty, MaxSize + 15);

                            if (OldData != nullptr)
                            {
                                for (SizeType Index = 0; Index < OldMaxSize; ++Index)
                                {
                                    if (OldControl[Index] & BucketUsed)
                                    {
                                        SizeType NewIndex = 0;
                                        KeyValue* Element = OldData + Index;

                                        [[maybe_unused]] KernelTypes::Bool Inserted = InsertKey<true>(HashTableDetail::GetKey(*Element), NewIndex);
                                        KernelAssert::DebugCheck(Inserted, "FastHashTable::GrowTable: не удалось перенести элемент");

                                        KernelTypes::ConstructAt(Data + NewIndex, KernelTypes::Move(*Element));
                                        KernelTypes::DestroyAt(Element);
                                    }
                                }

                                KernelMemory::Deallocate(OldData);
                            }
                        }

                    protected:
                        // Элемент по индексу корзины
                        KeyValue& GetElement(SizeType Index) const
                        {
                            return Data[Index];
                        }

                        // Найти место для ключа. Возвращает true, если место новое (элемент надо построить в OutIndex),
                        // и false, если такой ключ уже есть (OutIndex указывает на него).
                        template <bool InsertAfterGrow = false>
                        KernelTypes::Bool InsertKey(const Key& KeyToInsert, SizeType& OutIndex)
                        {
                            if (LoadLeft == 0)
                            {
                                if constexpr (InsertAfterGrow)
                                {
                                    KernelAssert::DebugCheck(false, "FastHashTable::InsertKey: рост во время роста");
                                }

                                // Либо чистим удалённые корзины, либо растём вдвое
                                SizeType DeletedCount = GetMaxLoad(MaxSize) - Count;
                                if (DeletedCount * MaxDeletedElementsDenominator > MaxSize * MaxDeletedElementsNumerator)
                                {
                                    Rehash(0);
                                }
                                else
                                {
                                    SizeType NewMaxSize = KernelTypes::Max<SizeType>(MaxSize << 1, 16);
                                    if (NewMaxSize < MaxSize)
                                    {
                                        KernelAssert::DebugCheck(false, "FastHashTable: переполнение размера таблицы, расти некуда");
                                        return false;
                                    }

                                    GrowTable(NewMaxSize);
                                }
                            }

                            SizeType Index;
                            KernelTypes::UInt8 ControlValue;
                            GetIndexAndControlValue(KeyToInsert, Index, ControlValue);

                            constexpr SizeType NoDeleted = ~SizeType(0);
                            SizeType FirstDeletedIndex = NoDeleted;

                            KeyEqual Equal;
                            SizeType BucketMask = MaxSize - 1;

                            for (;;)
                            {
                                // Читаем 16 контрольных байт (в конце есть дубль первых 15, переход через край не нужен)
                                if constexpr (!InsertAfterGrow)
                                {
                                    // При удалении могут появляться пустые корзины вместо удалённых, поэтому
                                    // совпадения нужно проверять во всей группе, в том числе после первой пустой.
                                    KernelTypes::UInt32 ControlEqual = KernelCompiler::EqualMask16(Control + Index, ControlValue);

                                    SizeType LocalIndex = Index;

                                    while (ControlEqual != 0)
                                    {
                                        KernelTypes::UInt32 FirstEqual = KernelCompiler::CountTrailingZeros(ControlEqual);

                                        LocalIndex += FirstEqual;
                                        LocalIndex &= BucketMask;

                                        if (Equal(HashTableDetail::GetKey(Data[LocalIndex]), KeyToInsert))
                                        {
                                            OutIndex = LocalIndex;
                                            return false;
                                        }

                                        ControlEqual >>= FirstEqual + 1;
                                        LocalIndex++;
                                    }

                                    // Запоминаем первую удалённую корзину
                                    if (FirstDeletedIndex == NoDeleted)
                                    {
                                        KernelTypes::UInt32 ControlDeleted = KernelCompiler::EqualMask16(Control + Index, BucketDeleted);
                                        if (ControlDeleted != 0)
                                        {
                                            FirstDeletedIndex = Index + KernelCompiler::CountTrailingZeros(ControlDeleted);
                                        }
                                    }
                                }

                                KernelTypes::UInt32 ControlEmpty = KernelCompiler::EqualMask16(Control + Index, BucketEmpty);
                                if (ControlEmpty != 0)
                                {
                                    // Удалённая корзина годится, даже если она дальше первой пустой: группы всё равно читаются по 16
                                    if (FirstDeletedIndex == NoDeleted || InsertAfterGrow)
                                    {
                                        Index += KernelCompiler::CountTrailingZeros(ControlEmpty);
                                        --LoadLeft; // занимая пустую корзину, расходуем запас загрузки
                                    }
                                    else
                                    {
                                        Index = FirstDeletedIndex;
                                    }

                                    Index &= BucketMask;

                                    SetControlValue(Index, ControlValue);
                                    ++Count;

                                    OutIndex = Index;
                                    return true;
                                }

                                // Следующая группа из 16 корзин
                                Index = (Index + 16) & BucketMask;
                            }
                        }

                    public:
                        // Итератор, меняющий элементы
                        class Iterator : public IteratorBase<FastHashTable, Iterator>
                        {
                            using Base = IteratorBase<FastHashTable, Iterator>;

                            public:
                                using Base::operator==;

                                using Reference = typename Base::ValueType&;
                                using Pointer = typename Base::ValueType*;

                                explicit Iterator(FastHashTable* TablePointer) : Base(TablePointer) { }
                                Iterator(FastHashTable* TablePointer, SizeType Index) : Base(TablePointer, Index) { }
                                Iterator(const Iterator& Other) : Base(Other) { }

                                Iterator& operator=(const Iterator& Other)
                                {
                                    Base::operator=(Other);
                                    return *this;
                                }

                                using Base::operator*;

                                KeyValue& operator*()
                                {
                                    KernelAssert::DebugCheck(this->IsValid(), "FastHashTable: разыменование недействительного итератора");
                                    return this->OwnerTable->Data[this->Index];
                                }

                                using Base::operator->;

                                KeyValue* operator->()
                                {
                                    KernelAssert::DebugCheck(this->IsValid(), "FastHashTable: разыменование недействительного итератора");
                                    return this->OwnerTable->Data + this->Index;
                                }
                        };

                        // Итератор только для чтения
                        class ConstIterator : public IteratorBase<const FastHashTable, ConstIterator>
                        {
                            using Base = IteratorBase<const FastHashTable, ConstIterator>;

                            public:
                                using Base::operator==;

                                using Reference = const typename Base::ValueType&;
                                using Pointer = const typename Base::ValueType*;

                                explicit ConstIterator(const FastHashTable* TablePointer) : Base(TablePointer) { }
                                ConstIterator(const FastHashTable* TablePointer, SizeType Index) : Base(TablePointer, Index) { }
                                ConstIterator(const ConstIterator& Other) : Base(Other) { }
                                ConstIterator(const Iterator& Other) : Base(Other.OwnerTable, Other.Index) { }

                                ConstIterator& operator=(const Iterator& Other)
                                {
                                    this->OwnerTable = Other.OwnerTable;
                                    this->Index = Other.Index;
                                    return *this;
                                }

                                ConstIterator& operator=(const ConstIterator& Other)
                                {
                                    Base::operator=(Other);
                                    return *this;
                                }
                        };

                        FastHashTable() = default;

                        FastHashTable(const FastHashTable& Other)
                        {
                            CopyTable(Other);
                        }

                        FastHashTable(FastHashTable&& Other) noexcept
                            : Data(Other.Data), Control(Other.Control), Count(Other.Count), MaxSize(Other.MaxSize), LoadLeft(Other.LoadLeft)
                        {
                            Other.Data = nullptr;
                            Other.Control = nullptr;
                            Other.Count = 0;
                            Other.MaxSize = 0;
                            Other.LoadLeft = 0;
                        }

                        FastHashTable& operator=(const FastHashTable& Other)
                        {
                            if (this != &Other)
                            {
                                Clear();
                                CopyTable(Other);
                            }

                            return *this;
                        }

                        FastHashTable& operator=(FastHashTable&& Other) noexcept
                        {
                            if (this != &Other)
                            {
                                Clear();

                                Data = Other.Data;
                                Control = Other.Control;
                                Count = Other.Count;
                                MaxSize = Other.MaxSize;
                                LoadLeft = Other.LoadLeft;

                                Other.Data = nullptr;
                                Other.Control = nullptr;
                                Other.Count = 0;
                                Other.MaxSize = 0;
                                Other.LoadLeft = 0;
                            }

                            return *this;
                        }

                        ~FastHashTable()
                        {
                            Clear();
                        }

                        // Зарезервировать место под заданное число элементов
                        void Reserve(SizeType ElementCount)
                        {
                            SizeType NewMaxSize = KernelTypes::NextPowerOfTwo(KernelTypes::Max<KernelTypes::UInt32>(
                                static_cast<KernelTypes::UInt32>((MaxLoadFactorDenominator * ElementCount) / MaxLoadFactorNumerator), 16));

                            if (NewMaxSize <= MaxSize)
                            {
                                return;
                            }

                            GrowTable(NewMaxSize);
                        }

                        // Уничтожить все элементы и освободить память
                        void Clear()
                        {
                            if constexpr (!KernelTypes::IsTriviallyDestructible<KeyValue>)
                            {
                                if (!Empty())
                                {
                                    for (SizeType Index = 0; Index < MaxSize; ++Index)
                                    {
                                        if (Control[Index] & BucketUsed)
                                        {
                                            KernelTypes::DestroyAt(Data + Index);
                                        }
                                    }
                                }
                            }

                            if (Data != nullptr)
                            {
                                KernelMemory::Deallocate(Data);

                                Data = nullptr;
                                Control = nullptr;
                                Count = 0;
                                MaxSize = 0;
                                LoadLeft = 0;
                            }
                        }

                        // Уничтожить все элементы, но оставить память
                        void ClearAndKeepMemory()
                        {
                            if constexpr (!KernelTypes::IsTriviallyDestructible<KeyValue>)
                            {
                                if (!Empty())
                                {
                                    for (SizeType Index = 0; Index < MaxSize; ++Index)
                                    {
                                        if (Control[Index] & BucketUsed)
                                        {
                                            KernelTypes::DestroyAt(Data + Index);
                                        }
                                    }
                                }
                            }

                            Count = 0;

                            // Если остались не пустые (удалённые) корзины, сбрасываем контрольные байты
                            SizeType MaxLoad = GetMaxLoad(MaxSize);
                            if (LoadLeft != MaxLoad)
                            {
                                KernelMemory::FillBytes(Control, BucketEmpty, MaxSize + 15);
                                LoadLeft = MaxLoad;
                            }
                        }

                        // begin() и end() записаны строчными, потому что их требует цикл range-for
                        Iterator begin()
                        {
                            return Iterator(this);
                        }

                        Iterator end()
                        {
                            return Iterator(this, MaxSize);
                        }

                        ConstIterator begin() const
                        {
                            return ConstIterator(this);
                        }

                        ConstIterator end() const
                        {
                            return ConstIterator(this, MaxSize);
                        }

                        ConstIterator cbegin() const
                        {
                            return ConstIterator(this);
                        }

                        ConstIterator cend() const
                        {
                            return ConstIterator(this, MaxSize);
                        }

                        // Число корзин
                        SizeType BucketCount() const
                        {
                            return MaxSize;
                        }

                        // Наибольшее возможное число корзин
                        constexpr SizeType MaxBucketCount() const
                        {
                            return SizeType(1) << (sizeof(SizeType) * 8 - 1);
                        }

                        KernelTypes::Bool Empty() const
                        {
                            return Count == 0;
                        }

                        // Число элементов
                        SizeType Size() const
                        {
                            return Count;
                        }

                        // Наибольшее возможное число элементов
                        constexpr SizeType MaxElementCount() const
                        {
                            return static_cast<SizeType>((static_cast<KernelTypes::UInt64>(MaxBucketCount()) * MaxLoadFactorNumerator) / MaxLoadFactorDenominator);
                        }

                        // Допустимая загрузка (элементов на корзину)
                        constexpr float MaxLoadFactor() const
                        {
                            return static_cast<float>(MaxLoadFactorNumerator) / static_cast<float>(MaxLoadFactorDenominator);
                        }

                        // Вставить элемент. Возвращает итератор и признак того, что элемент был вставлен.
                        KernelTypes::Pair<Iterator, KernelTypes::Bool> Insert(const ValueType& Value)
                        {
                            SizeType Index;
                            KernelTypes::Bool Inserted = InsertKey(HashTableDetail::GetKey(Value), Index);

                            if (Inserted)
                            {
                                KernelTypes::ConstructAt(Data + Index, Value);
                            }

                            return KernelTypes::Pair<Iterator, KernelTypes::Bool>(Iterator(this, Index), Inserted);
                        }

                        // Найти элемент. Возвращает итератор на него или end().
                        ConstIterator Find(const Key& KeyToFind) const
                        {
                            if (Empty())
                            {
                                return cend();
                            }

                            SizeType Index;
                            KernelTypes::UInt8 ControlValue;
                            GetIndexAndControlValue(KeyToFind, Index, ControlValue);

                            KeyEqual Equal;
                            SizeType BucketMask = MaxSize - 1;

                            for (;;)
                            {
                                // Совпадения проверяем во всей группе, в том числе после первой пустой корзины (см. InsertKey)
                                KernelTypes::UInt32 ControlEqual = KernelCompiler::EqualMask16(Control + Index, ControlValue);

                                SizeType LocalIndex = Index;

                                while (ControlEqual != 0)
                                {
                                    KernelTypes::UInt32 FirstEqual = KernelCompiler::CountTrailingZeros(ControlEqual);

                                    LocalIndex += FirstEqual;
                                    LocalIndex &= BucketMask;

                                    if (Equal(HashTableDetail::GetKey(Data[LocalIndex]), KeyToFind))
                                    {
                                        return ConstIterator(this, LocalIndex);
                                    }

                                    ControlEqual >>= FirstEqual + 1;
                                    LocalIndex++;
                                }

                                // Пустая корзина в группе значит, что ключа нет
                                if (KernelCompiler::EqualMask16(Control + Index, BucketEmpty) != 0)
                                {
                                    return cend();
                                }

                                Index = (Index + 16) & BucketMask;
                            }
                        }

                        // Удалить элемент по итератору
                        void Erase(const ConstIterator& Position)
                        {
                            KernelAssert::DebugCheck(Position.IsValid(), "FastHashTable::Erase: недействительный итератор");

                            // Группы по 16 байт до и после позиции (в конце есть дубль первых 15 байт)
                            KernelTypes::UInt32 ControlEmptyBefore = KernelCompiler::EqualMask16(Control + ((Position.Index - 16) & (MaxSize - 1)), BucketEmpty);
                            KernelTypes::UInt32 ControlEmptyAfter = KernelCompiler::EqualMask16(Control + Position.Index, BucketEmpty);

                            // Если вокруг есть 16 подряд занятых корзин (включая эту), зонд, ищущий другой элемент, должен идти дальше,
                            // поэтому пустой её делать нельзя, только удалённой.
                            // Используется CountLeadingZeros(uint16) = CountLeadingZeros(uint32) - 16.
                            KernelTypes::UInt8 ControlValue =
                                KernelCompiler::CountLeadingZeros(ControlEmptyBefore) - 16 + KernelCompiler::CountTrailingZeros(ControlEmptyAfter) < 16
                                    ? BucketEmpty
                                    : BucketDeleted;

                            SetControlValue(Position.Index, ControlValue);

                            KernelTypes::DestroyAt(Data + Position.Index);

                            if (ControlValue == BucketEmpty)
                            {
                                ++LoadLeft;
                            }

                            --Count;
                        }

                        // Удалить элемент по ключу. Возвращает число удалённых (0 или 1).
                        SizeType Erase(const Key& KeyToErase)
                        {
                            ConstIterator Found = Find(KeyToErase);
                            if (Found == cend())
                            {
                                return 0;
                            }

                            Erase(Found);
                            return 1;
                        }

                        void Swap(FastHashTable& Other) noexcept
                        {
                            KernelTypes::Swap(Data, Other.Data);
                            KernelTypes::Swap(Control, Other.Control);
                            KernelTypes::Swap(Count, Other.Count);
                            KernelTypes::Swap(MaxSize, Other.MaxSize);
                            KernelTypes::Swap(LoadLeft, Other.LoadLeft);
                        }

                        // Перехеширование на месте, убирает удалённые корзины. В отличие от std, размер не принимает: таблица остаётся того же размера.
                        void Rehash(SizeType)
                        {
                            // Удалённые становятся пустыми, занятые временно удалёнными (им надо переехать на правильное место)
                            for (SizeType Index = 0; Index < MaxSize; ++Index)
                            {
                                KernelTypes::UInt8& ControlValue = Control[Index];

                                switch (ControlValue)
                                {
                                    case BucketDeleted:
                                        ControlValue = BucketEmpty;
                                        break;
                                    case BucketEmpty:
                                        break;
                                    default:
                                        ControlValue = BucketDeleted;
                                        break;
                                }
                            }

                            for (SizeType Index = 0; Index < 15; ++Index)
                            {
                                Control[MaxSize + Index] = Control[Index];
                            }

                            SizeType BucketMask = MaxSize - 1;
                            KernelTypes::UInt32 ProbeMask = BucketMask & ~KernelTypes::UInt32(0b1111); // младшие 4 бита убираем: проверяем по 16 корзин

                            for (SizeType Source = 0; Source < MaxSize; ++Source)
                            {
                                if (Control[Source] != BucketDeleted)
                                {
                                    continue;
                                }

                                for (;;)
                                {
                                    SizeType SourceIndex;
                                    KernelTypes::UInt8 SourceControl;
                                    GetIndexAndControlValue(HashTableDetail::GetKey(Data[Source]), SourceIndex, SourceControl);

                                    // Ищем свободную корзину линейным зондированием
                                    SizeType Destination = SourceIndex;
                                    for (;;)
                                    {
                                        KernelTypes::UInt32 ControlFree = KernelCompiler::HighBitMask16(Control + Destination) ^ 0xffff;
                                        if (ControlFree != 0)
                                        {
                                            Destination += KernelCompiler::CountTrailingZeros(ControlFree);
                                            Destination &= BucketMask;
                                            break;
                                        }

                                        Destination = (Destination + 16) & BucketMask;
                                    }

                                    if (((Destination - SourceIndex) & ProbeMask) == ((Source - SourceIndex) & ProbeMask))
                                    {
                                        // Остаёмся в той же группе, элемент можно не двигать
                                        SetControlValue(Source, SourceControl);
                                        break;
                                    }
                                    else if (Control[Destination] == BucketEmpty)
                                    {
                                        // Нашлась пустая корзина, переезжаем
                                        SetControlValue(Destination, SourceControl);
                                        SetControlValue(Source, BucketEmpty);
                                        KernelTypes::ConstructAt(Data + Destination, KernelTypes::Move(Data[Source]));
                                        KernelTypes::DestroyAt(Data + Source);
                                        break;
                                    }
                                    else
                                    {
                                        // В нужной корзине сидит другой элемент: меняемся местами и повторяем для этой же исходной корзины
                                        KernelAssert::DebugCheck(Control[Destination] == BucketDeleted, "FastHashTable::Rehash: ожидалась удалённая корзина");
                                        SetControlValue(Destination, SourceControl);
                                        KernelTypes::Swap(Data[Source], Data[Destination]);
                                    }
                                }
                            }

                            LoadLeft = GetMaxLoad(MaxSize) - Count;
                        }

                    private:
                        // Выравнивание блока: не меньше стандартного выравнивания аллокатора
                        static constexpr KernelTypes::Size AllocationAlignment =
                            alignof(KeyValue) > KernelMemory::DefaultAlignment ? alignof(KeyValue) : KernelMemory::DefaultAlignment;

                        // Допустимая загрузка равна числителю / знаменателю
                        static constexpr KernelTypes::UInt64 MaxLoadFactorNumerator = 7;
                        static constexpr KernelTypes::UInt64 MaxLoadFactorDenominator = 8;

                        // Если удастся вернуть эту долю удалённых корзин, таблица перестраивается на месте, а не растёт
                        static constexpr KernelTypes::UInt64 MaxDeletedElementsNumerator = 1;
                        static constexpr KernelTypes::UInt64 MaxDeletedElementsDenominator = 8;

                        // Значения контрольных байт
                        static constexpr KernelTypes::UInt8 BucketEmpty = 0;
                        static constexpr KernelTypes::UInt8 BucketDeleted = 0x7f;
                        static constexpr KernelTypes::UInt8 BucketUsed = 0x80; // младшие 7 бит это младшие 7 бит хеша

                        // Корзины, массив из MaxSize элементов
                        KeyValue* Data = nullptr;

                        // Контрольные байты, массив из MaxSize + 15 элементов
                        KernelTypes::UInt8* Control = nullptr;

                        // Число элементов
                        SizeType Count = 0;

                        // Число корзин
                        SizeType MaxSize = 0;

                        // Сколько элементов можно добавить до роста
                        SizeType LoadLeft = 0;
                };
            }
        }
    }
}
