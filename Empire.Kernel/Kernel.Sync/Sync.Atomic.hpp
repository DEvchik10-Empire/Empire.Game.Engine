#pragma once

#include "Empire.Kernel/Kernel.Types/Types.Utility.hpp"

#if !defined(__GNUC__) && !defined(__clang__)
    #error "Sync.Atomic requires GCC or Clang atomic builtins"
#endif

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EngineKernel
        {
            namespace Sync
            {
                enum class MemoryOrder : Types::Int32
                {
                    Relaxed = __ATOMIC_RELAXED,
                    Acquire = __ATOMIC_ACQUIRE,
                    Release = __ATOMIC_RELEASE,
                    AcquireRelease = __ATOMIC_ACQ_REL,
                    SequentiallyConsistent = __ATOMIC_SEQ_CST
                };

                template <typename T>
                class Atomic
                {
                    private:
                        T Value;

                    public:
                        constexpr Atomic() noexcept : Value{} {}
                        constexpr explicit Atomic(T Initial) noexcept : Value(Initial) {}

                        Atomic(const Atomic&) = delete;
                        Atomic& operator=(const Atomic&) = delete;

                        T Load(MemoryOrder Order = MemoryOrder::SequentiallyConsistent) const noexcept
                        {
                            return __atomic_load_n(&Value, static_cast<Types::Int32>(Order));
                        }

                        void Store(T NewValue, MemoryOrder Order = MemoryOrder::SequentiallyConsistent) noexcept
                        {
                            __atomic_store_n(&Value, NewValue, static_cast<Types::Int32>(Order));
                        }

                        T Exchange(T NewValue, MemoryOrder Order = MemoryOrder::SequentiallyConsistent) noexcept
                        {
                            return __atomic_exchange_n(&Value, NewValue, static_cast<Types::Int32>(Order));
                        }

                        Types::Bool CompareExchange(
                            T& Expected,
                            T Desired,
                            MemoryOrder Success = MemoryOrder::SequentiallyConsistent,
                            MemoryOrder Failure = MemoryOrder::Relaxed
                        ) noexcept
                        {
                            return __atomic_compare_exchange_n(
                                &Value,
                                &Expected,
                                Desired,
                                false,
                                static_cast<Types::Int32>(Success),
                                static_cast<Types::Int32>(Failure)
                            );
                        }

                        T FetchAdd(T Amount, MemoryOrder Order = MemoryOrder::SequentiallyConsistent) noexcept
                            requires Types::Integral<T>
                        {
                            return __atomic_fetch_add(&Value, Amount, static_cast<Types::Int32>(Order));
                        }

                        T FetchSub(T Amount, MemoryOrder Order = MemoryOrder::SequentiallyConsistent) noexcept
                            requires Types::Integral<T>
                        {
                            return __atomic_fetch_sub(&Value, Amount, static_cast<Types::Int32>(Order));
                        }
                };
            }
        }
    }
}
