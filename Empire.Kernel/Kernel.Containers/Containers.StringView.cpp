#include "Empire.Kernel/Kernel.Containers/Containers.StringView.hpp"

#include "Empire.Kernel/Kernel.Assert/Assert.hpp"
#include "Empire.Kernel/Kernel.Memory/Memory.hpp"

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
                        throw Assert::OutOfRangeError("StringView index is out of range");

                    return Text[Index];
                }

                Bool operator==(StringView Left, StringView Right) noexcept
                {
                    return Left.GetCount() == Right.GetCount()
                        && Memory::CompareBytes(Left.GetText(), Right.GetText(), Left.GetCount()) == 0;
                }

                Bool operator!=(StringView Left, StringView Right) noexcept
                {
                    return !(Left == Right);
                }
            }
        }
    }
}
