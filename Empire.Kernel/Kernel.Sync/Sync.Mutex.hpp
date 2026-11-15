#pragma once

#include "Empire.Kernel/Kernel.Sync/Sync.Atomic.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireKernel
        {
            namespace KernelSync
            {
                // Мьютекс без зависимостей от ОС-типов в заголовке: короткий спин, затем уступка потока, затем короткий сон.
                // Конструктор constexpr, поэтому глобальные мьютексы безопасны при любом порядке статической инициализации.
                class Mutex
                {
                    private:
                        Atomic<KernelTypes::UInt32> State;

                    public:
                        constexpr Mutex() noexcept = default;

                        Mutex(const Mutex&) = delete;
                        Mutex& operator=(const Mutex&) = delete;

                        void Lock() noexcept;
                        KernelTypes::Bool TryLock() noexcept;
                        void Unlock() noexcept;
                };

                template <typename MutexType>
                class LockGuard
                {
                    private:
                        MutexType& Target;

                    public:
                        explicit LockGuard(MutexType& TargetMutex) noexcept
                            : Target(TargetMutex)
                        {
                            Target.Lock();
                        }

                        ~LockGuard()
                        {
                            Target.Unlock();
                        }

                        LockGuard(const LockGuard&) = delete;
                        LockGuard& operator=(const LockGuard&) = delete;
                };
            }
        }
    }
}
