#pragma once

#include "Empire.Engine/Engine.Kernel/Kernel.Containers/Containers.Array.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Containers/Containers.String.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Assert/Assert.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireEngine
        {
            namespace EngineKernel
            {
                namespace KernelContainers
                {
                    namespace StringFormatDetail
                    {
                        inline void AppendValue(String& Output, const String& Value) { Output.Append(Value); }
                        inline void AppendValue(String& Output, StringView Value) { Output.Append(Value); }
                        inline void AppendValue(String& Output, const KernelTypes::Char* Value)
                        {
                            if (Value != nullptr)
                                Output.Append(Value);
                        }
                        inline void AppendValue(String& Output, KernelTypes::Char Value) { Output.Append(Value); }

                        template <KernelTypes::Size Count>
                        inline void AppendValue(String& Output, const KernelTypes::Char (&Value)[Count])
                        {
                            Output.Append(StringView(Value, Count - 1));
                        }

                        template <typename Value>
                            requires (KernelTypes::Integral<Value> || KernelTypes::FloatingPoint<Value>)
                        inline void AppendValue(String& Output, Value Item)
                        {
                            Output.Append(ToString(Item));
                        }

                        template <typename Value>
                            requires (!(KernelTypes::Integral<Value> || KernelTypes::FloatingPoint<Value>) && requires(String& Output, Value&& Item) { Output.Append(ToString(KernelTypes::Forward<Value>(Item))); })
                        inline void AppendValue(String& Output, Value&& Item)
                        {
                            Output.Append(ToString(KernelTypes::Forward<Value>(Item)));
                        }

                        inline KernelTypes::Size FindPlaceholder(StringView Pattern, KernelTypes::Size Start)
                        {
                            for (KernelTypes::Size Index = Start; Index + 1 < Pattern.GetCount(); ++Index)
                            {
                                if (Pattern[Index] == '{' && Pattern[Index + 1] == '}')
                                    return Index;
                            }
                            return KernelTypes::MaxValue<KernelTypes::Size>();
                        }

                        inline KernelTypes::Size CountPlaceholders(StringView Pattern)
                        {
                            KernelTypes::Size Count = 0;
                            for (KernelTypes::Size Index = 0; Index + 1 < Pattern.GetCount(); ++Index)
                            {
                                if (Pattern[Index] == '{' && Pattern[Index + 1] == '}')
                                {
                                    ++Count;
                                    ++Index;
                                }
                            }
                            return Count;
                        }

                        inline void AppendFormat(String& Output, StringView Pattern)
                        {
                            Output.Append(Pattern);
                        }

                        template <typename Value, typename... Values>
                        void AppendFormat(String& Output, StringView Pattern, Value&& Item, Values&&... Rest)
                        {
                            const KernelTypes::Size Placeholder = FindPlaceholder(Pattern, 0);
                            if (Placeholder == KernelTypes::MaxValue<KernelTypes::Size>())
                            {
                                Output.Append(Pattern);
                                return;
                            }

                            Output.Append(StringView(Pattern.GetText(), Placeholder));
                            AppendValue(Output, KernelTypes::Forward<Value>(Item));
                            const KernelTypes::Size Next = Placeholder + 2;
                            AppendFormat(
                                Output,
                                StringView(Pattern.GetText() + Next, Pattern.GetCount() - Next),
                                KernelTypes::Forward<Values>(Rest)...
                            );
                        }
                    }

                    void Replace(String& Value, StringView Search, StringView Replacement);
                    void Split(StringView Value, Array<String>& Output, StringView Delimiter = StringView(","), KernelTypes::Bool ClearOutput = true);
                    void Join(const Array<String>& Values, String& Output, StringView Delimiter = StringView(","));
                    [[nodiscard]] String ToLowerAscii(StringView Value);
                    [[nodiscard]] const KernelTypes::Char* NibbleToBinary(KernelTypes::UInt32 Value) noexcept;

                    inline String ConvertToString(const String& Value) { return Value; }
                    inline String ConvertToString(StringView Value) { return String(Value); }
                    inline String ConvertToString(const KernelTypes::Char* Value) { return String(Value); }
                    template <typename Value>
                        requires (KernelTypes::Integral<Value> || KernelTypes::FloatingPoint<Value>)
                    [[nodiscard]] String ConvertToString(Value Item)
                    {
                        return KernelContainers::ToString(Item);
                    }

                    template <typename Value>
                        requires (!(KernelTypes::Integral<Value> || KernelTypes::FloatingPoint<Value>) && requires(const Value& Item) { String(ToString(Item)); })
                    [[nodiscard]] String ConvertToString(const Value& Item)
                    {
                        return String(ToString(Item));
                    }

                    // Форматирование только типобезопасными {} параметрами; C-style varargs из Jolt запрещены правилами Empire.
                    template <typename... Values>
                    [[nodiscard]] String Format(StringView Pattern, Values&&... Items)
                    {
                        KernelAssert::Check(
                            StringFormatDetail::CountPlaceholders(Pattern) == sizeof...(Values),
                            "Format: количество {} placeholder-ов не совпадает с количеством аргументов"
                        );
                        String Result;
                        StringFormatDetail::AppendFormat(Result, Pattern, KernelTypes::Forward<Values>(Items)...);
                        return Result;
                    }
                }
            }
        }
    }
}
