#include "Empire.Kernel/Kernel.Containers/Containers.String.hpp"

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
                namespace
                {
                    Size MeasureText(const Char* Source) noexcept
                    {
                        Size Length = 0;

                        while (Source[Length] != '\0')
                            Length++;

                        return Length;
                    }

                    Char* DuplicateText(const Char* Source, Size SourceCount)
                    {
                        Char* Result = static_cast<Char*>(Memory::Allocate(SourceCount + 1, alignof(Char)));

                        Memory::CopyBytes(Result, Source, SourceCount);
                        Result[SourceCount] = '\0';

                        return Result;
                    }
                }

                String::String(const Char* Source)
                {
                    if (Source == nullptr)
                        return;

                    Count = MeasureText(Source);
                    Capacity = Count;
                    Text = DuplicateText(Source, Count);
                }

                String::String(StringView Source)
                {
                    if (Source.GetCount() == 0)
                        return;

                    Count = Source.GetCount();
                    Capacity = Count;
                    Text = DuplicateText(Source.GetText(), Count);
                }

                String::String(const String& Other)
                {
                    if (Other.Count == 0)
                        return;

                    Count = Other.Count;
                    Capacity = Other.Count;
                    Text = DuplicateText(Other.Text, Count);
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
                    Memory::Deallocate(Text);
                }

                String& String::operator=(const String& Other)
                {
                    if (this == &Other)
                        return *this;

                    Char* NewText = nullptr;

                    if (Other.Count > 0)
                        NewText = DuplicateText(Other.Text, Other.Count);

                    Memory::Deallocate(Text);
                    Text = NewText;
                    Count = Other.Count;
                    Capacity = Other.Count;

                    return *this;
                }

                String& String::operator=(String&& Other) noexcept
                {
                    if (this == &Other)
                        return *this;

                    Memory::Deallocate(Text);

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

                StringView String::GetView() const noexcept
                {
                    return StringView(GetText(), Count);
                }

                Char& String::operator[](Size Index)
                {
                    if (Index >= Count)
                        throw Assert::OutOfRangeError("String index is out of range");

                    return Text[Index];
                }

                const Char& String::operator[](Size Index) const
                {
                    if (Index >= Count)
                        throw Assert::OutOfRangeError("String index is out of range");

                    return Text[Index];
                }

                void String::Reserve(Size NewCapacity)
                {
                    if (NewCapacity <= Capacity)
                        return;

                    if (NewCapacity == MaxValue<Size>())
                        throw Assert::LengthError("String capacity is too large");

                    Char* NewText = static_cast<Char*>(Memory::Allocate(NewCapacity + 1, alignof(Char)));

                    Memory::CopyBytes(NewText, Text, Count);
                    NewText[Count] = '\0';

                    Memory::Deallocate(Text);
                    Text = NewText;
                    Capacity = NewCapacity;
                }

                void String::AppendRange(const Char* Source, Size SourceCount)
                {
                    if (Source == nullptr || SourceCount == 0)
                        return;

                    if (SourceCount > MaxValue<Size>() - Count - 1)
                        throw Assert::LengthError("String size is too large");

                    const UIntPtr SourceAddress = reinterpret_cast<UIntPtr>(Source);
                    const UIntPtr TextAddress = reinterpret_cast<UIntPtr>(Text);
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
                            if (NewCapacity > MaxValue<Size>() / 2)
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

                    Memory::MoveBytes(Text + Count, Source, SourceCount);

                    Count = NewCount;
                    Text[Count] = '\0';
                }

                void String::Append(const Char* Source)
                {
                    if (Source == nullptr)
                        return;

                    AppendRange(Source, MeasureText(Source));
                }

                void String::Append(StringView Source)
                {
                    AppendRange(Source.GetText(), Source.GetCount());
                }

                void String::Append(const String& Source)
                {
                    AppendRange(Source.Text, Source.Count);
                }

                void String::Append(Char Character)
                {
                    AppendRange(&Character, 1);
                }

                void String::Clear() noexcept
                {
                    Count = 0;

                    if (Text != nullptr)
                        Text[0] = '\0';
                }

                String& String::operator+=(const Char* Source)
                {
                    Append(Source);
                    return *this;
                }

                String& String::operator+=(const String& Source)
                {
                    Append(Source);
                    return *this;
                }

                String& String::operator+=(Char Character)
                {
                    Append(Character);
                    return *this;
                }

                String operator+(const String& Left, const String& Right)
                {
                    String Result(Left);
                    Result.Append(Right);
                    return Result;
                }

                String operator+(const String& Left, const Char* Right)
                {
                    String Result(Left);
                    Result.Append(Right);
                    return Result;
                }

                String operator+(const Char* Left, const String& Right)
                {
                    String Result(Left);
                    Result.Append(Right);
                    return Result;
                }

                Bool operator==(const String& Left, const String& Right) noexcept
                {
                    return Left.GetView() == Right.GetView();
                }

                Bool operator!=(const String& Left, const String& Right) noexcept
                {
                    return !(Left == Right);
                }

                String FormatUnsigned(UInt64 Value)
                {
                    Char Buffer[24];
                    Size Position = sizeof(Buffer);

                    Buffer[--Position] = '\0';

                    do
                    {
                        Buffer[--Position] = static_cast<Char>('0' + Value % 10);
                        Value /= 10;
                    }
                    while (Value != 0);

                    return String(Buffer + Position);
                }

                String FormatSigned(Int64 Value)
                {
                    if (Value >= 0)
                        return FormatUnsigned(static_cast<UInt64>(Value));

                    const UInt64 Magnitude = static_cast<UInt64>(0) - static_cast<UInt64>(Value);

                    String Result("-");
                    Result.Append(FormatUnsigned(Magnitude));
                    return Result;
                }

                String FormatFloating(Double Value)
                {
                    if (Value != Value)
                        return String("nan");

                    String Result;

                    if (Value < 0.0)
                    {
                        Result.Append('-');
                        Value = -Value;
                    }

                    if (Value - Value != 0.0)
                    {
                        Result.Append("inf");
                        return Result;
                    }

                    if (Value == 0.0)
                    {
                        Result.Append('0');
                        return Result;
                    }

                    constexpr Double FixedLower = 1e-5;
                    constexpr Double FixedUpper = 1e15;

                    if (Value >= FixedLower && Value < FixedUpper)
                    {
                        UInt64 Whole = static_cast<UInt64>(Value);
                        UInt64 Scaled = static_cast<UInt64>((Value - static_cast<Double>(Whole)) * 1000000.0 + 0.5);

                        if (Scaled >= 1000000)
                        {
                            Whole++;
                            Scaled -= 1000000;
                        }

                        Result.Append(FormatUnsigned(Whole));

                        if (Scaled != 0)
                        {
                            Char Digits[7];

                            for (Size Index = 6; Index > 0; Index--)
                            {
                                Digits[Index - 1] = static_cast<Char>('0' + Scaled % 10);
                                Scaled /= 10;
                            }

                            Size Length = 6;

                            while (Length > 0 && Digits[Length - 1] == '0')
                                Length--;

                            Digits[Length] = '\0';

                            Result.Append('.');
                            Result.Append(static_cast<const Char*>(Digits));
                        }

                        return Result;
                    }

                    Int32 Exponent = 0;

                    while (Value >= 10.0)
                    {
                        Value /= 10.0;
                        Exponent++;
                    }

                    while (Value < 1.0)
                    {
                        Value *= 10.0;
                        Exponent--;
                    }

                    UInt64 Mantissa = static_cast<UInt64>(Value * 1000000.0 + 0.5);

                    if (Mantissa >= 10000000)
                    {
                        Mantissa /= 10;
                        Exponent++;
                    }

                    Result.Append(static_cast<Char>('0' + Mantissa / 1000000));

                    UInt64 Fraction = Mantissa % 1000000;

                    if (Fraction != 0)
                    {
                        Char Digits[7];

                        for (Size Index = 6; Index > 0; Index--)
                        {
                            Digits[Index - 1] = static_cast<Char>('0' + Fraction % 10);
                            Fraction /= 10;
                        }

                        Size Length = 6;

                        while (Length > 0 && Digits[Length - 1] == '0')
                            Length--;

                        Digits[Length] = '\0';

                        Result.Append('.');
                        Result.Append(static_cast<const Char*>(Digits));
                    }

                    Result.Append('e');
                    Result.Append(Exponent < 0 ? '-' : '+');

                    const UInt64 ExponentMagnitude = Exponent < 0
                        ? static_cast<UInt64>(-static_cast<Int64>(Exponent))
                        : static_cast<UInt64>(Exponent);

                    if (ExponentMagnitude < 10)
                        Result.Append('0');

                    Result.Append(FormatUnsigned(ExponentMagnitude));
                    return Result;
                }
            }
        }
    }
}
