#include "Empire.Kernel/Kernel.Reflection/Reflection.TypeRegistry.hpp"

#include <cstring>
#include <limits>
#include <tuple>

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        using namespace EngineKernel::Types;

        namespace EngineKernel
        {
            namespace Reflection
            {
                TypeInfo::TypeInfo(
                    TypeId Id,
                    const Char* Name,
                    Size ByteSize,
                    Size Alignment,
                    std::type_index NativeType
                )
                    : IdValue(Id),
                      NameValue(Name),
                      SizeValue(ByteSize),
                      AlignmentValue(Alignment),
                      NativeTypeValue(NativeType)
                {
                }

                TypeId TypeInfo::GetId() const noexcept
                {
                    return IdValue;
                }

                Containers::StringView TypeInfo::GetName() const noexcept
                {
                    return Containers::StringView(NameValue.GetText(), NameValue.GetCount());
                }

                Size TypeInfo::GetSize() const noexcept
                {
                    return SizeValue;
                }

                Size TypeInfo::GetAlignment() const noexcept
                {
                    return AlignmentValue;
                }

                const std::type_index& TypeInfo::GetNativeType() const noexcept
                {
                    return NativeTypeValue;
                }

                TypeId TypeRegistry::Register(
                    std::type_index NativeType,
                    const Char* Name,
                    Size ByteSize,
                    Size Alignment
                )
                {
                    if (Name == nullptr || Name[0] == '\0')
                        throw std::invalid_argument("Reflection type name cannot be empty");

                    if (Alignment == 0 || (Alignment & (Alignment - 1)) != 0)
                        throw std::invalid_argument("Reflection type alignment is invalid");

                    std::lock_guard Lock(Mutex);

                    const auto ExistingType = NativeTypes.find(NativeType);
                    if (ExistingType != NativeTypes.end())
                    {
                        const TypeInfo& ExistingInfo = RegisteredTypes.at(ExistingType->second);
                        const Containers::StringView ExistingName = ExistingInfo.GetName();
                        const Size NameLength = std::strlen(Name);

                        if (ExistingName.GetCount() != NameLength
                            || std::memcmp(ExistingName.GetText(), Name, NameLength) != 0)
                        {
                            throw std::logic_error("Native type is already registered with another name");
                        }

                        return ExistingInfo.GetId();
                    }

                    const Size NameLength = std::strlen(Name);

                    for (const auto& [RegisteredId, RegisteredInfo] : RegisteredTypes)
                    {
                        static_cast<void>(RegisteredId);
                        const Containers::StringView RegisteredName = RegisteredInfo.GetName();

                        if (RegisteredName.GetCount() == NameLength
                            && std::memcmp(RegisteredName.GetText(), Name, NameLength) == 0)
                        {
                            throw std::logic_error("Reflection type name is already registered");
                        }
                    }

                    if (NextId == 0 || NextId == std::numeric_limits<TypeId>::max())
                        throw std::overflow_error("Reflection type id space is exhausted");

                    const TypeId Id = NextId++;
                    auto [TypeIt, TypeInserted] = RegisteredTypes.emplace(
                        std::piecewise_construct,
                        std::forward_as_tuple(Id),
                        std::forward_as_tuple(Id, Name, ByteSize, Alignment, NativeType)
                    );

                    if (!TypeInserted)
                        throw std::logic_error("Reflection type id collision");

                    try
                    {
                        NativeTypes.emplace(NativeType, Id);
                    }
                    catch (...)
                    {
                        RegisteredTypes.erase(TypeIt);
                        throw;
                    }

                    return Id;
                }

                const TypeInfo* TypeRegistry::Find(TypeId Id) const
                {
                    std::lock_guard Lock(Mutex);
                    const auto It = RegisteredTypes.find(Id);
                    return It == RegisteredTypes.end() ? nullptr : &It->second;
                }

                const TypeInfo* TypeRegistry::FindByName(Containers::StringView Name) const
                {
                    std::lock_guard Lock(Mutex);

                    for (const auto& [RegisteredId, RegisteredInfo] : RegisteredTypes)
                    {
                        static_cast<void>(RegisteredId);
                        const Containers::StringView RegisteredName = RegisteredInfo.GetName();

                        if (RegisteredName.GetCount() == Name.GetCount()
                            && std::memcmp(RegisteredName.GetText(), Name.GetText(), Name.GetCount()) == 0)
                        {
                            return &RegisteredInfo;
                        }
                    }

                    return nullptr;
                }

                Size TypeRegistry::GetTypeCount() const
                {
                    std::lock_guard Lock(Mutex);
                    return RegisteredTypes.size();
                }
            }
        }
    }
}
