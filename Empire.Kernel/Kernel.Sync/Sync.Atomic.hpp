#pragma once

#include "Empire.Kernel/Kernel.Types/Types.Utility.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EngineKernel
        {
            namespace Sync
            {
                // Значения совпадают с порядками памяти, которые понимают встроенные атомарные операции GCC и Clang.
                enum class MemoryOrder : Types::Int32
                {
                    Relaxed = 0,
                    Acquire = 2,
                    Release = 3,
                    AcquireRelease = 4,
                    SequentiallyConsistent = 5
                };

                template <typename T>
                class Atomic
                {
                    private:
                        mutable T Value;

                        static constexpr Types::Int32 Raw(MemoryOrder Order) noexcept
                        {
                            return static_cast<Types::Int32>(Order);
                        }

                    public:
                        constexpr Atomic() noexcept : Value{} {}
                        constexpr explicit Atomic(T Initial) noexcept : Value(Initial) {}

                        Atomic(const Atomic&) = delete;
                        Atomic& operator=(const Atomic&) = delete;

                        [[nodiscard]] T Load(MemoryOrder Order = MemoryOrder::SequentiallyConsistent) const noexcept
                        {
                            return __atomic_load_n(&Value, Raw(Order));
                        }

                        void Store(T NewValue, MemoryOrder Order = MemoryOrder::SequentiallyConsistent) noexcept
                        {
                            __atomic_store_n(&Value, NewValue, Raw(Order));
                        }

                        T Exchange(T NewValue, MemoryOrder Order = MemoryOrder::SequentiallyConsistent) noexcept
                        {
                            return __atomic_exchange_n(&Value, NewValue, Raw(Order));
                        }

                        Types::Bool CompareExchange(
                            T& Expected,
                            T Desired,
                            MemoryOrder Success = MemoryOrder::SequentiallyConsistent,
                            MemoryOrder Failure = MemoryOrder::Relaxed
                        ) noexcept
                        {
                            return __atomic_compare_exchange_n(&Value, &Expected, Desired, false, Raw(Success), Raw(Failure));
                        }

                        T FetchAdd(T Amount, MemoryOrder Order = MemoryOrder::SequentiallyConsistent) noexcept
                            requires Types::Integral<T>
                        {
                            return __atomic_fetch_add(&Value, Amount, Raw(Order));
                        }

                        T FetchSub(T Amount, MemoryOrder Order = MemoryOrder::SequentiallyConsistent) noexcept
                            requires Types::Integral<T>
                        {
                            return __atomic_fetch_sub(&Value, Amount, Raw(Order));
                        }
                };
            }
        }
    }
}
