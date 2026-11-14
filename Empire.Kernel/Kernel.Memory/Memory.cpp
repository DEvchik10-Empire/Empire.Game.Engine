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
                // Малые блоки (до 2 КиБ, выравнивание до 16) берутся из классов размеров со списками свободных блоков,
                // куски по 256 КиБ нарезаются последовательно и в ОС не возвращаются.
                // У каждого потока есть свой кэш блоков (пачками по 32 из общего пула), обычный путь не берёт мьютекс.
                // Большие блоки получают собственные страницы и возвращаются в ОС при освобождении.
                namespace
                {
                    struct BlockHeader
                    {
                        void* RawBase;
                        Size RawSize;
                        Size ByteCount;
                        Size Alignment;
                        Size ClassIndex;
                        Size Padding;
                    };

                    static_assert(sizeof(BlockHeader) % 16 == 0, "BlockHeader must keep payloads 16-byte aligned");

                    struct FreeBlock
                    {
                        FreeBlock* Next;
                    };

                    constexpr Size LargeBlock = MaxValue<Size>();
                    constexpr Size MinimumAlignment = 16;
                    constexpr Size ChunkByteCount = 256 * 1024;
                    constexpr Size ClassCount = 14;
                    constexpr Size CacheBatch = 32;
                    constexpr Size ClassSizes[ClassCount] = {16, 32, 48, 64, 96, 128, 192, 256, 384, 512, 768, 1024, 1536, 2048};

                    KernelSync::Mutex PoolMutex;
                    FreeBlock* FreeLists[ClassCount] = {};
                    UInt8* ChunkCursor = nullptr;
                    Size ChunkRemaining = 0;

                    // Счётчики разнесены по осколкам (по одному на cache line), чтобы потоки не дёргали одну и ту же строку кэша.
                    // Поток привязан к своему осколку; освобождение может идти в другом осколке, поэтому отдельный осколок
                    // может «уходить в минус» (беззнаковое переполнение), а сумма всех осколков остаётся точной.
                    constexpr Size ShardCount = 64;

                    struct alignas(64) CounterShard
                    {
                        KernelSync::Atomic<UInt64> LiveBytes;
                        KernelSync::Atomic<UInt64> LiveBlocks;
                        KernelSync::Atomic<UInt64> TotalAllocations;
                    };

                    CounterShard CounterShards[ShardCount];
                    KernelSync::Atomic<UInt32> NextShard;

                    CounterShard& GetShard() noexcept
                    {
                        static thread_local CounterShard* Shard = nullptr;

                        if (Shard == nullptr)
                            Shard = &CounterShards[NextShard.FetchAdd(1, KernelSync::MemoryOrder::Relaxed) % ShardCount];

                        return *Shard;
                    }

                    UInt64 SumShards(KernelSync::Atomic<UInt64> CounterShard::*Member) noexcept
                    {
                        UInt64 Total = 0;

                        for (Size Index = 0; Index < ShardCount; Index++)
                            Total += (CounterShards[Index].*Member).Load(KernelSync::MemoryOrder::Relaxed);

                        return Total;
                    }

                    Bool IsValidAlignment(Size Alignment) noexcept
                    {
                        return Alignment != 0 && (Alignment & (Alignment - 1)) == 0;
                    }

                    BlockHeader* GetHeader(void* MemoryBlock) noexcept
                    {
                        return reinterpret_cast<BlockHeader*>(reinterpret_cast<UIntPtr>(MemoryBlock) - sizeof(BlockHeader));
                    }

                    const BlockHeader* GetHeader(const void* MemoryBlock) noexcept
                    {
                        return reinterpret_cast<const BlockHeader*>(reinterpret_cast<UIntPtr>(MemoryBlock) - sizeof(BlockHeader));
                    }

                    Size FindClassIndex(Size ByteCount) noexcept
                    {
                        for (Size Index = 0; Index < ClassCount; Index++)
                        {
                            if (ByteCount <= ClassSizes[Index])
                                return Index;
                        }

                        return LargeBlock;
                    }

                    // Берёт до Wanted блоков из общего пула одним захватом мьютекса и возвращает их связным списком.
                    FreeBlock* TakeBatchFromPool(Size ClassIndex, Size Wanted) noexcept
                    {
                        KernelSync::LockGuard Lock(PoolMutex);

                        FreeBlock* Head = nullptr;
                        const Size BlockSize = sizeof(BlockHeader) + ClassSizes[ClassIndex];

                        for (Size Taken = 0; Taken < Wanted; Taken++)
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

                                    ChunkCursor = Chunk;
                                    ChunkRemaining = ChunkByteCount;
                                }

                                Block = reinterpret_cast<FreeBlock*>(ChunkCursor);
                                ChunkCursor += BlockSize;
                                ChunkRemaining -= BlockSize;
                            }

                            Block->Next = Head;
                            Head = Block;
                        }

                        return Head;
                    }

                    // Возвращает связный список блоков в общий пул одним захватом мьютекса.
                    void GiveBatchToPool(Size ClassIndex, FreeBlock* Head) noexcept
                    {
                        if (Head == nullptr)
                            return;

                        FreeBlock* Tail = Head;

                        while (Tail->Next != nullptr)
                            Tail = Tail->Next;

                        KernelSync::LockGuard Lock(PoolMutex);

                        Tail->Next = FreeLists[ClassIndex];
                        FreeLists[ClassIndex] = Head;
                    }

                    // Кэш блоков потока: обычный путь выделения и освобождения не берёт ни одного мьютекса.
                    // При опустошении берётся пачка из общего пула, при переполнении лишнее возвращается пачкой.
                    // При завершении потока всё содержимое возвращается в общий пул.
                    struct ThreadCache
                    {
                        FreeBlock* Heads[ClassCount] = {};
                        Size Counts[ClassCount] = {};

                        ~ThreadCache()
                        {
                            for (Size Index = 0; Index < ClassCount; Index++)
                            {
                                GiveBatchToPool(Index, Heads[Index]);
                                Heads[Index] = nullptr;
                                Counts[Index] = 0;
                            }

                            CacheDestroyed = true;
                        }

                        static inline thread_local Bool CacheDestroyed = false;
                    };

                    ThreadCache& GetThreadCache() noexcept
                    {
                        static thread_local ThreadCache Cache;
                        return Cache;
                    }

                    UInt8* TakeSmallBlock(Size ClassIndex) noexcept
                    {
                        if (ThreadCache::CacheDestroyed)
                        {
                            FreeBlock* Single = TakeBatchFromPool(ClassIndex, 1);
                            return reinterpret_cast<UInt8*>(Single);
                        }

                        ThreadCache& Cache = GetThreadCache();

                        if (Cache.Heads[ClassIndex] == nullptr)
                        {
                            Cache.Heads[ClassIndex] = TakeBatchFromPool(ClassIndex, CacheBatch);

                            Size Count = 0;

                            for (FreeBlock* Block = Cache.Heads[ClassIndex]; Block != nullptr; Block = Block->Next)
                                Count++;

                            Cache.Counts[ClassIndex] = Count;

                            if (Count == 0)
                                return nullptr;
                        }

                        FreeBlock* Block = Cache.Heads[ClassIndex];
                        Cache.Heads[ClassIndex] = Block->Next;
                        Cache.Counts[ClassIndex]--;
                        return reinterpret_cast<UInt8*>(Block);
                    }

                    void GiveSmallBlock(UInt8* Block, Size ClassIndex) noexcept
                    {
                        FreeBlock* Free = reinterpret_cast<FreeBlock*>(Block);

                        if (ThreadCache::CacheDestroyed)
                        {
                            Free->Next = nullptr;
                            GiveBatchToPool(ClassIndex, Free);
                            return;
                        }

                        ThreadCache& Cache = GetThreadCache();

                        Free->Next = Cache.Heads[ClassIndex];
                        Cache.Heads[ClassIndex] = Free;
                        Cache.Counts[ClassIndex]++;

                        if (Cache.Counts[ClassIndex] >= CacheBatch * 2)
                        {
                            FreeBlock* Keep = Cache.Heads[ClassIndex];
                            FreeBlock* Last = Keep;

                            for (Size Index = 1; Index < CacheBatch; Index++)
                                Last = Last->Next;

                            FreeBlock* Surplus = Last->Next;
                            Last->Next = nullptr;
                            Cache.Counts[ClassIndex] = CacheBatch;

                            GiveBatchToPool(ClassIndex, Surplus);
                        }
                    }

                    void* AllocateSmall(Size ByteCount, Size ClassIndex)
                    {
                        UInt8* Block = TakeSmallBlock(ClassIndex);

                        if (Block == nullptr)
                            throw KernelAssert::AllocationError("Out of memory");

                        BlockHeader* Header = reinterpret_cast<BlockHeader*>(Block);
                        Header->RawBase = nullptr;
                        Header->RawSize = 0;
                        Header->ByteCount = ByteCount;
                        Header->Alignment = MinimumAlignment;
                        Header->ClassIndex = ClassIndex;
                        Header->Padding = 0;

                        return Block + sizeof(BlockHeader);
                    }

                    void* AllocateLarge(Size ByteCount, Size Alignment)
                    {
                        const Size Overhead = Alignment + sizeof(BlockHeader);
                        const Size PageSize = KernelSystem::GetPageSize();

                        if (ByteCount > MaxValue<Size>() - Overhead - PageSize)
                            throw KernelAssert::AllocationError("Allocation size is too large");

                        const Size RawSize = (ByteCount + Overhead + PageSize - 1) / PageSize * PageSize;
                        void* RawBase = KernelSystem::ReservePages(RawSize);

                        if (RawBase == nullptr)
                            throw KernelAssert::AllocationError("Out of memory");

                        const UIntPtr Start = reinterpret_cast<UIntPtr>(RawBase) + sizeof(BlockHeader);
                        const UIntPtr Aligned = (Start + Alignment - 1) & ~(static_cast<UIntPtr>(Alignment) - 1);
                        void* MemoryBlock = reinterpret_cast<void*>(Aligned);

                        BlockHeader* Header = GetHeader(MemoryBlock);
                        Header->RawBase = RawBase;
                        Header->RawSize = RawSize;
                        Header->ByteCount = ByteCount;
                        Header->Alignment = Alignment;
                        Header->ClassIndex = LargeBlock;
                        Header->Padding = 0;

                        return MemoryBlock;
                    }
                }

                void* Allocate(Size ByteCount, Size Alignment)
                {
                    if (!IsValidAlignment(Alignment))
                        throw KernelAssert::AllocationError("Alignment must be a non-zero power of two");

                    if (Alignment < MinimumAlignment)
                        Alignment = MinimumAlignment;

                    if (ByteCount == 0)
                        ByteCount = 1;

                    const Size ClassIndex = Alignment == MinimumAlignment ? FindClassIndex(ByteCount) : LargeBlock;

                    void* MemoryBlock = ClassIndex == LargeBlock
                        ? AllocateLarge(ByteCount, Alignment)
                        : AllocateSmall(ByteCount, ClassIndex);

                    CounterShard& Shard = GetShard();
                    Shard.LiveBytes.FetchAdd(ByteCount, KernelSync::MemoryOrder::Relaxed);
                    Shard.LiveBlocks.FetchAdd(1, KernelSync::MemoryOrder::Relaxed);
                    Shard.TotalAllocations.FetchAdd(1, KernelSync::MemoryOrder::Relaxed);

                    return MemoryBlock;
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

                    const BlockHeader* OldHeader = GetHeader(static_cast<const void*>(MemoryBlock));
                    const Size OldByteCount = OldHeader->ByteCount;

                    void* NewBlock = Allocate(NewByteCount, OldHeader->Alignment);
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
                    void* RawBase = Header->RawBase;
                    const Size RawSize = Header->RawSize;

                    CounterShard& Shard = GetShard();
                    Shard.LiveBytes.FetchSub(ByteCount, KernelSync::MemoryOrder::Relaxed);
                    Shard.LiveBlocks.FetchSub(1, KernelSync::MemoryOrder::Relaxed);

                    if (ClassIndex == LargeBlock)
                        KernelSystem::ReleasePages(RawBase, RawSize);
                    else
                        GiveSmallBlock(reinterpret_cast<UInt8*>(Header), ClassIndex);
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

                UInt64 GetLiveByteCount() noexcept
                {
                    return SumShards(&CounterShard::LiveBytes);
                }

                UInt64 GetLiveBlockCount() noexcept
                {
                    return SumShards(&CounterShard::LiveBlocks);
                }

                UInt64 GetTotalAllocationCount() noexcept
                {
                    return SumShards(&CounterShard::TotalAllocations);
                }
            }
        }
    }
}
