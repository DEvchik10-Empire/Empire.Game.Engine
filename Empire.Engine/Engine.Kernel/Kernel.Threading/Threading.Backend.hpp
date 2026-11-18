#pragma once

#include "Empire.Engine/Engine.Kernel/Kernel.Types/Types.hpp"

// Внутренний интерфейс между переносимой частью Kernel.Threading и бэкендами платформ.
// Каждая платформа (Threading.Linux, Threading.Windows, Threading.MacOS) реализует эти функции. Наружу модуля не виден.
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
                    namespace ThreadingBackend
                    {
                        using ThreadEntry = void (*)(void* Context);

                        // Число аппаратных потоков (не меньше 1)
                        [[nodiscard]] KernelTypes::UInt32 GetHardwareThreadCount() noexcept;

                        // Запустить поток. Имя (может быть nullptr) задаётся из самого потока. Возвращает false, если поток не создан.
                        [[nodiscard]] KernelTypes::Bool StartThread(ThreadEntry Entry, void* Context, const KernelTypes::Char* Name, KernelTypes::UInt64& OutHandle) noexcept;

                        // Дождаться завершения потока и освободить его ресурсы
                        void JoinThread(KernelTypes::UInt64 Handle) noexcept;

                        // Семафор ОС. Создаётся с нулевым счётчиком, возвращает nullptr при ошибке.
                        [[nodiscard]] void* CreateSemaphore() noexcept;
                        void DestroySemaphore(void* Semaphore) noexcept;

                        // Увеличить счётчик на Count (Count > 0), разбудив ожидающих
                        void SignalSemaphore(void* Semaphore, KernelTypes::UInt32 Count) noexcept;

                        // Уменьшить счётчик на 1, ожидая при нуле
                        void WaitSemaphore(void* Semaphore) noexcept;
                    }
                }
            }
        }
    }
}
