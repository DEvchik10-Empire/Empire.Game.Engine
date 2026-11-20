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

                    // Jolt Core/IssueReporting.h equivalent: application-overridable trace and assertion hooks.
                    // Trace receives an already formatted message; formatting remains type-safe in Empire.
                    using TraceFunction = void (*)(const KernelTypes::Char* Message, const KernelTypes::SourceLocation& Location);
                    using AssertFailedFunction = KernelTypes::Bool (*)(
                        const KernelTypes::Char* Expression,
                        const KernelTypes::Char* Message,
                        const KernelTypes::SourceLocation& Location
                    );

                    void SetTraceFunction(TraceFunction Callback) noexcept;
                    void SetAssertFailedFunction(AssertFailedFunction Callback) noexcept;
                    void Trace(const KernelTypes::Char* Message, const KernelTypes::SourceLocation& Location = KernelTypes::SourceLocation::Current());
                    // Returns true when the caller should trigger a debugger breakpoint.
                    [[nodiscard]] KernelTypes::Bool ReportAssertFailure(
                        const KernelTypes::Char* Expression,
                        const KernelTypes::Char* Message,
                        const KernelTypes::SourceLocation& Location = KernelTypes::SourceLocation::Current()
                    );

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
                        const KernelTypes::Char* Expression,
                        const KernelTypes::Char* Message,
                        const KernelTypes::SourceLocation& Location = KernelTypes::SourceLocation::Current()
                    )
                    {
                        if constexpr (KernelCompiler::IsDebugBuild)
                        {
                            if (!Condition && ReportAssertFailure(Expression, Message, Location))
                                KernelCompiler::Breakpoint();
                        }
                        else
                        {
                            (void)Condition;
                            (void)Expression;
                            (void)Message;
                            (void)Location;
                        }
                    }

                    inline void DebugCheck(
                        KernelTypes::Bool Condition,
                        const KernelTypes::Char* Message,
                        const KernelTypes::SourceLocation& Location = KernelTypes::SourceLocation::Current()
                    )
                    {
                        DebugCheck(Condition, "DebugCheck", Message, Location);
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
