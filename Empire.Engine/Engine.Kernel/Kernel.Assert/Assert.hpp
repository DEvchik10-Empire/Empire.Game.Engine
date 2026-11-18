#pragma once

#include "Empire.Engine/Engine.Kernel/Kernel.Types/Types.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireEngine
        {
            namespace EngineKernel
            {
                namespace KernelAssert
                {
                    class Error
                    {
                        private:
                            KernelTypes::Char MessageText[256];
                            KernelTypes::SourceLocation LocationValue;

                        public:
                            Error(
                                const KernelTypes::Char* Message,
                                const KernelTypes::SourceLocation& Location = KernelTypes::SourceLocation::Current()
                            ) noexcept;

                            Error(const Error& Other) noexcept = default;
                            Error& operator=(const Error& Other) noexcept = default;
                            virtual ~Error() = default;

                            virtual const KernelTypes::Char* GetKind() const noexcept;

                            const KernelTypes::Char* GetText() const noexcept;
                            const KernelTypes::SourceLocation& GetLocation() const noexcept;
                    };

                    class LogicError : public Error
                    {
                        public:
                            using Error::Error;

                            const KernelTypes::Char* GetKind() const noexcept override;
                    };

                    class InvalidArgumentError : public LogicError
                    {
                        public:
                            using LogicError::LogicError;

                            const KernelTypes::Char* GetKind() const noexcept override;
                    };

                    class OutOfRangeError : public LogicError
                    {
                        public:
                            using LogicError::LogicError;

                            const KernelTypes::Char* GetKind() const noexcept override;
                    };

                    class LengthError : public LogicError
                    {
                        public:
                            using LogicError::LogicError;

                            const KernelTypes::Char* GetKind() const noexcept override;
                    };

                    class OverflowError : public Error
                    {
                        public:
                            using Error::Error;

                            const KernelTypes::Char* GetKind() const noexcept override;
                    };

                    class AllocationError : public Error
                    {
                        public:
                            using Error::Error;

                            const KernelTypes::Char* GetKind() const noexcept override;
                    };

                    void Check(
                        KernelTypes::Bool Condition,
                        const KernelTypes::Char* Message,
                        const KernelTypes::SourceLocation& Location = KernelTypes::SourceLocation::Current()
                    );

                    // Проверка только в отладочной сборке. В релизе тело пустое и после подстановки компилятор убирает
                    // и условие, если оно без побочных эффектов (сравнения, индексы). Для горячих путей, где в условии стоит
                    // вызов с побочным эффектом, пользуйся обычным Check только если проверка нужна и в релизе.
                    inline void DebugCheck(
                        KernelTypes::Bool Condition,
                        const KernelTypes::Char* Message,
                        const KernelTypes::SourceLocation& Location = KernelTypes::SourceLocation::Current()
                    )
                    {
                        if constexpr (KernelCompiler::IsDebugBuild)
                        {
                            Check(Condition, Message, Location);
                        }
                        else
                        {
                            (void)Condition;
                            (void)Message;
                            (void)Location;
                        }
                    }

                    [[noreturn]] void Fail(
                        const KernelTypes::Char* Message,
                        const KernelTypes::SourceLocation& Location = KernelTypes::SourceLocation::Current()
                    );
                }
            }
        }
    }
}
