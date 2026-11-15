#include "Empire.Kernel/Kernel.Reflection/Reflection.TypeRegistry.hpp"

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
                    TypeKey NativeType
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
                    return NameValue.GetView();
                }

                Size TypeInfo::GetSize() const noexcept
                {
                    return SizeValue;
                }

                Size TypeInfo::GetAlignment() const noexcept
                {
                    return AlignmentValue;
                }

                TypeKey TypeInfo::GetNativeType() const noexcept
                {
                    return NativeTypeValue;
                }

                TypeId TypeRegistry::Register(
                    TypeKey NativeType,
                    const Char* Name,
                    Size ByteSize,
                    Size Alignment
                )
                {
                    if (Name == nullptr || Name[0] == '\0')
                        throw Assert::InvalidArgumentError("Reflection type name cannot be empty");

                    if (Alignment == 0 || (Alignment & (Alignment - 1)) != 0)
                        throw Assert::InvalidArgumentError("Reflection type alignment is invalid");

                    const Containers::StringView NewName(Name);

                    Sync::LockGuard Lock(StateMutex);

                    const TypeId* ExistingType = NativeTypes.Find(NativeType);

                    if (ExistingType != nullptr)
                    {
                        const Memory::SharedPtr<TypeInfo>* ExistingInfo = RegisteredTypes.Find(*ExistingType);

                        if (ExistingInfo == nullptr)
                            throw Assert::LogicError("Reflection registry is inconsistent");

                        if ((*ExistingInfo)->GetName() != NewName)
                            throw Assert::LogicError("Native type is already registered with another name");

                        return (*ExistingInfo)->GetId();
                    }

                    Bool NameExists = false;

                    RegisteredTypes.ForEach(
                        [&](const TypeId&, const Memory::SharedPtr<TypeInfo>& RegisteredInfo)
                        {
                            if (RegisteredInfo->GetName() == NewName)
                            {
                                NameExists = true;
                                return false;
                            }

                            return true;
                        }
                    );

                    if (NameExists)
                        throw Assert::LogicError("Reflection type name is already registered");

                    if (NextId == 0 || NextId == MaxValue<TypeId>())
                        throw Assert::OverflowError("Reflection type id space is exhausted");

                    const TypeId Id = NextId;

                    Memory::SharedPtr<TypeInfo> Info = Memory::MakeShared<TypeInfo>(Id, Name, ByteSize, Alignment, NativeType);

                    if (!RegisteredTypes.Insert(Id, Move(Info)))
                        throw Assert::LogicError("Reflection type id collision");

                    try
                    {
                        NativeTypes.Insert(NativeType, Id);
                    }
                    catch (...)
                    {
                        RegisteredTypes.Remove(Id);
                        throw;
                    }

                    NextId++;
                    return Id;
                }

                const TypeInfo* TypeRegistry::Find(TypeId Id) const
                {
                    Sync::LockGuard Lock(StateMutex);
                    const Memory::SharedPtr<TypeInfo>* Info = RegisteredTypes.Find(Id);
                    return Info == nullptr ? nullptr : Info->Get();
                }

                const TypeInfo* TypeRegistry::FindByName(Containers::StringView Name) const
                {
                    Sync::LockGuard Lock(StateMutex);

                    const TypeInfo* Result = nullptr;

                    RegisteredTypes.ForEach(
                        [&](const TypeId&, const Memory::SharedPtr<TypeInfo>& RegisteredInfo)
                        {
                            if (RegisteredInfo->GetName() == Name)
                            {
                                Result = RegisteredInfo.Get();
                                return false;
                            }

                            return true;
                        }
                    );

                    return Result;
                }

                Size TypeRegistry::GetTypeCount() const
                {
                    Sync::LockGuard Lock(StateMutex);
                    return RegisteredTypes.GetCount();
                }
            }
        }
    }
}
