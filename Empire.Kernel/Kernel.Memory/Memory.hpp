#pragma once

#include "Empire.Kernel/Kernel.Types/Types.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EngineKernel
        {
            namespace Memory
            {
                inline constexpr Types::Size DefaultAlignment = 16;

                // Блок хранит перед собой заголовок (исходный указатель, размер, выравнивание),
                // поэтому Deallocate и Reallocate не требуют размер и выравнивание от вызывающего.
                void* Allocate(Types::Size ByteCount, Types::Size Alignment = DefaultAlignment);
                void* Reallocate(void* MemoryBlock, Types::Size NewByteCount);
                void Deallocate(void* MemoryBlock) noexcept;

                Types::Size GetBlockByteCount(const void* MemoryBlock) noexcept;

                void CopyBytes(void* Destination, const void* Source, Types::Size ByteCount) noexcept;
                void MoveBytes(void* Destination, const void* Source, Types::Size ByteCount) noexcept;
                void FillBytes(void* Destination, Types::UInt8 Value, Types::Size ByteCount) noexcept;
                Types::Int32 CompareBytes(const void* Left, const void* Right, Types::Size ByteCount) noexcept;

                Types::UInt64 GetLiveByteCount() noexcept;
                Types::UInt64 GetLiveBlockCount() noexcept;
                Types::UInt64 GetTotalAllocationCount() noexcept;
            }
        }
    }
}
