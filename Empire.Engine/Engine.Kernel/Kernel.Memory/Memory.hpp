#pragma once

#include "Empire.Engine/Engine.Kernel/Kernel.Types/Types.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Types/Types.Utility.hpp"

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

                    // Подменяемый аллокатор для интеграции Jolt/пользовательских подсистем. Все callbacks
                    // получают размер и выравнивание явно; размер/выравнивание существующего блока нужны Reallocate.
                    struct AllocatorCallbacks
                    {
                        using AllocateFunction = void* (*)(KernelTypes::Size ByteCount, KernelTypes::Size Alignment) noexcept;
                        using ReallocateFunction = void* (*)(void* MemoryBlock, KernelTypes::Size OldByteCount, KernelTypes::Size NewByteCount, KernelTypes::Size Alignment) noexcept;
                        using DeallocateFunction = void (*)(void* MemoryBlock) noexcept;
                        using GetBlockByteCountFunction = KernelTypes::Size (*)(const void* MemoryBlock) noexcept;
                        using GetBlockAlignmentFunction = KernelTypes::Size (*)(const void* MemoryBlock) noexcept;

                        AllocateFunction Allocate = nullptr;
                        ReallocateFunction Reallocate = nullptr;
                        DeallocateFunction Deallocate = nullptr;
                        GetBlockByteCountFunction GetBlockByteCount = nullptr;
                        GetBlockAlignmentFunction GetBlockAlignment = nullptr;
                    };

                    // Устанавливается до первого выделения памяти в процессе. После первого Allocate смена
                    // аллокатора запрещена, чтобы каждый блок освобождался тем же backend. Callbacks
                    // не должны вызывать KernelMemory повторно: они являются нижним уровнем аллокации.
                    [[nodiscard]] KernelTypes::Bool SetAllocatorCallbacks(const AllocatorCallbacks& Callbacks) noexcept;

                    // Блок хранит перед собой заголовок (исходный указатель, размер, выравнивание),
                    // поэтому Deallocate и Reallocate не требуют размер и выравнивание от вызывающего.
                    void* Allocate(KernelTypes::Size ByteCount, KernelTypes::Size Alignment = DefaultAlignment);
                    void* Reallocate(void* MemoryBlock, KernelTypes::Size NewByteCount);
                    void Deallocate(void* MemoryBlock) noexcept;

                    // Явная замена глобальным new/delete для объектов, принадлежащих Kernel и Jolt-порту.
                    // Ошибка выделения обрабатывается fail-fast внутри Allocate; исключения не используются.
                    template <typename T, typename... Arguments>
                    [[nodiscard]] T* Make(Arguments&&... Args)
                    {
                        void* Storage = Allocate(sizeof(T), alignof(T));
                        return KernelTypes::ConstructAt(static_cast<T*>(Storage), KernelTypes::Forward<Arguments>(Args)...);
                    }

                    template <typename T>
                    void Destroy(T* Object) noexcept
                    {
                        if (Object == nullptr)
                            return;
                        KernelTypes::DestroyAt(Object);
                        Deallocate(Object);
                    }

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
