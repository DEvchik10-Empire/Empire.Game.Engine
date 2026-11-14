#pragma once

#include "Empire.Kernel/Kernel.Types/Types.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireKernel
        {
            namespace KernelContainers
            {
                class StringView
                {
                    private:
                        const KernelTypes::Char* Text = nullptr;
                        KernelTypes::Size Count = 0;

                    public:
                        StringView() = default;
                        StringView(const KernelTypes::Char* Source);
                        StringView(const KernelTypes::Char* Source, KernelTypes::Size SourceCount) noexcept;

                        KernelTypes::Size GetCount() const noexcept;
                        KernelTypes::Bool IsEmpty() const noexcept;
                        const KernelTypes::Char* GetText() const noexcept;
                        const KernelTypes::Char& operator[](KernelTypes::Size Index) const;
                };

                KernelTypes::Bool operator==(StringView Left, StringView Right) noexcept;
                KernelTypes::Bool operator!=(StringView Left, StringView Right) noexcept;
            }
        }
    }
}
