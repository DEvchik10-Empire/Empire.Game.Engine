#pragma once

#include "Empire.Kernel/Kernel.Types/Types.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireKernel
        {
            namespace KernelMemory
            {
                inline constexpr KernelTypes::Size DefaultAlignment = 16;

                // Блок хранит перед собой заголовок (исходный указатель, размер, выравнивание),
                // поэтому Deallocate и Reallocate не требуют размер и выравнивание от вызывающего.
                void* Allocate(KernelTypes::Size ByteCount, KernelTypes::Size Alignment = DefaultAlignment);
                void* Reallocate(void* MemoryBlock, KernelTypes::Size NewByteCount);
                void Deallocate(void* MemoryBlock) noexcept;

                KernelTypes::Size GetBlockByteCount(const void* MemoryBlock) noexcept;

                void CopyBytes(void* Destination, const void* Source, KernelTypes::Size ByteCount) noexcept;
                void MoveBytes(void* Destination, const void* Source, KernelTypes::Size ByteCount) noexcept;
                void FillBytes(void* Destination, KernelTypes::UInt8 Value, KernelTypes::Size ByteCount) noexcept;
                KernelTypes::Int32 CompareBytes(const void* Left, const void* Right, KernelTypes::Size ByteCount) noexcept;

                KernelTypes::UInt64 GetLiveByteCount() noexcept;
                KernelTypes::UInt64 GetLiveBlockCount() noexcept;
                KernelTypes::UInt64 GetTotalAllocationCount() noexcept;
            }
        }
    }
}
