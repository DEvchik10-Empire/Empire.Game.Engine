#pragma once

#include "Empire.Kernel/Kernel.Assert/Assert.hpp"
#include "Empire.Kernel/Kernel.Memory/Memory.SharedPtr.hpp"
#include "Empire.Kernel/Kernel.Types/Types.Utility.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireKernel
        {
            namespace KernelContainers
            {
                template <typename Signature>
                class Function;

                // Копии разделяют одно и то же вызываемое значение (счётчик ссылок), копирование не бросает исключений.
                template <typename Result, typename... Arguments>
                class Function<Result(Arguments...)>
                {
                    private:
                        class Invoker
                        {
                            public:
                                virtual ~Invoker() = default;
                                virtual Result Invoke(Arguments... Args) = 0;
                        };

                        template <typename Callable>
                        class Holder final : public Invoker
                        {
                            private:
                                Callable Target;

                            public:
                                template <typename Source>
                                explicit Holder(Source&& Value)
                                    : Target(KernelTypes::Forward<Source>(Value))
                                {
                                }

                                Result Invoke(Arguments... Args) override
                                {
                                    return Target(KernelTypes::Forward<Arguments>(Args)...);
                                }
                        };

                        KernelMemory::SharedPtr<Invoker> Target;

                    public:
                        Function() = default;
                        Function(KernelTypes::NullPointer) noexcept {}

                        template <typename Callable>
                            requires (!KernelTypes::IsSame<KernelTypes::RemoveCVReference<Callable>, Function>)
                        Function(Callable&& Source)
                            : Target(KernelMemory::MakeShared<Holder<KernelTypes::RemoveCVReference<Callable>>>(KernelTypes::Forward<Callable>(Source)))
                        {
                        }

                        explicit operator KernelTypes::Bool() const noexcept
                        {
                            return static_cast<KernelTypes::Bool>(Target);
                        }

                        Result operator()(Arguments... Args) const
                        {
                            if (!Target)
                                throw KernelAssert::LogicError("Function is empty");

                            return Target->Invoke(KernelTypes::Forward<Arguments>(Args)...);
                        }
                };
            }
        }
    }
}
