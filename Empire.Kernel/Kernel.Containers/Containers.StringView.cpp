#include "Empire.Kernel/Kernel.Containers/Containers.StringView.hpp"

#include <stdexcept>

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        using namespace EngineKernel::Types;

        namespace EngineKernel
        {
            namespace Containers
            {
                StringView::StringView(const Char* Source)
                {
                    if (Source == nullptr)
                        return;

                    Text = Source;

                    while (Source[Count] != '\0')
                        Count++;
                }

                StringView::StringView(const Char* Source, Size SourceCount) noexcept
                    : Text(Source), Count(Source != nullptr ? SourceCount : 0)
                {
                }

                Size StringView::GetCount() const noexcept
                {
                    return Count;
                }

                Bool StringView::IsEmpty() const noexcept
                {
                    return Count == 0;
                }

                const Char* StringView::GetText() const noexcept
                {
                    return Text != nullptr ? Text : "";
                }

                const Char& StringView::operator[](Size Index) const
                {
                    if (Index >= Count)
                        throw std::out_of_range("StringView index is out of range");

                    return Text[Index];
                }
            }
        }
    }
}
