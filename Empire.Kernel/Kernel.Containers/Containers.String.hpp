#pragma once

#include "Empire.Kernel/Kernel.Containers/Containers.StringView.hpp"
#include "Empire.Kernel/Kernel.Types/Types.Utility.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireKernel
        {
            namespace KernelContainers
            {
                class String
                {
                    private:
                        KernelTypes::Char* Text = nullptr;
                        KernelTypes::Size Count = 0;
                        KernelTypes::Size Capacity = 0;

                        void Reserve(KernelTypes::Size NewCapacity);
                        void AppendRange(const KernelTypes::Char* Source, KernelTypes::Size SourceCount);

                    public:
                        String() = default;
                        String(const KernelTypes::Char* Source);
                        String(StringView Source);
                        String(const String& Other);
                        String(String&& Other) noexcept;
                        ~String();

                        String& operator=(const String& Other);
                        String& operator=(String&& Other) noexcept;

                        KernelTypes::Size GetCount() const noexcept;
                        KernelTypes::Size GetCapacity() const noexcept;
                        KernelTypes::Bool IsEmpty() const noexcept;
                        const KernelTypes::Char* GetText() const noexcept;
                        KernelTypes::Char* GetData() noexcept;
                        StringView GetView() const noexcept;

                        KernelTypes::Char& operator[](KernelTypes::Size Index);
                        const KernelTypes::Char& operator[](KernelTypes::Size Index) const;

                        void Append(const KernelTypes::Char* Source);
                        void Append(StringView Source);
                        void Append(const String& Source);
                        void Append(KernelTypes::Char Character);
                        void Clear() noexcept;

                        String& operator+=(const KernelTypes::Char* Source);
                        String& operator+=(const String& Source);
                        String& operator+=(KernelTypes::Char Character);
                };

                String operator+(const String& Left, const String& Right);
                String operator+(const String& Left, const KernelTypes::Char* Right);
                String operator+(const KernelTypes::Char* Left, const String& Right);

                KernelTypes::Bool operator==(const String& Left, const String& Right) noexcept;
                KernelTypes::Bool operator!=(const String& Left, const String& Right) noexcept;

                String FormatSigned(KernelTypes::Int64 Value);
                String FormatUnsigned(KernelTypes::UInt64 Value);
                String FormatFloating(KernelTypes::Double Value);

                template <typename T>
                    requires (KernelTypes::Integral<T> || KernelTypes::FloatingPoint<T>)
                String ToString(T Value)
                {
                    if constexpr (KernelTypes::IsSame<KernelTypes::RemoveCV<T>, KernelTypes::Bool>)
                        return String(Value ? "true" : "false");
                    else if constexpr (KernelTypes::FloatingPoint<T>)
                        return FormatFloating(static_cast<KernelTypes::Double>(Value));
                    else if constexpr (KernelTypes::IsSigned<T>)
                        return FormatSigned(static_cast<KernelTypes::Int64>(Value));
                    else
                        return FormatUnsigned(static_cast<KernelTypes::UInt64>(Value));
                }
            }
        }
    }
}
