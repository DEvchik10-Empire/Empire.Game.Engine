#pragma once

#include "Empire.Kernel/Kernel.Compiler/Compiler.hpp"
#include "Empire.Kernel/Kernel.Assert/Assert.hpp"
#include "Empire.Kernel/Kernel.Memory/Memory.hpp"
#include "Empire.Kernel/Kernel.Sync/Sync.Atomic.hpp"
#include "Empire.Kernel/Kernel.Sync/Sync.Mutex.hpp"
#include "Empire.Kernel/Kernel.Types/Types.NonCopyable.hpp"
#include "Empire.Kernel/Kernel.Types/Types.Utility.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireKernel
        {
            namespace KernelContainers
            {
                // Пул объектов фиксированного размера с созданием и уничтожением без блокировок (перенесён из Jolt, Core/FixedSizeFreeList.h).
                // Блокировка берётся только когда нужно выделить новую страницу объектов. Можно собрать много объектов
                // в пакет (Batch) и вернуть их в пул одной атомарной операцией.
                template <typename Object>
                class FixedSizeFreeList : public KernelTypes::NonCopyable
                {
                    private:
                        // Хранилище одного объекта
                        struct ObjectStorage
                        {
                            // Сам объект
                            Object StoredObject;

                            // Пока объект свободен (или освобождается пакетом), здесь индекс следующего свободного.
                            // Пока объект занят, здесь его собственный индекс.
                            KernelSync::Atomic<KernelTypes::UInt32> NextFreeObject;
                        };

                        static_assert(alignof(ObjectStorage) == alignof(Object), "Object не выровнен как нужно");

                        const ObjectStorage& GetStorage(KernelTypes::UInt32 ObjectIndex) const
                        {
                            return Pages[ObjectIndex >> PageShift][ObjectIndex & ObjectMask];
                        }

                        ObjectStorage& GetStorage(KernelTypes::UInt32 ObjectIndex)
                        {
                            return Pages[ObjectIndex >> PageShift][ObjectIndex & ObjectMask];
                        }

                        // Размер страницы в объектах
                        KernelTypes::UInt32 PageSize = 0;

                        // На сколько бит сдвинуть индекс вправо, чтобы получить номер страницы
                        KernelTypes::UInt32 PageShift = 0;

                        // Маска индекса внутри страницы
                        KernelTypes::UInt32 ObjectMask = 0;

                        // Число доступных страниц
                        KernelTypes::UInt32 PageCount = 0;

                        // Сколько объектов выделено под страницы. Меняется только под PageMutex, но читается без блокировки.
                        // Пишется с Release после записи указателя на страницу, поэтому читатель нового значения видит и страницу.
                        KernelSync::Atomic<KernelTypes::UInt32> ObjectsAllocated;

                        // Таблица страниц
                        ObjectStorage** Pages = nullptr;

                        // Мьютекс выделения новой страницы. Выровнен по строке кэша, чтобы не делить её с константами,
                        // по которым индексируется Get().
                        alignas(KernelCompiler::CacheLineSize) KernelSync::Mutex PageMutex;

                        // Сколько объектов в списке свободных и в ещё не тронутых страницах (считается только в отладочной сборке)
                        KernelSync::Atomic<KernelTypes::UInt32> FreeObjectCount;

                        // Счётчик, меняющий указатель на первый свободный при каждом CAS (против проблемы ABA)
                        KernelSync::Atomic<KernelTypes::UInt32> AllocationTag;

                        // Индекс первого свободного в младших 32 битах и тег в старших
                        KernelSync::Atomic<KernelTypes::UInt64> FirstFreeObjectAndTag;

                        // Первый объект из ещё ни разу не использованных страниц (когда список свободных пуст)
                        KernelSync::Atomic<KernelTypes::UInt32> FirstFreeObjectInNewPage;

                    public:
                        // Недействительный индекс
                        static constexpr KernelTypes::UInt32 InvalidObjectIndex = 0xffffffff;

                        // Размер объекта вместе со служебными данными
                        static constexpr KernelTypes::UInt32 ObjectStorageSize = sizeof(ObjectStorage);

                        ~FixedSizeFreeList()
                        {
                            if (Pages != nullptr)
                            {
                                // К моменту уничтожения всё должно быть возвращено
                                if constexpr (KernelCompiler::IsDebugBuild)
                                {
                                    KernelAssert::DebugCheck(
                                        FreeObjectCount.Load(KernelSync::MemoryOrder::Relaxed) == PageCount * PageSize,
                                        "FixedSizeFreeList: уничтожение пула с живыми объектами");
                                }

                                KernelTypes::UInt32 AllocatedPages = ObjectsAllocated.Load(KernelSync::MemoryOrder::Relaxed) / PageSize;
                                for (KernelTypes::UInt32 Page = 0; Page < AllocatedPages; ++Page)
                                {
                                    KernelMemory::Deallocate(Pages[Page]);
                                }

                                KernelMemory::Deallocate(Pages);
                            }
                        }

                        // Подготовить пул: можно создать до MaxObjects объектов, страницами по PageSizeValue (степень двойки)
                        void Init(KernelTypes::UInt32 MaxObjects, KernelTypes::UInt32 PageSizeValue)
                        {
                            KernelAssert::DebugCheck(PageSizeValue > 0 && KernelTypes::IsPowerOfTwo(PageSizeValue), "FixedSizeFreeList::Init: размер страницы должен быть степенью двойки");
                            KernelAssert::DebugCheck(Pages == nullptr, "FixedSizeFreeList::Init: пул уже подготовлен");

                            PageCount = (MaxObjects + PageSizeValue - 1) / PageSizeValue;
                            PageSize = PageSizeValue;
                            PageShift = KernelCompiler::CountTrailingZeros(PageSizeValue);
                            ObjectMask = PageSizeValue - 1;
                            FreeObjectCount.Store(PageCount * PageSizeValue, KernelSync::MemoryOrder::Relaxed);

                            Pages = static_cast<ObjectStorage**>(KernelMemory::Allocate(PageCount * sizeof(ObjectStorage*)));

                            ObjectsAllocated.Store(0, KernelSync::MemoryOrder::Relaxed);
                            FirstFreeObjectInNewPage.Store(0);

                            // Первый тег равен 1
                            AllocationTag.Store(1);

                            // Первый свободный: пусто (тег 0)
                            FirstFreeObjectAndTag.Store(InvalidObjectIndex);
                        }

                        // Создать объект без блокировок. Аргументы передаются конструктору. Возвращает индекс или InvalidObjectIndex, если места нет.
                        template <typename... Parameters>
                        KernelTypes::UInt32 ConstructObject(Parameters&&... Arguments)
                        {
                            for (;;)
                            {
                                KernelTypes::UInt64 FirstFreeAndTag = FirstFreeObjectAndTag.Load(KernelSync::MemoryOrder::Acquire);
                                KernelTypes::UInt32 FirstFree = static_cast<KernelTypes::UInt32>(FirstFreeAndTag);

                                if (FirstFree == InvalidObjectIndex)
                                {
                                    // Список свободных пуст, берём объект из ещё не использованной страницы
                                    FirstFree = FirstFreeObjectInNewPage.FetchAdd(1, KernelSync::MemoryOrder::Relaxed);

                                    // Acquire: если страницу выделил другой поток, указатель на неё должен быть виден
                                    if (FirstFree >= ObjectsAllocated.Load(KernelSync::MemoryOrder::Acquire))
                                    {
                                        KernelSync::LockGuard<KernelSync::Mutex> Lock(PageMutex);

                                        KernelTypes::UInt32 Allocated = ObjectsAllocated.Load(KernelSync::MemoryOrder::Relaxed); // меняется только под блокировкой
                                        while (FirstFree >= Allocated)
                                        {
                                            KernelTypes::UInt32 NextPage = Allocated / PageSize;
                                            if (NextPage == PageCount)
                                            {
                                                return InvalidObjectIndex; // места нет
                                            }

                                            ObjectStorage* Page = static_cast<ObjectStorage*>(KernelMemory::Allocate(
                                                PageSize * sizeof(ObjectStorage),
                                                alignof(ObjectStorage) > KernelCompiler::CacheLineSize ? alignof(ObjectStorage) : KernelCompiler::CacheLineSize));

                                            // Объекты строятся позже, а счётчики следующего свободного нужно создать сразу
                                            for (KernelTypes::UInt32 Index = 0; Index < PageSize; ++Index)
                                            {
                                                KernelTypes::ConstructAt(&Page[Index].NextFreeObject);
                                            }

                                            Pages[NextPage] = Page;
                                            Allocated += PageSize;
                                            ObjectsAllocated.Store(Allocated, KernelSync::MemoryOrder::Release); // Release: указатель на страницу виден читателям нового значения
                                        }
                                    }

                                    return FinishConstruct(FirstFree, KernelTypes::Forward<Parameters>(Arguments)...);
                                }
                                else
                                {
                                    KernelTypes::UInt32 NewFirstFree = GetStorage(FirstFree).NextFreeObject.Load(KernelSync::MemoryOrder::Acquire);

                                    KernelTypes::UInt64 NewFirstFreeAndTag = static_cast<KernelTypes::UInt64>(NewFirstFree)
                                        + (static_cast<KernelTypes::UInt64>(AllocationTag.FetchAdd(1, KernelSync::MemoryOrder::Relaxed)) << 32);

                                    if (FirstFreeObjectAndTag.CompareExchange(FirstFreeAndTag, NewFirstFreeAndTag, KernelSync::MemoryOrder::Release))
                                    {
                                        return FinishConstruct(FirstFree, KernelTypes::Forward<Parameters>(Arguments)...);
                                    }
                                }
                            }
                        }

                        // Уничтожить объект и вернуть в пул (без блокировок)
                        void DestructObject(KernelTypes::UInt32 ObjectIndex)
                        {
                            KernelAssert::DebugCheck(ObjectIndex != InvalidObjectIndex, "FixedSizeFreeList::DestructObject: недействительный индекс");

                            ObjectStorage& Storage = GetStorage(ObjectIndex);
                            KernelTypes::DestroyAt(&Storage.StoredObject);

                            for (;;)
                            {
                                KernelTypes::UInt64 FirstFreeAndTag = FirstFreeObjectAndTag.Load(KernelSync::MemoryOrder::Acquire);
                                KernelTypes::UInt32 FirstFree = static_cast<KernelTypes::UInt32>(FirstFreeAndTag);

                                Storage.NextFreeObject.Store(FirstFree, KernelSync::MemoryOrder::Release);

                                KernelTypes::UInt64 NewFirstFreeAndTag = static_cast<KernelTypes::UInt64>(ObjectIndex)
                                    + (static_cast<KernelTypes::UInt64>(AllocationTag.FetchAdd(1, KernelSync::MemoryOrder::Relaxed)) << 32);

                                if (FirstFreeObjectAndTag.CompareExchange(FirstFreeAndTag, NewFirstFreeAndTag, KernelSync::MemoryOrder::Release))
                                {
                                    if constexpr (KernelCompiler::IsDebugBuild)
                                    {
                                        FreeObjectCount.FetchAdd(1, KernelSync::MemoryOrder::Relaxed);
                                    }

                                    return;
                                }
                            }
                        }

                        // То же по указателю на объект
                        void DestructObject(Object* ObjectPointer)
                        {
                            KernelTypes::UInt32 Index = GetObjectIndex(ObjectPointer);
                            if constexpr (KernelCompiler::IsDebugBuild)
                            {
                                KernelAssert::DebugCheck(Index < ObjectsAllocated.Load(KernelSync::MemoryOrder::Relaxed), "FixedSizeFreeList::DestructObject: объект не из этого пула");
                            }
                            DestructObject(Index);
                        }

                        // Пакет объектов на уничтожение
                        struct Batch
                        {
                            KernelTypes::UInt32 FirstObjectIndex = InvalidObjectIndex;
                            KernelTypes::UInt32 LastObjectIndex = InvalidObjectIndex;
                            KernelTypes::UInt32 ObjectCount = 0;
                        };

                        // Добавить объект в пакет. Объект не меняется и не уничтожается, объекты лишь связываются в цепочку,
                        // чтобы весь пакет вернулся в пул одной атомарной операцией.
                        void AddObjectToBatch(Batch& TargetBatch, KernelTypes::UInt32 ObjectIndex)
                        {
                            KernelAssert::DebugCheck(TargetBatch.ObjectCount != KernelTypes::UInt32(-1), "FixedSizeFreeList: пакет уже освобождён");

                            KernelSync::Atomic<KernelTypes::UInt32>& NextFree = GetStorage(ObjectIndex).NextFreeObject;
                            if constexpr (KernelCompiler::IsDebugBuild)
                            {
                                KernelAssert::DebugCheck(NextFree.Load(KernelSync::MemoryOrder::Relaxed) == ObjectIndex, "FixedSizeFreeList: объект уже в списке свободных");
                            }
                            NextFree.Store(InvalidObjectIndex, KernelSync::MemoryOrder::Release);

                            if (TargetBatch.FirstObjectIndex == InvalidObjectIndex)
                            {
                                TargetBatch.FirstObjectIndex = ObjectIndex;
                            }
                            else
                            {
                                GetStorage(TargetBatch.LastObjectIndex).NextFreeObject.Store(ObjectIndex, KernelSync::MemoryOrder::Release);
                            }

                            TargetBatch.LastObjectIndex = ObjectIndex;
                            TargetBatch.ObjectCount++;
                        }

                        // Уничтожить пакет объектов (без блокировок)
                        void DestructObjectBatch(Batch& TargetBatch)
                        {
                            if (TargetBatch.FirstObjectIndex == InvalidObjectIndex)
                            {
                                return;
                            }

                            if constexpr (!KernelTypes::IsTriviallyDestructible<Object>)
                            {
                                KernelTypes::UInt32 ObjectIndex = TargetBatch.FirstObjectIndex;
                                do
                                {
                                    ObjectStorage& Storage = GetStorage(ObjectIndex);
                                    KernelTypes::DestroyAt(&Storage.StoredObject);
                                    ObjectIndex = Storage.NextFreeObject.Load(KernelSync::MemoryOrder::Relaxed);
                                }
                                while (ObjectIndex != InvalidObjectIndex);
                            }

                            ObjectStorage& Last = GetStorage(TargetBatch.LastObjectIndex);
                            for (;;)
                            {
                                KernelTypes::UInt64 FirstFreeAndTag = FirstFreeObjectAndTag.Load(KernelSync::MemoryOrder::Acquire);
                                KernelTypes::UInt32 FirstFree = static_cast<KernelTypes::UInt32>(FirstFreeAndTag);

                                // Последний объект пакета указывает на прежний первый свободный
                                Last.NextFreeObject.Store(FirstFree, KernelSync::MemoryOrder::Release);

                                KernelTypes::UInt64 NewFirstFreeAndTag = static_cast<KernelTypes::UInt64>(TargetBatch.FirstObjectIndex)
                                    + (static_cast<KernelTypes::UInt64>(AllocationTag.FetchAdd(1, KernelSync::MemoryOrder::Relaxed)) << 32);

                                if (FirstFreeObjectAndTag.CompareExchange(FirstFreeAndTag, NewFirstFreeAndTag, KernelSync::MemoryOrder::Release))
                                {
                                    if constexpr (KernelCompiler::IsDebugBuild)
                                    {
                                        FreeObjectCount.FetchAdd(TargetBatch.ObjectCount, KernelSync::MemoryOrder::Relaxed);
                                        TargetBatch.ObjectCount = KernelTypes::UInt32(-1); // пакет использован
                                    }

                                    return;
                                }
                            }
                        }

                        // Объект по индексу
                        Object& Get(KernelTypes::UInt32 ObjectIndex)
                        {
                            return GetStorage(ObjectIndex).StoredObject;
                        }

                        const Object& Get(KernelTypes::UInt32 ObjectIndex) const
                        {
                            return GetStorage(ObjectIndex).StoredObject;
                        }

                        // Индекс объекта, созданного этим пулом
                        KernelTypes::UInt32 GetObjectIndex(const Object* ObjectPointer) const
                        {
                            return reinterpret_cast<const ObjectStorage*>(ObjectPointer)->NextFreeObject.Load(KernelSync::MemoryOrder::Relaxed);
                        }

                    private:
                        template <typename... Parameters>
                        KernelTypes::UInt32 FinishConstruct(KernelTypes::UInt32 Index, Parameters&&... Arguments)
                        {
                            if constexpr (KernelCompiler::IsDebugBuild)
                            {
                                FreeObjectCount.FetchSub(1, KernelSync::MemoryOrder::Relaxed);
                            }

                            ObjectStorage& Storage = GetStorage(Index);
                            KernelTypes::ConstructAt(&Storage.StoredObject, KernelTypes::Forward<Parameters>(Arguments)...);
                            Storage.NextFreeObject.Store(Index, KernelSync::MemoryOrder::Release);
                            return Index;
                        }
                };
            }
        }
    }
}
