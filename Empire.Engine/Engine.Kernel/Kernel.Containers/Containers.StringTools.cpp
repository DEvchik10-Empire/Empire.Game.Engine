#include "Empire.Engine/Engine.Kernel/Kernel.Containers/Containers.StringTools.hpp"

#include "Empire.Engine/Engine.Kernel/Kernel.Assert/Assert.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        using namespace EmpireEngine::EngineKernel::KernelTypes;

        namespace EmpireEngine
        {
            namespace EngineKernel
            {
                namespace KernelContainers
                {
                    namespace
                    {
                        Size FindText(StringView Source, StringView Pattern, Size Start)
                        {
                            if (Pattern.GetCount() == 0 || Pattern.GetCount() > Source.GetCount())
                                return MaxValue<Size>();

                            for (Size Index = Start; Index + Pattern.GetCount() <= Source.GetCount(); ++Index)
                            {
                                Size Match = 0;
                                while (Match < Pattern.GetCount() && Source[Index + Match] == Pattern[Match])
                                    ++Match;

                                if (Match == Pattern.GetCount())
                                    return Index;
                            }

                            return MaxValue<Size>();
                        }
                    }

                    // Строковые операции из Jolt 5.7.0, Core/StringTools.h/.cpp; реализованы средствами KernelContainers.
                    void Replace(String& Value, StringView Search, StringView Replacement)
                    {
                        KernelAssert::Check(!Search.IsEmpty(), "Replace: строка поиска не должна быть пустой");
                        String Replaced;
                        StringView Source = Value.GetView();
                        Size SourceOffset = 0;

                        for (;;)
                        {
                            const Size Match = FindText(Source, Search, SourceOffset);
                            if (Match == MaxValue<Size>())
                                break;

                            Replaced.Append(StringView(Source.GetText() + SourceOffset, Match - SourceOffset));
                            Replaced.Append(Replacement);
                            SourceOffset = Match + Search.GetCount();
                        }

                        if (SourceOffset < Source.GetCount())
                            Replaced.Append(StringView(Source.GetText() + SourceOffset, Source.GetCount() - SourceOffset));
                        Value = Move(Replaced);
                    }

                    void Split(StringView Value, Array<String>& Output, StringView Delimiter, Bool ClearOutput)
                    {
                        KernelAssert::Check(!Delimiter.IsEmpty(), "Split: разделитель не должен быть пустым");
                        if (ClearOutput)
                            Output.Clear();
                        if (Value.IsEmpty())
                            return;

                        Size Start = 0;
                        for (;;)
                        {
                            const Size Match = FindText(Value, Delimiter, Start);
                            if (Match == MaxValue<Size>())
                                break;

                            Output.EmplaceBack(StringView(Value.GetText() + Start, Match - Start));
                            Start = Match + Delimiter.GetCount();
                        }

                        // Jolt StringToVector appends the remaining suffix even when it is empty;
                        // a trailing delimiter therefore produces a final empty element.
                        Output.EmplaceBack(StringView(Value.GetText() + Start, Value.GetCount() - Start));
                    }

                    void Join(const Array<String>& Values, String& Output, StringView Delimiter)
                    {
                        Output.Clear();
                        for (Size Index = 0; Index < Values.GetCount(); ++Index)
                        {
                            if (!Output.IsEmpty())
                                Output.Append(Delimiter);
                            Output.Append(Values[Index]);
                        }
                    }

                    String ToLowerAscii(StringView Value)
                    {
                        String Result;
                        for (Size Index = 0; Index < Value.GetCount(); ++Index)
                        {
                            Char Character = Value[Index];
                            if (Character >= 'A' && Character <= 'Z')
                                Character = static_cast<Char>(Character + ('a' - 'A'));
                            Result.Append(Character);
                        }
                        return Result;
                    }

                    const Char* NibbleToBinary(UInt32 Value) noexcept
                    {
                        static constexpr const Char* BinaryValues[] = {
                            "0000", "0001", "0010", "0011", "0100", "0101", "0110", "0111",
                            "1000", "1001", "1010", "1011", "1100", "1101", "1110", "1111"
                        };
                        return BinaryValues[Value & 0x0f];
                    }
                }
            }
        }
    }
}
