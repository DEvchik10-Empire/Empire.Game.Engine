#pragma once

#include "Empire.Engine/Engine.Kernel/Kernel.Sync/Sync.Atomic.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Types/Types.NonCopyable.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Types/Types.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireEngine
        {
            namespace EngineKernel
            {
                namespace KernelThreading
                {
                    // Число аппаратных потоков (не меньше 1)
                    [[nodiscard]] KernelTypes::UInt32 GetHardwareThreadCount() noexcept;

                    // Поток ОС. Функция потока получает указатель на свои данные, исключений между потоками нет.
                    // Перед уничтожением поток нужно присоединить (Join), иначе это ошибка.
                    class Thread : public KernelTypes::NonCopyable
                    {
                        public:
                            using Entry = void (*)(void* Context);

                            Thread() noexcept = default;
                            ~Thread();

                            Thread(Thread&& Other) noexcept;
                            Thread& operator=(Thread&& Other) noexcept;

                            // Запустить поток. Name может быть nullptr. Возвращает false, если не удалось создать поток.
                            [[nodiscard]] KernelTypes::Bool Start(Entry EntryPoint, void* Context, const KernelTypes::Char* Name = nullptr) noexcept;

                            // Дождаться завершения потока
                            void Join() noexcept;

                            [[nodiscard]] KernelTypes::Bool IsJoinable() const noexcept
                            {
                                return Joinable;
                            }

                        private:
                            KernelTypes::UInt64 Handle = 0;
                            KernelTypes::Bool Joinable = false;
                    };

                    // Счётный семафор (перенесён из Jolt, Core/Semaphore.h). Быстрый путь на атомарном счётчике:
                    // к семафору ОС обращаемся только если есть ждущие потоки.
                    class Semaphore : public KernelTypes::NonCopyable
                    {
                        public:
                            Semaphore();
                            ~Semaphore();

                            // Увеличить счётчик на Number, разбудив ждущих
                            void Release(KernelTypes::UInt32 Number = 1);

                            // Уменьшить счётчик на Number, ожидая, пока он наберётся
                            void Acquire(KernelTypes::UInt32 Number = 1);

                            // Текущее значение (отрицательное: сколько потоков ждёт)
                            [[nodiscard]] KernelTypes::Int32 GetValue() const
                            {
                                return Count.Load(KernelSync::MemoryOrder::Relaxed);
                            }

                        private:
                            // Растёт при Release и падает при Acquire. Если отрицательное, кто-то ждёт на семафоре ОС
                            // (семафор ОС дорогой, поэтому обращаемся к нему только при ожидании).
                            alignas(KernelCompiler::CacheLineSize) KernelSync::Atomic<KernelTypes::Int32> Count{0};
                            void* OsSemaphore = nullptr;
                    };
                }
            }
        }
    }
}
