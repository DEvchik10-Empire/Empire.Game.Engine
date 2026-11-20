#pragma once

#include "Empire.Engine/Engine.Kernel/Kernel.Assert/Assert.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Containers/Containers.String.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Types/Types.Utility.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireEngine
        {
            namespace EngineKernel
            {
                namespace KernelContainers
                {
                    // Значение или сообщение об ошибке (перенесено из Jolt 5.7.0, Core/Result.h).
                    // Типы и управление временем жизни адаптированы к Kernel.Types и KernelContainers::String.
                    template <typename T>
                    class [[nodiscard]] Result
                    {
                        private:
                            union Storage
                            {
                                T Value;
                                String Error;

                                Storage() {}
                                ~Storage() {}
                            } Data;

                            enum class State : KernelTypes::UInt8
                            {
                                Empty,
                                Valid,
                                Error
                            } CurrentState = State::Empty;

                            void CopyFrom(const Result& Other)
                            {
                                if (Other.CurrentState == State::Valid)
                                    KernelTypes::ConstructAt(&Data.Value, Other.Data.Value);
                                else if (Other.CurrentState == State::Error)
                                    KernelTypes::ConstructAt(&Data.Error, Other.Data.Error);

                                CurrentState = Other.CurrentState;
                            }

                            void MoveFrom(Result&& Other)
                            {
                                if (Other.CurrentState == State::Valid)
                                    KernelTypes::ConstructAt(&Data.Value, KernelTypes::Move(Other.Data.Value));
                                else if (Other.CurrentState == State::Error)
                                    KernelTypes::ConstructAt(&Data.Error, KernelTypes::Move(Other.Data.Error));

                                CurrentState = Other.CurrentState;
                            }

                        public:
                            Result() = default;

                            Result(const Result& Other)
                            {
                                CopyFrom(Other);
                            }

                            Result(Result&& Other)
                                noexcept(KernelTypes::IsNothrowMoveConstructible<T>)
                            {
                                MoveFrom(KernelTypes::Move(Other));
                            }

                            ~Result()
                            {
                                Clear();
                            }

                            Result& operator=(const Result& Other)
                            {
                                if (this != &Other)
                                {
                                    Clear();
                                    CopyFrom(Other);
                                }

                                return *this;
                            }

                            Result& operator=(Result&& Other)
                                noexcept(KernelTypes::IsNothrowMoveConstructible<T>)
                            {
                                if (this != &Other)
                                {
                                    Clear();
                                    MoveFrom(KernelTypes::Move(Other));
                                }

                                return *this;
                            }

                            void Clear() noexcept
                            {
                                if (CurrentState == State::Valid)
                                    KernelTypes::DestroyAt(&Data.Value);
                                else if (CurrentState == State::Error)
                                    KernelTypes::DestroyAt(&Data.Error);

                                CurrentState = State::Empty;
                            }

                            [[nodiscard]] KernelTypes::Bool IsEmpty() const noexcept { return CurrentState == State::Empty; }
                            [[nodiscard]] KernelTypes::Bool IsValid() const noexcept { return CurrentState == State::Valid; }
                            [[nodiscard]] KernelTypes::Bool HasError() const noexcept { return CurrentState == State::Error; }

                            T& Get()
                            {
                                KernelAssert::DebugCheck(IsValid(), "Result::Get: результат не содержит значения");
                                return Data.Value;
                            }

                            const T& Get() const
                            {
                                KernelAssert::DebugCheck(IsValid(), "Result::Get: результат не содержит значения");
                                return Data.Value;
                            }

                            void Set(const T& Value)
                            {
                                Clear();
                                KernelTypes::ConstructAt(&Data.Value, Value);
                                CurrentState = State::Valid;
                            }

                            void Set(T&& Value)
                            {
                                Clear();
                                KernelTypes::ConstructAt(&Data.Value, KernelTypes::Move(Value));
                                CurrentState = State::Valid;
                            }

                            const String& GetError() const
                            {
                                KernelAssert::DebugCheck(HasError(), "Result::GetError: результат не содержит ошибки");
                                return Data.Error;
                            }

                            void SetError(const KernelTypes::Char* Message)
                            {
                                Clear();
                                KernelTypes::ConstructAt(&Data.Error, Message);
                                CurrentState = State::Error;
                            }

                            void SetError(StringView Message)
                            {
                                Clear();
                                KernelTypes::ConstructAt(&Data.Error, Message);
                                CurrentState = State::Error;
                            }

                            void SetError(String&& Message)
                            {
                                Clear();
                                KernelTypes::ConstructAt(&Data.Error, KernelTypes::Move(Message));
                                CurrentState = State::Error;
                            }
                    };
                }
            }
        }
    }
}
