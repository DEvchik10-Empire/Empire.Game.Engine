#pragma once

#include "Empire.Kernel/Kernel.Types/Types.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EngineKernel
        {
            namespace Containers
            {
                class StringView
                {
                    private:
                        const Types::Char* Text = nullptr;
                        Types::Size Count = 0;

                    public:
                        StringView() = default;
                        StringView(const Types::Char* Source);
                        StringView(const Types::Char* Source, Types::Size SourceCount) noexcept;

                        Types::Size GetCount() const noexcept;
                        Types::Bool IsEmpty() const noexcept;
                        const Types::Char* GetText() const noexcept;
                        const Types::Char& operator[](Types::Size Index) const;
                };

                Types::Bool operator==(StringView Left, StringView Right) noexcept;
                Types::Bool operator!=(StringView Left, StringView Right) noexcept;
            }
        }
    }
}
