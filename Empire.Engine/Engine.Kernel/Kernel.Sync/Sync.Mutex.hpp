#pragma once

#include "Empire.Engine/Engine.Kernel/Kernel.Assert/Assert.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Sync/Sync.Atomic.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.System/System.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireEngine
        {
            namespace EngineKernel
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

                            // Занят ли мьютекс (в Jolt is_locked). Результат моментальный снимок, для проверок и отладки.
                            [[nodiscard]] KernelTypes::Bool IsLocked() const noexcept
                            {
                                return State.Load(MemoryOrder::Relaxed) != 0;
                            }
                    };

                    // Reader/writer lock (перенесён из Jolt 5.7.0, Core/Mutex.h).
                    // Счётчик неотрицателен при читателях и равен -1 при эксклюзивном владельце.
                    class SharedMutex
                    {
                        private:
                            Atomic<KernelTypes::Int32> State{0};

                        public:
                            void Lock() noexcept
                            {
                                KernelTypes::UInt32 Attempts = 0;
                                for (;;)
                                {
                                    KernelTypes::Int32 Expected = 0;
                                    if (State.CompareExchange(Expected, -1, MemoryOrder::Acquire))
                                        return;

                                    if (Attempts++ < 64)
                                        KernelCompiler::Pause();
                                    else
                                        KernelSystem::YieldThread();
                                }
                            }

                            [[nodiscard]] KernelTypes::Bool TryLock() noexcept
                            {
                                KernelTypes::Int32 Expected = 0;
                                return State.CompareExchange(Expected, -1, MemoryOrder::Acquire);
                            }

                            void Unlock() noexcept
                            {
                                State.Store(0, MemoryOrder::Release);
                            }

                            void LockShared() noexcept
                            {
                                KernelTypes::UInt32 Attempts = 0;
                                for (;;)
                                {
                                    KernelTypes::Int32 Current = State.Load(MemoryOrder::Relaxed);
                                    if (Current >= 0 && Current < KernelTypes::MaxValue<KernelTypes::Int32>())
                                    {
                                        if (State.CompareExchange(Current, Current + 1, MemoryOrder::Acquire))
                                            return;
                                    }

                                    if (Attempts++ < 64)
                                        KernelCompiler::Pause();
                                    else
                                        KernelSystem::YieldThread();
                                }
                            }

                            [[nodiscard]] KernelTypes::Bool TryLockShared() noexcept
                            {
                                KernelTypes::Int32 Current = State.Load(MemoryOrder::Relaxed);
                                while (Current >= 0 && Current < KernelTypes::MaxValue<KernelTypes::Int32>())
                                {
                                    if (State.CompareExchange(Current, Current + 1, MemoryOrder::Acquire))
                                        return true;
                                }

                                return false;
                            }

                            void UnlockShared() noexcept
                            {
                                State.FetchSub(1, MemoryOrder::Release);
                            }

                            // Держит ли кто-нибудь блокировку (эксклюзивную или общую). Моментальный снимок для проверок и отладки.
                            [[nodiscard]] KernelTypes::Bool IsLocked() const noexcept
                            {
                                return State.Load(MemoryOrder::Relaxed) != 0;
                            }
                    };

                    struct DeferLockTag { };
                    struct TryToLockTag { };
                    inline constexpr DeferLockTag DeferLock{};
                    inline constexpr TryToLockTag TryToLock{};

                    template <typename MutexType>
                    class UniqueLock
                    {
                        private:
                            MutexType* Target = nullptr;
                            KernelTypes::Bool Locked = false;

                            void Reset() noexcept
                            {
                                if (Target != nullptr && Locked)
                                    Target->Unlock();
                                Target = nullptr;
                                Locked = false;
                            }

                        public:
                            explicit UniqueLock(MutexType& Value) noexcept : Target(&Value), Locked(true) { Target->Lock(); }
                            UniqueLock(MutexType& Value, DeferLockTag) noexcept : Target(&Value) { }
                            UniqueLock(MutexType& Value, TryToLockTag) noexcept : Target(&Value), Locked(Target->TryLock()) { }
                            UniqueLock(UniqueLock&& Other) noexcept : Target(Other.Target), Locked(Other.Locked)
                            {
                                Other.Target = nullptr;
                                Other.Locked = false;
                            }
                            UniqueLock& operator=(UniqueLock&& Other) noexcept
                            {
                                if (this != &Other)
                                {
                                    Reset();
                                    Target = Other.Target;
                                    Locked = Other.Locked;
                                    Other.Target = nullptr;
                                    Other.Locked = false;
                                }
                                return *this;
                            }
                            ~UniqueLock() { Reset(); }

                            void Lock() noexcept { KernelAssert::DebugCheck(Target != nullptr && !Locked, "UniqueLock::Lock: неверное состояние"); Target->Lock(); Locked = true; }
                            [[nodiscard]] KernelTypes::Bool TryLock() noexcept { KernelAssert::DebugCheck(Target != nullptr && !Locked, "UniqueLock::TryLock: неверное состояние"); Locked = Target->TryLock(); return Locked; }
                            void Unlock() noexcept { KernelAssert::DebugCheck(Target != nullptr && Locked, "UniqueLock::Unlock: неверное состояние"); Target->Unlock(); Locked = false; }
                            [[nodiscard]] KernelTypes::Bool OwnsLock() const noexcept { return Locked; }
                            [[nodiscard]] MutexType* GetMutex() const noexcept { return Target; }

                            UniqueLock(const UniqueLock&) = delete;
                            UniqueLock& operator=(const UniqueLock&) = delete;
                    };

                    template <typename MutexType>
                    class SharedLock
                    {
                        private:
                            MutexType* Target = nullptr;
                            KernelTypes::Bool Locked = false;

                            void Reset() noexcept
                            {
                                if (Target != nullptr && Locked)
                                    Target->UnlockShared();
                                Target = nullptr;
                                Locked = false;
                            }

                        public:
                            explicit SharedLock(MutexType& Value) noexcept : Target(&Value), Locked(true) { Target->LockShared(); }
                            SharedLock(MutexType& Value, DeferLockTag) noexcept : Target(&Value) { }
                            SharedLock(MutexType& Value, TryToLockTag) noexcept : Target(&Value), Locked(Target->TryLockShared()) { }
                            SharedLock(SharedLock&& Other) noexcept : Target(Other.Target), Locked(Other.Locked)
                            {
                                Other.Target = nullptr;
                                Other.Locked = false;
                            }
                            SharedLock& operator=(SharedLock&& Other) noexcept
                            {
                                if (this != &Other)
                                {
                                    Reset();
                                    Target = Other.Target;
                                    Locked = Other.Locked;
                                    Other.Target = nullptr;
                                    Other.Locked = false;
                                }
                                return *this;
                            }
                            ~SharedLock() { Reset(); }

                            void Lock() noexcept { KernelAssert::DebugCheck(Target != nullptr && !Locked, "SharedLock::Lock: неверное состояние"); Target->LockShared(); Locked = true; }
                            [[nodiscard]] KernelTypes::Bool TryLock() noexcept { KernelAssert::DebugCheck(Target != nullptr && !Locked, "SharedLock::TryLock: неверное состояние"); Locked = Target->TryLockShared(); return Locked; }
                            void Unlock() noexcept { KernelAssert::DebugCheck(Target != nullptr && Locked, "SharedLock::Unlock: неверное состояние"); Target->UnlockShared(); Locked = false; }
                            [[nodiscard]] KernelTypes::Bool OwnsLock() const noexcept { return Locked; }
                            [[nodiscard]] MutexType* GetMutex() const noexcept { return Target; }

                            SharedLock(const SharedLock&) = delete;
                            SharedLock& operator=(const SharedLock&) = delete;
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
}
