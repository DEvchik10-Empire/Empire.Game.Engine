#include "Empire.Kernel/Kernel.Memory/Memory.hpp"

#include <algorithm>
#include <cstring>
#include <new>

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
                    Bool IsValidAlignment(Size Alignment)
                    {
                        return Alignment != 0 && (Alignment & (Alignment - 1)) == 0;
                    }

                    Bool UsesDefaultAlignment(Size Alignment)
                    {
                        return Alignment <= alignof(std::max_align_t);
                    }
                }

                void* Allocate(Size ByteCount, Size Alignment)
                {
                    if (!IsValidAlignment(Alignment))
                        throw std::bad_alloc();

                    if (ByteCount == 0)
                        ByteCount = 1;

                    if (UsesDefaultAlignment(Alignment))
                        return ::operator new(ByteCount);

                    return ::operator new(ByteCount, std::align_val_t(Alignment));
                }

                void* Reallocate(void* MemoryBlock, Size OldByteCount, Size NewByteCount, Size Alignment)
                {
                    if (NewByteCount == 0)
                    {
                        Deallocate(MemoryBlock, Alignment);
                        return nullptr;
                    }

                    if (MemoryBlock == nullptr)
                        return Allocate(NewByteCount, Alignment);

                    void* NewBlock = Allocate(NewByteCount, Alignment);
                    std::memcpy(NewBlock, MemoryBlock, std::min(OldByteCount, NewByteCount));
                    Deallocate(MemoryBlock, Alignment);

                    return NewBlock;
                }

                void Deallocate(void* MemoryBlock, Size Alignment) noexcept
                {
                    if (MemoryBlock == nullptr)
                        return;

                    if (UsesDefaultAlignment(Alignment))
                        ::operator delete(MemoryBlock);
                    else
                        ::operator delete(MemoryBlock, std::align_val_t(Alignment));
                }
            }
        }
    }
}
