#include "Empire.Kernel/Kernel.Memory/Memory.hpp"

#include "Empire.Kernel/Kernel.Assert/Assert.hpp"
#include "Empire.Kernel/Kernel.Sync/Sync.Atomic.hpp"
#include "Empire.Kernel/Kernel.Types/Types.Utility.hpp"

#include <cstdlib>

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        using namespace EngineKernel::Types;

        namespace EngineKernel
        {
            namespace Memory
            {
                namespace
                {
                    struct BlockHeader
                    {
                        void* RawBlock;
                        Size ByteCount;
                        Size Alignment;
                    };

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
                }

                void* Allocate(Size ByteCount, Size Alignment)
                {
                    if (!IsValidAlignment(Alignment))
                        throw Assert::AllocationError("Alignment must be a non-zero power of two");

                    if (Alignment < alignof(BlockHeader))
                        Alignment = alignof(BlockHeader);

                    if (ByteCount == 0)
                        ByteCount = 1;

                    const Size Overhead = Alignment + sizeof(BlockHeader);

                    if (ByteCount > MaxValue<Size>() - Overhead)
                        throw Assert::AllocationError("Allocation size is too large");

                    void* RawBlock = std::malloc(ByteCount + Overhead);

                    if (RawBlock == nullptr)
                        throw Assert::AllocationError("Out of memory");

                    const UIntPtr Start = reinterpret_cast<UIntPtr>(RawBlock) + sizeof(BlockHeader);
                    const UIntPtr Aligned = (Start + Alignment - 1) & ~(static_cast<UIntPtr>(Alignment) - 1);
                    void* MemoryBlock = reinterpret_cast<void*>(Aligned);

                    BlockHeader* Header = GetHeader(MemoryBlock);
                    Header->RawBlock = RawBlock;
                    Header->ByteCount = ByteCount;
                    Header->Alignment = Alignment;

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
                    void* RawBlock = Header->RawBlock;

                    LiveBytes.FetchSub(Header->ByteCount, Sync::MemoryOrder::Relaxed);
                    LiveBlocks.FetchSub(1, Sync::MemoryOrder::Relaxed);

                    std::free(RawBlock);
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
