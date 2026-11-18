#pragma once

#include "Empire.Engine/Engine.Kernel/Kernel.Types/Types.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireEngine
        {
            namespace EngineKernel
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

                    // Возвращает ОС куски памяти малых блоков, все блоки которых свободны. Вызывать в удобный момент
                    // (между уровнями, на паузе): обходит весь пул свободных блоков. Возвращает число освобождённых байт.
                    // Блоки в кэшах других потоков не трогаются. Автоматически то же происходит при простое пула больше 256 МиБ.
                    KernelTypes::Size Trim() noexcept;

                    // Сколько байт держит аллокатор у ОС под малые блоки (без больших блоков, которые сразу возвращаются).
                    KernelTypes::Size GetReservedByteCount() noexcept;

                    KernelTypes::UInt64 GetLiveByteCount() noexcept;
                    KernelTypes::UInt64 GetLiveBlockCount() noexcept;
                    KernelTypes::UInt64 GetTotalAllocationCount() noexcept;
                }
            }
        }
    }
}
