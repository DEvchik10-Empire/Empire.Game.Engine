#include "Empire.Kernel/Kernel.Memory/Memory.hpp"

#include "Empire.Kernel/Kernel.Assert/Assert.hpp"
#include "Empire.Kernel/Kernel.Sync/Sync.Atomic.hpp"
#include "Empire.Kernel/Kernel.Sync/Sync.Mutex.hpp"
#include "Empire.Kernel/Kernel.System/System.hpp"
#include "Empire.Kernel/Kernel.Types/Types.Utility.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        using namespace EmpireKernel::KernelTypes;

        namespace EmpireKernel
        {
            namespace KernelMemory
            {
                // Аллокатор поверх страниц ОС (KernelSystem::ReservePages).
                //
                // Малые блоки (до 2 КиБ, выравнивание до 16) делятся на классы размеров. Блок это 16 байт заголовка
                // (размер и класс) и полезная часть; в свободном блоке указатель на следующий лежит в полезной части,
                // поэтому класс в заголовке записывается один раз при нарезке и больше не меняется.
                // Куски по 256 КиБ нарезаются последовательно и в ОС не возвращаются.
                //
                // У каждого потока есть свой кэш блоков (пачками из общего пула) и свой блок счётчиков:
                // обычный путь выделения и освобождения не берёт мьютекс и не делает атомарных операций с read-modify-write.
                //
                // Большие блоки (и блоки с выравниванием больше 16) получают собственные страницы и возвращаются в ОС.
                namespace
                {
                    struct BlockHeader
                    {
                        Size ByteCount;
                        Size ClassIndex;
                    };

                    // Лежит прямо перед BlockHeader и есть только у больших блоков.
                    struct LargeInfo
                    {
                        void* RawBase;
                        Size RawSize;
                        Size Alignment;
                        Size Padding;
                    };

                    static_assert(sizeof(BlockHeader) == 16, "BlockHeader must keep payloads 16-byte aligned");
                    static_assert(sizeof(LargeInfo) % 16 == 0, "LargeInfo must keep payloads 16-byte aligned");

                    // Лежит в полезной части свободного блока. NextBatch используется только у первого блока целой пачки.
                    struct FreeBlock
                    {
                        FreeBlock* Next;
                        FreeBlock* NextBatch;
                    };

                    constexpr Size LargeBlock = MaxValue<Size>();
                    constexpr Size MinimumAlignment = 16;
                    constexpr Size ChunkByteCount = 128 * 1024;
                    constexpr Size AutoTrimBytes = 256 * 1024 * 1024;
                    constexpr Size ClassCount = 14;
                    constexpr Size MaxCacheBatch = 32;
                    constexpr Size BatchByteBudget = 8 * 1024;
                    constexpr Size MaxSmallByteCount = 2048;
                    constexpr Size ClassSizes[ClassCount] = {16, 32, 48, 64, 96, 128, 192, 256, 384, 512, 768, 1024, 1536, 2048};

                    // Таблица «размер, округлённый вверх до 16 → класс», чтобы не искать класс циклом.
                    struct ClassTable
                    {
                        UInt8 Index[MaxSmallByteCount / 16 + 1];
                    };

                    constexpr ClassTable BuildClassTable() noexcept
                    {
                        ClassTable Table = {};

                        for (Size Slot = 0; Slot <= MaxSmallByteCount / 16; Slot++)
                        {
                            Size Class = 0;

                            while (Class + 1 < ClassCount && ClassSizes[Class] < Slot * 16)
                                Class++;

                            Table.Index[Slot] = static_cast<UInt8>(Class);
                        }

                        return Table;
                    }

                    constexpr ClassTable ClassLookup = BuildClassTable();

                    // Размер пачки зависит от класса: порядка 8 КиБ на пачку (от 2 до 32 блоков).
                    struct BatchTable
                    {
                        UInt8 Count[ClassCount];
                    };

                    constexpr BatchTable BuildBatchTable() noexcept
                    {
                        BatchTable Table = {};

                        for (Size Class = 0; Class < ClassCount; Class++)
                        {
                            const Size Count = BatchByteBudget / (16 + ClassSizes[Class]);
                            Table.Count[Class] = static_cast<UInt8>(Count < 2 ? 2 : (Count > MaxCacheBatch ? MaxCacheBatch : Count));
                        }

                        return Table;
                    }

                    constexpr BatchTable BatchSizes = BuildBatchTable();

                    inline Size BatchFor(Size ClassIndex) noexcept
                    {
                        return BatchSizes.Count[ClassIndex];
                    }

                    inline Size FindClassIndex(Size ByteCount) noexcept
                    {
                        return ByteCount > MaxSmallByteCount ? LargeBlock : ClassLookup.Index[(ByteCount + 15) >> 4];
                    }

                    // Счётчики одного потока (живые блоки = выделения минус освобождения). Пишет только владелец (обычные load и store без lock-префикса),
                    // читают все. Освобождение в другом потоке пишет в счётчики того потока, поэтому отдельный блок
                    // может «уходить в минус» (беззнаковое переполнение), а сумма по всем блокам остаётся точной.
                    struct alignas(64) CounterBlock
                    {
                        KernelSync::Atomic<UInt64> LiveBytes;
                        KernelSync::Atomic<UInt64> Allocations;
                        KernelSync::Atomic<UInt64> Deallocations;
                        CounterBlock* NextInList = nullptr;
                        CounterBlock* NextFree = nullptr;
                    };

                    KernelSync::Mutex PoolMutex;
                    // Общий пул: целые пачки (BatchFor блоков, связанные через Next; пачки связаны через NextBatch у первого блока)
                    // отдаются и берутся за O(1), одиночные блоки (остатки завершившихся потоков) лежат в FreeLists.
                    FreeBlock* FreeBatches[ClassCount] = {};
                    FreeBlock* FreeLists[ClassCount] = {};
                    UInt8* ChunkCursor = nullptr;
                    Size ChunkRemaining = 0;

                    // Куски (spans), полученные от ОС для малых блоков, в порядке возрастания адреса. Нужны только для Trim.
                    struct SpanInfo
                    {
                        UInt8* Base;
                        Size ByteCount;
                        Size Carved;
                        Size Pooled;
                    };

                    SpanInfo* Spans = nullptr;
                    Size SpanCount = 0;
                    Size SpanCapacity = 0;
                    UInt8* ActiveSpanBase = nullptr;
                    Size ActiveCarved = 0;
                    Size ReservedBytes = 0;
                    Size PooledBatchBytes = 0;
                    Size NextAutoTrimAt = AutoTrimBytes;

                    KernelSync::Atomic<CounterBlock*> CounterList;
                    CounterBlock* FreeCounters = nullptr;
                    UInt8* CounterCursor = nullptr;
                    Size CounterRemaining = 0;

                    // Счётчики потоков, у которых нет своего блока (поток уже завершается или не хватило памяти): атомарно.
                    CounterBlock SharedCounters;

                    CounterBlock* AcquireCounters() noexcept
                    {
                        KernelSync::LockGuard Lock(PoolMutex);

                        if (CounterBlock* Free = FreeCounters)
                        {
                            FreeCounters = Free->NextFree;
                            return Free;
                        }

                        if (CounterRemaining < sizeof(CounterBlock))
                        {
                            const Size PageSize = KernelSystem::GetPageSize();
                            UInt8* Page = static_cast<UInt8*>(KernelSystem::ReservePages(PageSize));

                            if (Page == nullptr)
                                return nullptr;

                            CounterCursor = Page;
                            CounterRemaining = PageSize;
                        }

                        CounterBlock* Block = ::new (PlacementTag{}, static_cast<void*>(CounterCursor)) CounterBlock();
                        CounterCursor += sizeof(CounterBlock);
                        CounterRemaining -= sizeof(CounterBlock);

                        Block->NextInList = CounterList.Load(KernelSync::MemoryOrder::Relaxed);
                        CounterList.Store(Block, KernelSync::MemoryOrder::Release);
                        return Block;
                    }

                    // Блок остаётся в общем списке (накопленные значения входят в сумму) и может достаться новому потоку.
                    void ReleaseCounters(CounterBlock* Block) noexcept
                    {
                        KernelSync::LockGuard Lock(PoolMutex);

                        Block->NextFree = FreeCounters;
                        FreeCounters = Block;
                    }

                    UInt64 SumCounters(KernelSync::Atomic<UInt64> CounterBlock::*Member) noexcept
                    {
                        UInt64 Total = (SharedCounters.*Member).Load(KernelSync::MemoryOrder::Relaxed);

                        for (CounterBlock* Block = CounterList.Load(KernelSync::MemoryOrder::Acquire); Block != nullptr; Block = Block->NextInList)
                            Total += (Block->*Member).Load(KernelSync::MemoryOrder::Relaxed);

                        return Total;
                    }

                    Bool IsValidAlignment(Size Alignment) noexcept
                    {
                        return Alignment != 0 && (Alignment & (Alignment - 1)) == 0;
                    }

                    inline BlockHeader* GetHeader(void* MemoryBlock) noexcept
                    {
                        return reinterpret_cast<BlockHeader*>(reinterpret_cast<UIntPtr>(MemoryBlock) - sizeof(BlockHeader));
                    }

                    inline const BlockHeader* GetHeader(const void* MemoryBlock) noexcept
                    {
                        return reinterpret_cast<const BlockHeader*>(reinterpret_cast<UIntPtr>(MemoryBlock) - sizeof(BlockHeader));
                    }

                    inline LargeInfo* GetLargeInfo(BlockHeader* Header) noexcept
                    {
                        return reinterpret_cast<LargeInfo*>(reinterpret_cast<UIntPtr>(Header) - sizeof(LargeInfo));
                    }

                    // Ниже все функции со словом Locked вызываются под PoolMutex.

                    // Индекс первого куска с Base > Address (то есть куски до него начинаются не выше Address).
                    Size UpperBoundSpan(const UInt8* Address) noexcept
                    {
                        Size Low = 0;
                        Size High = SpanCount;

                        while (Low < High)
                        {
                            const Size Middle = Low + (High - Low) / 2;

                            if (Spans[Middle].Base <= Address)
                                Low = Middle + 1;
                            else
                                High = Middle;
                        }

                        return Low;
                    }

                    // Кусок, которому принадлежит адрес (nullptr, если адрес не из малых блоков).
                    SpanInfo* FindSpanLocked(const void* Address) noexcept
                    {
                        const UInt8* Bytes = static_cast<const UInt8*>(Address);
                        const Size After = UpperBoundSpan(Bytes);

                        if (After == 0)
                            return nullptr;

                        SpanInfo* Candidate = &Spans[After - 1];
                        return Bytes < Candidate->Base + Candidate->ByteCount ? Candidate : nullptr;
                    }

                    void CommitActiveSpanLocked() noexcept
                    {
                        if (ActiveSpanBase == nullptr)
                            return;

                        if (SpanInfo* Active = FindSpanLocked(ActiveSpanBase))
                            Active->Carved = ActiveCarved;
                    }

                    Bool RegisterSpanLocked(UInt8* Base, Size ByteCount) noexcept
                    {
                        if (SpanCount == SpanCapacity)
                        {
                            const Size NewCapacity = SpanCapacity == 0 ? 256 : SpanCapacity * 2;
                            const Size NewBytes = NewCapacity * sizeof(SpanInfo);
                            SpanInfo* NewTable = static_cast<SpanInfo*>(KernelSystem::ReservePages(NewBytes));

                            if (NewTable == nullptr)
                                return false;

                            for (Size Index = 0; Index < SpanCount; Index++)
                                NewTable[Index] = Spans[Index];

                            if (Spans != nullptr)
                                KernelSystem::ReleasePages(Spans, SpanCapacity * sizeof(SpanInfo));

                            Spans = NewTable;
                            SpanCapacity = NewCapacity;
                        }

                        Size Position = UpperBoundSpan(Base);

                        for (Size Index = SpanCount; Index > Position; Index--)
                            Spans[Index] = Spans[Index - 1];

                        Spans[Position] = SpanInfo{Base, ByteCount, 0, 0};
                        SpanCount++;
                        ReservedBytes += ByteCount;
                        return true;
                    }

                    // Возвращает в ОС все куски, все блоки которых лежат в общем пуле. Возвращает число освобождённых байт.
                    Size TrimLocked() noexcept
                    {
                        if (SpanCount == 0)
                            return 0;

                        CommitActiveSpanLocked();

                        for (Size Index = 0; Index < SpanCount; Index++)
                            Spans[Index].Pooled = 0;

                        // Сколько блоков каждого куска лежит в пуле.
                        for (Size Class = 0; Class < ClassCount; Class++)
                        {
                            for (FreeBlock* Batch = FreeBatches[Class]; Batch != nullptr; Batch = Batch->NextBatch)
                                for (FreeBlock* Block = Batch; Block != nullptr; Block = Block->Next)
                                    if (SpanInfo* Span = FindSpanLocked(Block))
                                        Span->Pooled++;

                            for (FreeBlock* Block = FreeLists[Class]; Block != nullptr; Block = Block->Next)
                                if (SpanInfo* Span = FindSpanLocked(Block))
                                    Span->Pooled++;
                        }

                        Size FreeSpans = 0;

                        for (Size Index = 0; Index < SpanCount; Index++)
                            if (Spans[Index].Carved != 0 && Spans[Index].Pooled == Spans[Index].Carved)
                                FreeSpans++;

                        if (FreeSpans == 0)
                            return 0;

                        auto IsReleased = [](const void* Block) noexcept
                        {
                            const SpanInfo* Span = FindSpanLocked(Block);
                            return Span != nullptr && Span->Carved != 0 && Span->Pooled == Span->Carved;
                        };

                        // Убираем из списков блоки освобождаемых кусков. Пачка, из которой что-то убрали, распадается на одиночные блоки.
                        for (Size Class = 0; Class < ClassCount; Class++)
                        {
                            FreeBlock* Batches = FreeBatches[Class];
                            FreeBatches[Class] = nullptr;
                            FreeBlock* Singles = FreeLists[Class];
                            FreeLists[Class] = nullptr;

                            while (Batches != nullptr)
                            {
                                FreeBlock* NextBatch = Batches->NextBatch;
                                Bool Intact = true;

                                for (FreeBlock* Block = Batches; Block != nullptr; Block = Block->Next)
                                {
                                    if (IsReleased(Block))
                                    {
                                        Intact = false;
                                        break;
                                    }
                                }

                                if (Intact)
                                {
                                    Batches->NextBatch = FreeBatches[Class];
                                    FreeBatches[Class] = Batches;
                                }
                                else
                                {
                                    FreeBlock* Block = Batches;

                                    while (Block != nullptr)
                                    {
                                        FreeBlock* Next = Block->Next;

                                        if (!IsReleased(Block))
                                        {
                                            Block->Next = FreeLists[Class];
                                            FreeLists[Class] = Block;
                                        }

                                        Block = Next;
                                    }
                                }

                                Batches = NextBatch;
                            }

                            while (Singles != nullptr)
                            {
                                FreeBlock* Next = Singles->Next;

                                if (!IsReleased(Singles))
                                {
                                    Singles->Next = FreeLists[Class];
                                    FreeLists[Class] = Singles;
                                }

                                Singles = Next;
                            }
                        }

                        // Освобождаем сами куски и убираем их из таблицы (порядок остальных сохраняется).
                        Size Released = 0;
                        Size Kept = 0;

                        for (Size Index = 0; Index < SpanCount; Index++)
                        {
                            const SpanInfo Span = Spans[Index];

                            if (Span.Carved != 0 && Span.Pooled == Span.Carved)
                            {
                                if (Span.Base == ActiveSpanBase)
                                {
                                    ActiveSpanBase = nullptr;
                                    ActiveCarved = 0;
                                    ChunkCursor = nullptr;
                                    ChunkRemaining = 0;
                                }

                                KernelSystem::ReleasePages(Span.Base, Span.ByteCount);
                                Released += Span.ByteCount;
                            }
                            else
                            {
                                Spans[Kept++] = Span;
                            }
                        }

                        SpanCount = Kept;
                        ReservedBytes -= Released;
                        PooledBatchBytes = 0;

                        for (Size Class = 0; Class < ClassCount; Class++)
                            for (FreeBlock* Batch = FreeBatches[Class]; Batch != nullptr; Batch = Batch->NextBatch)
                                PooledBatchBytes += BatchFor(Class) * (sizeof(BlockHeader) + ClassSizes[Class]);

                        return Released;
                    }

                    // Берёт до Wanted блоков из общего пула одним захватом мьютекса и возвращает их связным списком.
                    FreeBlock* TakeBatchFromPool(Size ClassIndex, Size Wanted, Size& Taken) noexcept
                    {
                        KernelSync::LockGuard Lock(PoolMutex);

                        // Быстрый путь: готовая пачка целиком, без обхода блоков (они «холодные» в кэше процессора).
                        if (Wanted == BatchFor(ClassIndex))
                        {
                            if (FreeBlock* Batch = FreeBatches[ClassIndex])
                            {
                                FreeBatches[ClassIndex] = Batch->NextBatch;
                                PooledBatchBytes -= Wanted * (sizeof(BlockHeader) + ClassSizes[ClassIndex]);
                                Taken = Wanted;
                                return Batch;
                            }
                        }

                        FreeBlock* Head = nullptr;
                        const Size BlockSize = sizeof(BlockHeader) + ClassSizes[ClassIndex];
                        Taken = 0;

                        while (Taken < Wanted)
                        {
                            FreeBlock* Block = FreeLists[ClassIndex];

                            if (Block != nullptr)
                            {
                                FreeLists[ClassIndex] = Block->Next;
                            }
                            else
                            {
                                if (ChunkRemaining < BlockSize)
                                {
                                    UInt8* Chunk = static_cast<UInt8*>(KernelSystem::ReservePages(ChunkByteCount));

                                    if (Chunk == nullptr)
                                        break;

                                    CommitActiveSpanLocked();

                                    if (!RegisterSpanLocked(Chunk, ChunkByteCount))
                                    {
                                        KernelSystem::ReleasePages(Chunk, ChunkByteCount);
                                        break;
                                    }

                                    ActiveSpanBase = Chunk;
                                    ActiveCarved = 0;
                                    ChunkCursor = Chunk;
                                    ChunkRemaining = ChunkByteCount;
                                }

                                ActiveCarved++;

                                BlockHeader* Header = reinterpret_cast<BlockHeader*>(ChunkCursor);
                                Header->ByteCount = 0;
                                Header->ClassIndex = ClassIndex;

                                Block = reinterpret_cast<FreeBlock*>(ChunkCursor + sizeof(BlockHeader));
                                ChunkCursor += BlockSize;
                                ChunkRemaining -= BlockSize;
                            }

                            Block->Next = Head;
                            Head = Block;
                            Taken++;
                        }

                        return Head;
                    }

                    // Возвращает связный список блоков (от Head до Tail) в общий пул одним захватом мьютекса.
                    void GiveBatchToPool(Size ClassIndex, FreeBlock* Head, FreeBlock* Tail) noexcept
                    {
                        KernelSync::LockGuard Lock(PoolMutex);

                        Tail->Next = FreeLists[ClassIndex];
                        FreeLists[ClassIndex] = Head;
                    }

                    // Отдаёт целую пачку (ровно BatchFor блоков, связанных через Next) в общий пул за O(1).
                    void GiveWholeBatchToPool(Size ClassIndex, FreeBlock* First) noexcept
                    {
                        KernelSync::LockGuard Lock(PoolMutex);

                        First->NextBatch = FreeBatches[ClassIndex];
                        FreeBatches[ClassIndex] = First;
                        PooledBatchBytes += BatchFor(ClassIndex) * (sizeof(BlockHeader) + ClassSizes[ClassIndex]);

                        // Страховка: при очень большом простое в пуле вернуть пустые куски ОС (обход O(размер пула), поэтому редко).
                        if (PooledBatchBytes >= NextAutoTrimAt) [[unlikely]]
                        {
                            TrimLocked();
                            NextAutoTrimAt = PooledBatchBytes * 2 > AutoTrimBytes ? PooledBatchBytes * 2 : AutoTrimBytes;
                        }
                    }

                    // Состояние потока без конструктора и деструктора: доступ к нему не проверяет инициализацию.
                    // Phase: 0 ещё не использовался, 1 готов, 2 поток завершается (кэша нет, всё идёт напрямую через пул).
                    struct ThreadState
                    {
                        FreeBlock* Heads[ClassCount];
                        Size Counts[ClassCount];
                        CounterBlock* Counters;
                        UInt32 Phase;
                    };

                    constinit thread_local ThreadState State = {};

                    // Возвращает весь кэш потока в общий пул (одиночными блоками).
                    void FlushWholeCache(ThreadState& Current) noexcept
                    {
                        for (Size Index = 0; Index < ClassCount; Index++)
                        {
                            FreeBlock* Tail = Current.Heads[Index];

                            if (Tail != nullptr)
                            {
                                while (Tail->Next != nullptr)
                                    Tail = Tail->Next;

                                GiveBatchToPool(Index, Current.Heads[Index], Tail);
                            }

                            Current.Heads[Index] = nullptr;
                            Current.Counts[Index] = 0;
                        }
                    }

                    // Живёт только ради деструктора: при завершении потока возвращает кэш и счётчики.
                    struct ThreadExit
                    {
                        ~ThreadExit()
                        {
                            ThreadState& Current = State;
                            Current.Phase = 2;

                            FlushWholeCache(Current);

                            if (Current.Counters != nullptr)
                            {
                                CounterBlock* Block = Current.Counters;
                                Current.Counters = nullptr;
                                ReleaseCounters(Block);
                            }
                        }
                    };

                    [[gnu::noinline]] void InitializeThread(ThreadState& Current) noexcept
                    {
                        static thread_local ThreadExit Exit;
                        (void) Exit;

                        Current.Counters = AcquireCounters();
                        Current.Phase = 1;
                    }

                    // nullptr, если поток уже завершается.
                    inline ThreadState* GetThreadState() noexcept
                    {
                        ThreadState& Current = State;

                        if (Current.Phase == 1) [[likely]]
                            return &Current;

                        if (Current.Phase == 0)
                        {
                            InitializeThread(Current);
                            return &Current;
                        }

                        return nullptr;
                    }

                    inline void CountAllocation(ThreadState* Current, Size ByteCount) noexcept
                    {
                        if (Current != nullptr && Current->Counters != nullptr) [[likely]]
                        {
                            CounterBlock& Block = *Current->Counters;
                            Block.LiveBytes.Store(Block.LiveBytes.Load(KernelSync::MemoryOrder::Relaxed) + ByteCount, KernelSync::MemoryOrder::Relaxed);
                            Block.Allocations.Store(Block.Allocations.Load(KernelSync::MemoryOrder::Relaxed) + 1, KernelSync::MemoryOrder::Relaxed);
                        }
                        else
                        {
                            SharedCounters.LiveBytes.FetchAdd(ByteCount, KernelSync::MemoryOrder::Relaxed);
                            SharedCounters.Allocations.FetchAdd(1, KernelSync::MemoryOrder::Relaxed);
                        }
                    }

                    inline void CountDeallocation(ThreadState* Current, Size ByteCount) noexcept
                    {
                        if (Current != nullptr && Current->Counters != nullptr) [[likely]]
                        {
                            CounterBlock& Block = *Current->Counters;
                            Block.LiveBytes.Store(Block.LiveBytes.Load(KernelSync::MemoryOrder::Relaxed) - ByteCount, KernelSync::MemoryOrder::Relaxed);
                            Block.Deallocations.Store(Block.Deallocations.Load(KernelSync::MemoryOrder::Relaxed) + 1, KernelSync::MemoryOrder::Relaxed);
                        }
                        else
                        {
                            SharedCounters.LiveBytes.FetchSub(ByteCount, KernelSync::MemoryOrder::Relaxed);
                            SharedCounters.Deallocations.FetchAdd(1, KernelSync::MemoryOrder::Relaxed);
                        }
                    }

                    [[gnu::noinline]] FreeBlock* RefillCache(ThreadState& Current, Size ClassIndex) noexcept
                    {
                        Size Taken = 0;
                        FreeBlock* Batch = TakeBatchFromPool(ClassIndex, BatchFor(ClassIndex), Taken);

                        if (Batch == nullptr)
                            return nullptr;

                        Current.Heads[ClassIndex] = Batch->Next;
                        Current.Counts[ClassIndex] = Taken - 1;
                        return Batch;
                    }

                    // Отдаёт в пул самые свежие Batch блоков целой пачкой (они ещё в кэше процессора, обход дешёвый), старые оставляет потоку.
                    [[gnu::noinline]] void FlushCache(ThreadState& Current, Size ClassIndex) noexcept
                    {
                        const Size Batch = BatchFor(ClassIndex);
                        FreeBlock* First = Current.Heads[ClassIndex];
                        FreeBlock* Last = First;

                        for (Size Index = 1; Index < Batch; Index++)
                            Last = Last->Next;

                        Current.Heads[ClassIndex] = Last->Next;
                        Current.Counts[ClassIndex] -= Batch;
                        Last->Next = nullptr;

                        GiveWholeBatchToPool(ClassIndex, First);
                    }

                    void* AllocateSmall(Size ByteCount, Size ClassIndex)
                    {
                        ThreadState* Current = GetThreadState();
                        FreeBlock* Block;

                        if (Current != nullptr) [[likely]]
                        {
                            Block = Current->Heads[ClassIndex];

                            if (Block != nullptr) [[likely]]
                            {
                                Current->Heads[ClassIndex] = Block->Next;
                                Current->Counts[ClassIndex]--;
                            }
                            else
                            {
                                Block = RefillCache(*Current, ClassIndex);
                            }
                        }
                        else
                        {
                            Size Taken = 0;
                            Block = TakeBatchFromPool(ClassIndex, 1, Taken);
                        }

                        if (Block == nullptr) [[unlikely]]
                            throw KernelAssert::AllocationError("Out of memory");

                        GetHeader(static_cast<void*>(Block))->ByteCount = ByteCount;
                        CountAllocation(Current, ByteCount);
                        return Block;
                    }

                    void DeallocateSmall(BlockHeader* Header, Size ByteCount, Size ClassIndex) noexcept
                    {
                        ThreadState* Current = GetThreadState();
                        FreeBlock* Free = reinterpret_cast<FreeBlock*>(reinterpret_cast<UIntPtr>(Header) + sizeof(BlockHeader));

                        CountDeallocation(Current, ByteCount);

                        if (Current == nullptr) [[unlikely]]
                        {
                            Free->Next = nullptr;
                            GiveBatchToPool(ClassIndex, Free, Free);
                            return;
                        }

                        Free->Next = Current->Heads[ClassIndex];
                        Current->Heads[ClassIndex] = Free;

                        if (++Current->Counts[ClassIndex] >= BatchFor(ClassIndex) * 2) [[unlikely]]
                            FlushCache(*Current, ClassIndex);
                    }

                    void* AllocateLarge(Size ByteCount, Size Alignment)
                    {
                        const Size Overhead = Alignment + sizeof(LargeInfo) + sizeof(BlockHeader);
                        const Size PageSize = KernelSystem::GetPageSize();

                        if (ByteCount > MaxValue<Size>() - Overhead - PageSize)
                            throw KernelAssert::AllocationError("Allocation size is too large");

                        const Size RawSize = (ByteCount + Overhead + PageSize - 1) / PageSize * PageSize;
                        void* RawBase = KernelSystem::ReservePages(RawSize);

                        if (RawBase == nullptr)
                            throw KernelAssert::AllocationError("Out of memory");

                        const UIntPtr Start = reinterpret_cast<UIntPtr>(RawBase) + sizeof(LargeInfo) + sizeof(BlockHeader);
                        const UIntPtr Aligned = (Start + Alignment - 1) & ~(static_cast<UIntPtr>(Alignment) - 1);
                        void* MemoryBlock = reinterpret_cast<void*>(Aligned);

                        BlockHeader* Header = GetHeader(MemoryBlock);
                        Header->ByteCount = ByteCount;
                        Header->ClassIndex = LargeBlock;

                        LargeInfo* Info = GetLargeInfo(Header);
                        Info->RawBase = RawBase;
                        Info->RawSize = RawSize;
                        Info->Alignment = Alignment;
                        Info->Padding = 0;

                        CountAllocation(GetThreadState(), ByteCount);
                        return MemoryBlock;
                    }
                }

                void* Allocate(Size ByteCount, Size Alignment)
                {
                    if (Alignment != MinimumAlignment) [[unlikely]]
                    {
                        if (!IsValidAlignment(Alignment))
                            throw KernelAssert::AllocationError("Alignment must be a non-zero power of two");

                        if (Alignment < MinimumAlignment)
                            Alignment = MinimumAlignment;
                    }

                    if (ByteCount == 0) [[unlikely]]
                        ByteCount = 1;

                    const Size ClassIndex = Alignment == MinimumAlignment ? FindClassIndex(ByteCount) : LargeBlock;

                    return ClassIndex == LargeBlock ? AllocateLarge(ByteCount, Alignment) : AllocateSmall(ByteCount, ClassIndex);
                }

                void* Reallocate(void* MemoryBlock, Size NewByteCount)
                {
                    if (MemoryBlock == nullptr)
                        return Allocate(NewByteCount);

                    if (NewByteCount == 0)
                    {
                        Deallocate(MemoryBlock);
                        return nullptr;
                    }

                    BlockHeader* OldHeader = GetHeader(MemoryBlock);
                    const Size OldByteCount = OldHeader->ByteCount;
                    const Size OldAlignment = OldHeader->ClassIndex == LargeBlock ? GetLargeInfo(OldHeader)->Alignment : MinimumAlignment;

                    void* NewBlock = Allocate(NewByteCount, OldAlignment);
                    CopyBytes(NewBlock, MemoryBlock, Min(OldByteCount, NewByteCount));
                    Deallocate(MemoryBlock);

                    return NewBlock;
                }

                void Deallocate(void* MemoryBlock) noexcept
                {
                    if (MemoryBlock == nullptr)
                        return;

                    BlockHeader* Header = GetHeader(MemoryBlock);

                    const Size ByteCount = Header->ByteCount;
                    const Size ClassIndex = Header->ClassIndex;

                    if (ClassIndex != LargeBlock) [[likely]]
                    {
                        DeallocateSmall(Header, ByteCount, ClassIndex);
                        return;
                    }

                    const LargeInfo Info = *GetLargeInfo(Header);

                    CountDeallocation(GetThreadState(), ByteCount);
                    KernelSystem::ReleasePages(Info.RawBase, Info.RawSize);
                }

                Size GetBlockByteCount(const void* MemoryBlock) noexcept
                {
                    return MemoryBlock == nullptr ? 0 : GetHeader(MemoryBlock)->ByteCount;
                }

                void CopyBytes(void* Destination, const void* Source, Size ByteCount) noexcept
                {
                    UInt8* Target = static_cast<UInt8*>(Destination);
                    const UInt8* Origin = static_cast<const UInt8*>(Source);

                    for (Size Index = 0; Index < ByteCount; Index++)
                        Target[Index] = Origin[Index];
                }

                void MoveBytes(void* Destination, const void* Source, Size ByteCount) noexcept
                {
                    UInt8* Target = static_cast<UInt8*>(Destination);
                    const UInt8* Origin = static_cast<const UInt8*>(Source);

                    if (Target == Origin || ByteCount == 0)
                        return;

                    if (Target < Origin)
                    {
                        for (Size Index = 0; Index < ByteCount; Index++)
                            Target[Index] = Origin[Index];
                    }
                    else
                    {
                        for (Size Index = ByteCount; Index > 0; Index--)
                            Target[Index - 1] = Origin[Index - 1];
                    }
                }

                void FillBytes(void* Destination, UInt8 Value, Size ByteCount) noexcept
                {
                    UInt8* Target = static_cast<UInt8*>(Destination);

                    for (Size Index = 0; Index < ByteCount; Index++)
                        Target[Index] = Value;
                }

                Int32 CompareBytes(const void* Left, const void* Right, Size ByteCount) noexcept
                {
                    const UInt8* LeftBytes = static_cast<const UInt8*>(Left);
                    const UInt8* RightBytes = static_cast<const UInt8*>(Right);

                    for (Size Index = 0; Index < ByteCount; Index++)
                    {
                        if (LeftBytes[Index] != RightBytes[Index])
                            return LeftBytes[Index] < RightBytes[Index] ? -1 : 1;
                    }

                    return 0;
                }


                Size Trim() noexcept
                {
                    // Кэш вызывающего потока тоже уходит в пул, иначе его блоки удерживали бы куски.
                    if (ThreadState* Current = GetThreadState())
                        FlushWholeCache(*Current);

                    KernelSync::LockGuard Lock(PoolMutex);
                    return TrimLocked();
                }

                Size GetReservedByteCount() noexcept
                {
                    KernelSync::LockGuard Lock(PoolMutex);
                    return ReservedBytes;
                }

                UInt64 GetLiveByteCount() noexcept
                {
                    return SumCounters(&CounterBlock::LiveBytes);
                }

                UInt64 GetLiveBlockCount() noexcept
                {
                    return SumCounters(&CounterBlock::Allocations) - SumCounters(&CounterBlock::Deallocations);
                }

                UInt64 GetTotalAllocationCount() noexcept
                {
                    return SumCounters(&CounterBlock::Allocations);
                }
            }
        }
    }
}
