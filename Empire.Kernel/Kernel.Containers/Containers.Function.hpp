#pragma once

#include "Empire.Kernel/Kernel.Assert/Assert.hpp"
#include "Empire.Kernel/Kernel.Memory/Memory.SharedPtr.hpp"
#include "Empire.Kernel/Kernel.Types/Types.Utility.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EngineKernel
        {
            namespace Containers
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
                                    : Target(Types::Forward<Source>(Value))
                                {
                                }

                                Result Invoke(Arguments... Args) override
                                {
                                    return Target(Types::Forward<Arguments>(Args)...);
                                }
                        };

                        Memory::SharedPtr<Invoker> Target;

                    public:
                        Function() = default;
                        Function(Types::NullPointer) noexcept {}

                        template <typename Callable>
                            requires (!Types::IsSame<Types::RemoveCVReference<Callable>, Function>)
                        Function(Callable&& Source)
                            : Target(Memory::MakeShared<Holder<Types::RemoveCVReference<Callable>>>(Types::Forward<Callable>(Source)))
                        {
                        }

                        explicit operator Types::Bool() const noexcept
                        {
                            return static_cast<Types::Bool>(Target);
                        }

                        Result operator()(Arguments... Args) const
                        {
                            if (!Target)
                                throw Assert::LogicError("Function is empty");

                            return Target->Invoke(Types::Forward<Arguments>(Args)...);
                        }
                };
            }
        }
    }
}
