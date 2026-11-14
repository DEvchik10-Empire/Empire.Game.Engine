#pragma once

#include "Empire.Kernel/Kernel.Types/Types.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireKernel
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

                [[noreturn]] void Fail(
                    const KernelTypes::Char* Message,
                    const KernelTypes::SourceLocation& Location = KernelTypes::SourceLocation::Current()
                );
            }
        }
    }
}
