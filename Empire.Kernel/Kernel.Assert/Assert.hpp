#pragma once

#include "Empire.Kernel/Kernel.Types/Types.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EngineKernel
        {
            namespace Assert
            {
                class Error
                {
                    private:
                        Types::Char MessageText[256];
                        Types::SourceLocation LocationValue;

                    public:
                        Error(
                            const Types::Char* Message,
                            const Types::SourceLocation& Location = Types::SourceLocation::Current()
                        ) noexcept;

                        Error(const Error& Other) noexcept = default;
                        Error& operator=(const Error& Other) noexcept = default;
                        virtual ~Error() = default;

                        virtual const Types::Char* GetKind() const noexcept;

                        const Types::Char* GetText() const noexcept;
                        const Types::SourceLocation& GetLocation() const noexcept;
                };

                class LogicError : public Error
                {
                    public:
                        using Error::Error;

                        const Types::Char* GetKind() const noexcept override;
                };

                class InvalidArgumentError : public LogicError
                {
                    public:
                        using LogicError::LogicError;

                        const Types::Char* GetKind() const noexcept override;
                };

                class OutOfRangeError : public LogicError
                {
                    public:
                        using LogicError::LogicError;

                        const Types::Char* GetKind() const noexcept override;
                };

                class LengthError : public LogicError
                {
                    public:
                        using LogicError::LogicError;

                        const Types::Char* GetKind() const noexcept override;
                };

                class OverflowError : public Error
                {
                    public:
                        using Error::Error;

                        const Types::Char* GetKind() const noexcept override;
                };

                class AllocationError : public Error
                {
                    public:
                        using Error::Error;

                        const Types::Char* GetKind() const noexcept override;
                };

                void Check(
                    Types::Bool Condition,
                    const Types::Char* Message,
                    const Types::SourceLocation& Location = Types::SourceLocation::Current()
                );

                [[noreturn]] void Fail(
                    const Types::Char* Message,
                    const Types::SourceLocation& Location = Types::SourceLocation::Current()
                );
            }
        }
    }
}
