#include "Empire.Kernel/Kernel.Containers/Containers.String.hpp"

#include "Empire.Kernel/Kernel.Memory/Memory.hpp"

#include <cstdint>
#include <limits>
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
                String::String(const Char* Source)
                {
                    if (Source == nullptr)
                        return;

                    while (Source[Count] != '\0')
                        Count++;

                    Capacity = Count;
                    Text = static_cast<Char*>(Memory::Allocate(Capacity + 1, alignof(Char)));

                    for (Size Index = 0; Index < Count; Index++)
                        Text[Index] = Source[Index];

                    Text[Count] = '\0';
                }

                String::String(const String& Other)
                {
                    if (Other.Count == 0)
                        return;

                    Count = Other.Count;
                    Capacity = Other.Count;
                    Text = static_cast<Char*>(Memory::Allocate(Capacity + 1, alignof(Char)));

                    for (Size Index = 0; Index < Count; Index++)
                        Text[Index] = Other.Text[Index];

                    Text[Count] = '\0';
                }

                String::String(String&& Other) noexcept
                    : Text(Other.Text), Count(Other.Count), Capacity(Other.Capacity)
                {
                    Other.Text = nullptr;
                    Other.Count = 0;
                    Other.Capacity = 0;
                }

                String::~String()
                {
                    Memory::Deallocate(Text, alignof(Char));
                }

                String& String::operator=(const String& Other)
                {
                    if (this == &Other)
                        return *this;

                    Char* NewText = nullptr;

                    if (Other.Count > 0)
                    {
                        NewText = static_cast<Char*>(Memory::Allocate(Other.Count + 1, alignof(Char)));

                        for (Size Index = 0; Index < Other.Count; Index++)
                            NewText[Index] = Other.Text[Index];

                        NewText[Other.Count] = '\0';
                    }

                    Memory::Deallocate(Text, alignof(Char));
                    Text = NewText;
                    Count = Other.Count;
                    Capacity = Other.Count;

                    return *this;
                }

                String& String::operator=(String&& Other) noexcept
                {
                    if (this == &Other)
                        return *this;

                    Memory::Deallocate(Text, alignof(Char));

                    Text = Other.Text;
                    Count = Other.Count;
                    Capacity = Other.Capacity;

                    Other.Text = nullptr;
                    Other.Count = 0;
                    Other.Capacity = 0;

                    return *this;
                }

                Size String::GetCount() const noexcept
                {
                    return Count;
                }

                Size String::GetCapacity() const noexcept
                {
                    return Capacity;
                }

                Bool String::IsEmpty() const noexcept
                {
                    return Count == 0;
                }

                const Char* String::GetText() const noexcept
                {
                    return Text != nullptr ? Text : "";
                }

                Char* String::GetData() noexcept
                {
                    return Text;
                }

                Char& String::operator[](Size Index)
                {
                    if (Index >= Count)
                        throw std::out_of_range("String index is out of range");

                    return Text[Index];
                }

                const Char& String::operator[](Size Index) const
                {
                    if (Index >= Count)
                        throw std::out_of_range("String index is out of range");

                    return Text[Index];
                }

                void String::Reserve(Size NewCapacity)
                {
                    if (NewCapacity <= Capacity)
                        return;

                    if (NewCapacity == std::numeric_limits<Size>::max())
                        throw std::length_error("String capacity is too large");

                    Char* NewText = static_cast<Char*>(Memory::Allocate(NewCapacity + 1, alignof(Char)));

                    for (Size Index = 0; Index < Count; Index++)
                        NewText[Index] = Text[Index];

                    NewText[Count] = '\0';

                    Memory::Deallocate(Text, alignof(Char));
                    Text = NewText;
                    Capacity = NewCapacity;
                }

                void String::Append(const Char* Source)
                {
                    if (Source == nullptr || Source[0] == '\0')
                        return;

                    Size SourceCount = 0;
                    while (Source[SourceCount] != '\0')
                        SourceCount++;

                    if (SourceCount > std::numeric_limits<Size>::max() - Count - 1)
                        throw std::length_error("String size is too large");

                    const std::uintptr_t SourceAddress = reinterpret_cast<std::uintptr_t>(Source);
                    const std::uintptr_t TextAddress = reinterpret_cast<std::uintptr_t>(Text);
                    const Bool IsInternal = Text != nullptr
                        && SourceAddress >= TextAddress
                        && SourceAddress <= TextAddress + Count;
                    const Size SourceOffset = IsInternal ? static_cast<Size>(SourceAddress - TextAddress) : 0;

                    const Size NewCount = Count + SourceCount;

                    if (NewCount > Capacity)
                    {
                        Size NewCapacity = Capacity == 0 ? 8 : Capacity;

                        while (NewCapacity < NewCount)
                        {
                            if (NewCapacity > std::numeric_limits<Size>::max() / 2)
                            {
                                NewCapacity = NewCount;
                                break;
                            }

                            NewCapacity *= 2;
                        }

                        Reserve(NewCapacity);
                    }

                    if (IsInternal)
                        Source = Text + SourceOffset;

                    for (Size Index = 0; Index < SourceCount; Index++)
                        Text[Count + Index] = Source[Index];

                    Count = NewCount;
                    Text[Count] = '\0';
                }

                void String::Append(const String& Source)
                {
                    Append(Source.GetText());
                }

                void String::Clear() noexcept
                {
                    Count = 0;

                    if (Text != nullptr)
                        Text[0] = '\0';
                }
            }
        }
    }
}
