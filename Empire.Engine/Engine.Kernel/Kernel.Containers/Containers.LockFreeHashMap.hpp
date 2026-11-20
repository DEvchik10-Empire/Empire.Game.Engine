#pragma once

#include "Empire.Engine/Engine.Kernel/Kernel.Assert/Assert.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Compiler/Compiler.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Containers/Containers.FastArray.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Memory/Memory.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Sync/Sync.Atomic.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Types/Types.NonCopyable.hpp"
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
                    struct LockFreeHashMapStatistics
                    {
                        static constexpr KernelTypes::UInt32 HistogramSize = 256;
                        KernelTypes::UInt32 MaxObjectsPerBucket = 0;
                        KernelTypes::UInt32 BucketCount = 0;
                        KernelTypes::UInt32 ObjectCount = 0;
                        KernelTypes::UInt32 Histogram[HistogramSize]{};
                    };

                    // Аллокатор для LockFreeHashMap (перенесён из Jolt, Core/LockFreeHashMap.h): один большой блок памяти под все пары ключ-значение.
                    class LockFreeAllocator : public KernelTypes::NonCopyable
                    {
                        public:
                            ~LockFreeAllocator()
                            {
                                KernelMemory::Deallocate(ObjectStore);
                            }

                            // Выделить ObjectStoreBytes байт под все пары
                            void Init(KernelTypes::UInt32 ObjectStoreBytes)
                            {
                                KernelAssert::DebugCheck(ObjectStore == nullptr, "LockFreeAllocator::Init: уже подготовлен");

                                ObjectStoreSizeBytes = ObjectStoreBytes;
                                ObjectStore = static_cast<KernelTypes::UInt8*>(KernelMemory::Allocate(ObjectStoreBytes, 16));
                            }

                            // Освободить все выделения
                            void Clear()
                            {
                                WriteOffset.Store(0);
                            }

                            // Выделить блок. На входе Begin и End это текущий блок вызывающего (первый свободный байт и конец),
                            // на выходе новый блок (может быть меньше BlockSize, если память кончается).
                            void Allocate(KernelTypes::UInt32 BlockSize, KernelTypes::UInt32& Begin, KernelTypes::UInt32& End)
                            {
                                // Если буфер уже исчерпан, атомарную добавку не делаем: при множестве вставок после заполнения
                                // WriteOffset (32 бита) мог бы обернуться через ноль и испортить память. Так он вылезает за размер
                                // буфера максимум на (число процессоров * BlockSize).
                                if (WriteOffset.Load(KernelSync::MemoryOrder::Relaxed) >= ObjectStoreSizeBytes)
                                {
                                    return;
                                }

                                KernelTypes::UInt32 NewBegin = WriteOffset.FetchAdd(BlockSize, KernelSync::MemoryOrder::Relaxed);
                                KernelTypes::UInt32 NewEnd = KernelTypes::Min(NewBegin + BlockSize, ObjectStoreSizeBytes);

                                if (End == NewBegin)
                                {
                                    // Блок идёт сразу за предыдущим
                                    NewBegin = Begin;
                                }
                                else
                                {
                                    NewBegin = KernelTypes::Min(NewBegin, ObjectStoreSizeBytes);
                                }

                                Begin = NewBegin;
                                End = NewEnd;
                            }

                            // Указатель в смещение
                            template <typename T>
                            KernelTypes::UInt32 ToOffset(const T* Data) const
                            {
                                const KernelTypes::UIntPtr Address = reinterpret_cast<KernelTypes::UIntPtr>(Data);
                                const KernelTypes::UIntPtr BaseAddress = reinterpret_cast<KernelTypes::UIntPtr>(ObjectStore);
                                KernelAssert::DebugCheck(
                                    Address >= BaseAddress && Address - BaseAddress < ObjectStoreSizeBytes,
                                    "LockFreeAllocator::ToOffset: указатель вне буфера"
                                );
                                return static_cast<KernelTypes::UInt32>(Address - BaseAddress);
                            }

                            // Смещение в указатель
                            template <typename T>
                            T* FromOffset(KernelTypes::UInt32 Offset) const
                            {
                                KernelAssert::DebugCheck(Offset < ObjectStoreSizeBytes, "LockFreeAllocator::FromOffset: смещение вне буфера");
                                return reinterpret_cast<T*>(ObjectStore + Offset);
                            }

                        private:
                            KernelTypes::UInt8* ObjectStore = nullptr;
                            KernelTypes::UInt32 ObjectStoreSizeBytes = 0;
                            KernelSync::Atomic<KernelTypes::UInt32> WriteOffset{0};
                    };

                    // Контекст аллокатора: берёт у LockFreeAllocator крупный блок и раздаёт его мелкими порциями,
                    // чтобы потоки реже обращались к общему атомарному счётчику. Один контекст на поток.
                    class LockFreeAllocatorContext : public KernelTypes::NonCopyable
                    {
                        public:
                            LockFreeAllocatorContext(LockFreeAllocator& TargetAllocator, KernelTypes::UInt32 BlockSizeBytes)
                                : Allocator(TargetAllocator), BlockSize(BlockSizeBytes)
                            {
                            }

                            // Выделить Size байт с выравниванием Alignment, OutWriteOffset это смещение в буфере. Возвращает false, если места нет.
                            KernelTypes::Bool Allocate(KernelTypes::UInt32 Size, KernelTypes::UInt32 Alignment, KernelTypes::UInt32& OutWriteOffset)
                            {
                                KernelAssert::DebugCheck(KernelTypes::IsPowerOfTwo(Alignment), "LockFreeAllocatorContext::Allocate: выравнивание должно быть степенью двойки");
                                KernelTypes::UInt32 AlignmentMask = Alignment - 1;
                                KernelTypes::UInt32 Padding = (Alignment - (Begin & AlignmentMask)) & AlignmentMask;

                                if (End - Begin < Size + Padding)
                                {
                                    Allocator.Allocate(BlockSize, Begin, End);

                                    Padding = (Alignment - (Begin & AlignmentMask)) & AlignmentMask;

                                    if (End - Begin < Size + Padding)
                                    {
                                        return false;
                                    }
                                }

                                Begin += Padding;
                                OutWriteOffset = Begin;
                                Begin += Size;
                                return true;
                            }

                        private:
                            LockFreeAllocator& Allocator;
                            KernelTypes::UInt32 BlockSize;
                            KernelTypes::UInt32 Begin = 0;
                            KernelTypes::UInt32 End = 0;
                    };

                    // Простая хеш-карта без блокировок: только вставка и поиск, фиксированное число корзин и фиксированное хранилище
                    // (перенесена из Jolt, Core/LockFreeHashMap.h). Ключ и значение должны быть простыми типами без деструкторов.
                    // Вставлять можно из нескольких потоков одновременно.
                    template <typename KeyType, typename ValueType>
                    class LockFreeHashMap : public KernelTypes::NonCopyable
                    {
                        public:
                            using MapType = LockFreeHashMap<KeyType, ValueType>;

                            explicit LockFreeHashMap(LockFreeAllocator& TargetAllocator)
                                : Allocator(TargetAllocator)
                            {
                            }

                            ~LockFreeHashMap()
                            {
                                KernelMemory::Deallocate(Buckets);
                            }

                            // Подготовить карту. MaxBucketCountValue должно быть степенью двойки не меньше 4.
                            void Init(KernelTypes::UInt32 MaxBucketCountValue)
                            {
                                KernelAssert::DebugCheck(MaxBucketCountValue >= 4 && KernelTypes::IsPowerOfTwo(MaxBucketCountValue), "LockFreeHashMap::Init: число корзин должно быть степенью двойки не меньше 4");
                                KernelAssert::DebugCheck(Buckets == nullptr, "LockFreeHashMap::Init: уже подготовлена");

                                BucketCountValue = MaxBucketCountValue;
                                MaxBuckets = MaxBucketCountValue;

                                Buckets = static_cast<KernelSync::Atomic<KernelTypes::UInt32>*>(
                                    KernelMemory::Allocate(MaxBucketCountValue * sizeof(KernelSync::Atomic<KernelTypes::UInt32>), 16));

                                for (KernelTypes::UInt32 Index = 0; Index < MaxBucketCountValue; ++Index)
                                {
                                    KernelTypes::ConstructAt(Buckets + Index);
                                }

                                Clear();
                            }

                            // Удалить все элементы. Нельзя вызывать одновременно со вставкой.
                            void Clear()
                            {
                                if (BucketCountValue == 0)
                                    return;

                                if constexpr (KernelCompiler::IsDebugBuild)
                                {
                                    KeyValueCount.Store(0, KernelSync::MemoryOrder::Relaxed);
                                }

                                // InvalidHandle это 0xffffffff, значит все байты 0xff
                                static_assert(sizeof(KernelSync::Atomic<KernelTypes::UInt32>) == sizeof(KernelTypes::UInt32));
                                KernelMemory::FillBytes(Buckets, 0xff, BucketCountValue * sizeof(KernelTypes::UInt32));
                            }

                            // Текущее число корзин
                            KernelTypes::UInt32 GetBucketCount() const
                            {
                                return BucketCountValue;
                            }

                            // Точное имя публичного метода Jolt 5.7.0.
                            KernelTypes::UInt32 GetNumBuckets() const
                            {
                                return GetBucketCount();
                            }

                            // Наибольшее число корзин
                            KernelTypes::UInt32 GetMaxBucketCount() const
                            {
                                return MaxBuckets;
                            }

                            KernelTypes::UInt32 GetMaxBuckets() const
                            {
                                return GetMaxBucketCount();
                            }

                            // Изменить число корзин (степень двойки, не больше заданного в Init). Только после Clear и не одновременно с другими операциями.
                            void SetBucketCount(KernelTypes::UInt32 NewBucketCount)
                            {
                                KernelAssert::DebugCheck(KeyValueCount.Load(KernelSync::MemoryOrder::Relaxed) == 0, "LockFreeHashMap::SetBucketCount: карта не пуста");
                                KernelAssert::DebugCheck(NewBucketCount <= MaxBuckets, "LockFreeHashMap::SetBucketCount: больше максимума");
                                KernelAssert::DebugCheck(NewBucketCount >= 4 && KernelTypes::IsPowerOfTwo(NewBucketCount), "LockFreeHashMap::SetBucketCount: нужна степень двойки не меньше 4");

                                BucketCountValue = NewBucketCount;
                            }

                            void SetNumBuckets(KernelTypes::UInt32 NewBucketCount)
                            {
                                SetBucketCount(NewBucketCount);
                            }

                            // Пара ключ-значение
                            class KeyValue
                            {
                                public:
                                    const KeyType& GetKey() const
                                    {
                                        return StoredKey;
                                    }

                                    ValueType& GetValue()
                                    {
                                        return StoredValue;
                                    }

                                    const ValueType& GetValue() const
                                    {
                                        return StoredValue;
                                    }

                                private:
                                    template <typename OtherKey, typename OtherValue>
                                    friend class LockFreeHashMap;

                                    KeyType StoredKey;                  // ключ
                                    KernelTypes::UInt32 NextOffset;     // смещение следующей пары с тем же хешем
                                    ValueType StoredValue;              // значение (и, возможно, дополнительные байты после него)
                            };

                            // Вставить элемент, вернуть nullptr, если хранилище заполнено. Вставлять можно из нескольких потоков.
                            template <typename... Parameters>
                            KeyValue* Create(LockFreeAllocatorContext& Context, const KeyType& KeyToInsert, KernelTypes::UInt64 KeyHash, KernelTypes::UInt32 ExtraBytes, Parameters&&... ConstructorParameters)
                            {
                                // Это не multimap: ключа ещё не должно быть
                                if constexpr (KernelCompiler::IsDebugBuild)
                                {
                                    KernelAssert::DebugCheck(Find(KeyToInsert, KeyHash) == nullptr, "LockFreeHashMap::Create: ключ уже есть");
                                }

                                KernelTypes::UInt32 Size = sizeof(KeyValue) + ExtraBytes;

                                KernelTypes::UInt32 WriteOffset;
                                if (!Context.Allocate(Size, alignof(KeyValue), WriteOffset))
                                {
                                    return nullptr;
                                }

                                if constexpr (KernelCompiler::IsDebugBuild)
                                {
                                    KeyValueCount.FetchAdd(1, KernelSync::MemoryOrder::Relaxed);
                                }

                                KeyValue* Pair = Allocator.template FromOffset<KeyValue>(WriteOffset);
                                if constexpr (KernelCompiler::IsDebugBuild)
                                {
                                    KernelMemory::FillBytes(Pair, 0xcd, Size);
                                }

                                KernelTypes::ConstructAt(&Pair->StoredKey, KeyToInsert);
                                KernelTypes::ConstructAt(&Pair->StoredValue, KernelTypes::Forward<Parameters>(ConstructorParameters)...);

                                // Ставим пару первой в цепочку своей корзины
                                KernelSync::Atomic<KernelTypes::UInt32>& Head = Buckets[KeyHash & (BucketCountValue - 1)];

                                KernelTypes::UInt32 OldOffset = Head.Load(KernelSync::MemoryOrder::Relaxed);
                                for (;;)
                                {
                                    Pair->NextOffset = OldOffset;
                                    if (Head.CompareExchange(OldOffset, WriteOffset, KernelSync::MemoryOrder::Release))
                                    {
                                        break;
                                    }
                                }

                                return Pair;
                            }

                            // Найти элемент, nullptr если нет
                            const KeyValue* Find(const KeyType& KeyToFind, KernelTypes::UInt64 KeyHash) const
                            {
                                KernelTypes::UInt32 Offset = Buckets[KeyHash & (BucketCountValue - 1)].Load(KernelSync::MemoryOrder::Acquire);
                                while (Offset != InvalidHandle)
                                {
                                    const KeyValue* Pair = Allocator.template FromOffset<const KeyValue>(Offset);
                                    if (Pair->StoredKey == KeyToFind)
                                    {
                                        return Pair;
                                    }

                                    Offset = Pair->NextOffset;
                                }

                                return nullptr;
                            }

                            // Значение недействительного хендла
                            static constexpr KernelTypes::UInt32 InvalidHandle = KernelTypes::UInt32(-1);

                            // Пара в хендл
                            KernelTypes::UInt32 ToHandle(const KeyValue* Pair) const
                            {
                                return Allocator.ToOffset(Pair);
                            }

                            // Хендл в пару
                            const KeyValue* FromHandle(KernelTypes::UInt32 Handle) const
                            {
                                return Allocator.template FromOffset<const KeyValue>(Handle);
                            }

                            // Хендл в пару (ключ менять нельзя)
                            KeyValue* FromHandle(KernelTypes::UInt32 Handle)
                            {
                                return Allocator.template FromOffset<KeyValue>(Handle);
                            }

                            // Число пар. Считается только в отладочной сборке (в release всегда 0): счётчик создаёт конкуренцию между потоками.
                            KernelTypes::UInt32 GetKeyValueCount() const
                            {
                                return KeyValueCount.Load(KernelSync::MemoryOrder::Relaxed);
                            }

                            KernelTypes::UInt32 GetNumKeyValues() const
                            {
                                return GetKeyValueCount();
                            }

                            // Собрать все пары
                            void GetAllKeyValues(FastArray<const KeyValue*>& OutAll) const
                            {
                                if (BucketCountValue == 0)
                                    return;

                                for (const KernelSync::Atomic<KernelTypes::UInt32>* Bucket = Buckets; Bucket < Buckets + BucketCountValue; ++Bucket)
                                {
                                    KernelTypes::UInt32 Offset = Bucket->Load();
                                    while (Offset != InvalidHandle)
                                    {
                                        const KeyValue* Pair = Allocator.template FromOffset<const KeyValue>(Offset);
                                        OutAll.PushBack(Pair);
                                        Offset = Pair->NextOffset;
                                    }
                                }
                            }

                            // Итератор. Обходить параллельно с Clear() нельзя. Параллельно со вставкой можно, но новые пары могут не попасть в обход.
                            struct Iterator
                            {
                                KernelTypes::Bool operator==(const Iterator& Other) const
                                {
                                    return Map == Other.Map && Bucket == Other.Bucket && Offset == Other.Offset;
                                }

                                KernelTypes::Bool operator!=(const Iterator& Other) const
                                {
                                    return !(*this == Other);
                                }

                                KeyValue& operator*()
                                {
                                    KernelAssert::DebugCheck(Offset != InvalidHandle, "LockFreeHashMap::Iterator: разыменование конца");
                                    return *Map->Allocator.template FromOffset<KeyValue>(Offset);
                                }

                                Iterator& operator++()
                                {
                                    KernelAssert::DebugCheck(Bucket < Map->BucketCountValue, "LockFreeHashMap::Iterator: инкремент конца");

                                    if (Offset != InvalidHandle)
                                    {
                                        const KeyValue* Pair = Map->Allocator.template FromOffset<const KeyValue>(Offset);
                                        Offset = Pair->NextOffset;
                                        if (Offset != InvalidHandle)
                                        {
                                            return *this;
                                        }
                                    }

                                    for (;;)
                                    {
                                        ++Bucket;
                                        if (Bucket >= Map->BucketCountValue)
                                        {
                                            return *this;
                                        }

                                        Offset = Map->Buckets[Bucket].Load();
                                        if (Offset != InvalidHandle)
                                        {
                                            return *this;
                                        }
                                    }
                                }

                                MapType* Map;
                                KernelTypes::UInt32 Bucket;
                                KernelTypes::UInt32 Offset;
                            };

                            // begin() и end() записаны строчными, потому что их требует цикл range-for
                            Iterator begin()
                            {
                                if (BucketCountValue == 0)
                                    return end();

                                Iterator It{this, 0, Buckets[0].Load()};

                                if (It.Offset == InvalidHandle)
                                {
                                    ++It;
                                }

                                return It;
                            }

                            Iterator end()
                            {
                                return {this, BucketCountValue, InvalidHandle};
                            }

                            // Статистика распределения элементов по корзинам (аналог Jolt TraceStats).
                            // Возвращаем данные, а не вызываем глобальный логгер: Kernel.Containers не зависит от Kernel.Logging.
                            [[nodiscard]] LockFreeHashMapStatistics GetStatistics() const
                            {
                                LockFreeHashMapStatistics Statistics;
                                Statistics.BucketCount = BucketCountValue;

                                for (KernelTypes::UInt32 BucketIndex = 0; BucketIndex < BucketCountValue; ++BucketIndex)
                                {
                                    KernelTypes::UInt32 ObjectsInBucket = 0;
                                    KernelTypes::UInt32 Offset = Buckets[BucketIndex].Load(KernelSync::MemoryOrder::Relaxed);
                                    while (Offset != InvalidHandle)
                                    {
                                        const KeyValue* Pair = Allocator.template FromOffset<const KeyValue>(Offset);
                                        Offset = Pair->NextOffset;
                                        ++ObjectsInBucket;
                                        ++Statistics.ObjectCount;
                                    }

                                    Statistics.MaxObjectsPerBucket = KernelTypes::Max(Statistics.MaxObjectsPerBucket, ObjectsInBucket);
                                    const KernelTypes::UInt32 HistogramIndex = KernelTypes::Min(
                                        ObjectsInBucket,
                                        LockFreeHashMapStatistics::HistogramSize - 1
                                    );
                                    ++Statistics.Histogram[HistogramIndex];
                                }

                                return Statistics;
                            }

                        private:
                            LockFreeAllocator& Allocator;                                           // выделяет пары

                            KernelSync::Atomic<KernelTypes::UInt32> KeyValueCount;                  // число пар (только отладочная сборка)

                            KernelSync::Atomic<KernelTypes::UInt32>* Buckets = nullptr;             // смещение первой пары с данным хешем
                            KernelTypes::UInt32 BucketCountValue = 0;                               // текущее число корзин
                            KernelTypes::UInt32 MaxBuckets = 0;                                     // наибольшее число корзин
                    };
                }
            }
        }
    }
}
