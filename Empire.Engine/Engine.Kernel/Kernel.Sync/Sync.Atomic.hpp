#pragma once

#include "Empire.Engine/Engine.Kernel/Kernel.Compiler/Compiler.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Types/Types.Utility.hpp"

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
                    // Порядки памяти и сами атомарные операции определяет Kernel.Compiler.
                    using MemoryOrder = KernelCompiler::AtomicOrder;

                    // Метка конструктора Atomic, который не записывает начальное значение. Нужен там, где объект строится заново поверх
                    // памяти, которую другие потоки ещё могут читать атомарно (например узел очереди в пуле фиксированного размера):
                    // обычная запись начального значения для ThreadSanitizer это гонка с такими чтениями.
                    // После такого конструктора значение нужно сразу записать через Store.
                    struct NoInitialize
                    {
                    };

                    template <typename T>
                    class Atomic
                    {
                        private:
                            mutable T Value;

                        public:
                            constexpr Atomic() noexcept : Value{} {}
                            constexpr explicit Atomic(T Initial) noexcept : Value(Initial) {}

                            Atomic(NoInitialize) noexcept {}

                            Atomic(const Atomic&) = delete;
                            Atomic& operator=(const Atomic&) = delete;

                            [[nodiscard]] T Load(MemoryOrder Order = MemoryOrder::SequentiallyConsistent) const noexcept
                            {
                                return KernelCompiler::AtomicLoad(&Value, Order);
                            }

                            void Store(T NewValue, MemoryOrder Order = MemoryOrder::SequentiallyConsistent) noexcept
                            {
                                KernelCompiler::AtomicStore(&Value, NewValue, Order);
                            }

                            T Exchange(T NewValue, MemoryOrder Order = MemoryOrder::SequentiallyConsistent) noexcept
                            {
                                return KernelCompiler::AtomicExchange(&Value, NewValue, Order);
                            }

                            KernelTypes::Bool CompareExchange(
                                T& Expected,
                                T Desired,
                                MemoryOrder Success = MemoryOrder::SequentiallyConsistent,
                                MemoryOrder Failure = MemoryOrder::Relaxed
                            ) noexcept
                            {
                                return KernelCompiler::AtomicCompareExchange(&Value, &Expected, Desired, Success, Failure);
                            }

                            T FetchAdd(T Amount, MemoryOrder Order = MemoryOrder::SequentiallyConsistent) noexcept
                                requires KernelTypes::Integral<T>
                            {
                                return KernelCompiler::AtomicFetchAdd(&Value, Amount, Order);
                            }

                            T FetchSub(T Amount, MemoryOrder Order = MemoryOrder::SequentiallyConsistent) noexcept
                                requires KernelTypes::Integral<T>
                            {
                                return KernelCompiler::AtomicFetchSub(&Value, Amount, Order);
                            }
                    };
                }
            }
        }
    }
}
