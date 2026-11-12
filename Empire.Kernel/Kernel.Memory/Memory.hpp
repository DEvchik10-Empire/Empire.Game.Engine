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
                void* Allocate(Types::Size ByteCount, Types::Size Alignment = alignof(std::max_align_t));
                void* Reallocate(
                    void* MemoryBlock,
                    Types::Size OldByteCount,
                    Types::Size NewByteCount,
                    Types::Size Alignment = alignof(std::max_align_t)
                );
                void Deallocate(void* MemoryBlock, Types::Size Alignment = alignof(std::max_align_t)) noexcept;
            }
        }
    }
}
