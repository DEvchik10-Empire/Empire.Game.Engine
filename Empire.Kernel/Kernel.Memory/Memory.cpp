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
        using namespace EngineKernel::Types;

        namespace EngineKernel
        {
            namespace Memory
            {
                // Аллокатор поверх страниц ОС (System::ReservePages).
                // Малые блоки (до 2 КиБ, выравнивание до 16) берутся из классов размеров со списками свободных блоков,
                // куски по 256 КиБ нарезаются последовательно и в ОС не возвращаются.
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
                    constexpr Size ClassSizes[ClassCount] = {16, 32, 48, 64, 96, 128, 192, 256, 384, 512, 768, 1024, 1536, 2048};

                    Sync::Mutex PoolMutex;
                    FreeBlock* FreeLists[ClassCount] = {};
                    UInt8* ChunkCursor = nullptr;
                    Size ChunkRemaining = 0;

                    Sync::Atomic<UInt64> LiveBytes;
                    Sync::Atomic<UInt64> LiveBlocks;
                    Sync::Atomic<UInt64> TotalAllocations;

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

                    UInt8* TakeSmallBlock(Size ClassIndex) noexcept
                    {
                        Sync::LockGuard Lock(PoolMutex);

                        if (FreeBlock* Block = FreeLists[ClassIndex])
                        {
                            FreeLists[ClassIndex] = Block->Next;
                            return reinterpret_cast<UInt8*>(Block);
                        }

                        const Size BlockSize = sizeof(BlockHeader) + ClassSizes[ClassIndex];

                        if (ChunkRemaining < BlockSize)
                        {
                            UInt8* Chunk = static_cast<UInt8*>(System::ReservePages(ChunkByteCount));

                            if (Chunk == nullptr)
                                return nullptr;

                            ChunkCursor = Chunk;
                            ChunkRemaining = ChunkByteCount;
                        }

                        UInt8* Block = ChunkCursor;
                        ChunkCursor += BlockSize;
                        ChunkRemaining -= BlockSize;
                        return Block;
                    }

                    void GiveSmallBlock(UInt8* Block, Size ClassIndex) noexcept
                    {
                        Sync::LockGuard Lock(PoolMutex);

                        FreeBlock* Free = reinterpret_cast<FreeBlock*>(Block);
                        Free->Next = FreeLists[ClassIndex];
                        FreeLists[ClassIndex] = Free;
                    }

                    void* AllocateSmall(Size ByteCount, Size ClassIndex)
                    {
                        UInt8* Block = TakeSmallBlock(ClassIndex);

                        if (Block == nullptr)
                            throw Assert::AllocationError("Out of memory");

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
                        const Size PageSize = System::GetPageSize();

                        if (ByteCount > MaxValue<Size>() - Overhead - PageSize)
                            throw Assert::AllocationError("Allocation size is too large");

                        const Size RawSize = (ByteCount + Overhead + PageSize - 1) / PageSize * PageSize;
                        void* RawBase = System::ReservePages(RawSize);

                        if (RawBase == nullptr)
                            throw Assert::AllocationError("Out of memory");

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
                        throw Assert::AllocationError("Alignment must be a non-zero power of two");

                    if (Alignment < MinimumAlignment)
                        Alignment = MinimumAlignment;

                    if (ByteCount == 0)
                        ByteCount = 1;

                    const Size ClassIndex = Alignment == MinimumAlignment ? FindClassIndex(ByteCount) : LargeBlock;

                    void* MemoryBlock = ClassIndex == LargeBlock
                        ? AllocateLarge(ByteCount, Alignment)
                        : AllocateSmall(ByteCount, ClassIndex);

                    LiveBytes.FetchAdd(ByteCount, Sync::MemoryOrder::Relaxed);
                    LiveBlocks.FetchAdd(1, Sync::MemoryOrder::Relaxed);
                    TotalAllocations.FetchAdd(1, Sync::MemoryOrder::Relaxed);

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

                    LiveBytes.FetchSub(ByteCount, Sync::MemoryOrder::Relaxed);
                    LiveBlocks.FetchSub(1, Sync::MemoryOrder::Relaxed);

                    if (ClassIndex == LargeBlock)
                        System::ReleasePages(RawBase, RawSize);
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
                    return LiveBytes.Load(Sync::MemoryOrder::Relaxed);
                }

                UInt64 GetLiveBlockCount() noexcept
                {
                    return LiveBlocks.Load(Sync::MemoryOrder::Relaxed);
                }

                UInt64 GetTotalAllocationCount() noexcept
                {
                    return TotalAllocations.Load(Sync::MemoryOrder::Relaxed);
                }
            }
        }
    }
}
