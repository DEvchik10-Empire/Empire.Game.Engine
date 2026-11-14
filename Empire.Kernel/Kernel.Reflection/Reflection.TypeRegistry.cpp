#include "Empire.Kernel/Kernel.Reflection/Reflection.TypeRegistry.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        using namespace EmpireKernel::KernelTypes;

        namespace EmpireKernel
        {
            namespace KernelReflection
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

                KernelContainers::StringView TypeInfo::GetName() const noexcept
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
                        throw KernelAssert::InvalidArgumentError("Reflection type name cannot be empty");

                    if (Alignment == 0 || (Alignment & (Alignment - 1)) != 0)
                        throw KernelAssert::InvalidArgumentError("Reflection type alignment is invalid");

                    const KernelContainers::StringView NewName(Name);

                    KernelSync::LockGuard Lock(StateMutex);

                    const TypeId* ExistingType = NativeTypes.Find(NativeType);

                    if (ExistingType != nullptr)
                    {
                        const KernelMemory::SharedPtr<TypeInfo>* ExistingInfo = RegisteredTypes.Find(*ExistingType);

                        if (ExistingInfo == nullptr)
                            throw KernelAssert::LogicError("Reflection registry is inconsistent");

                        if ((*ExistingInfo)->GetName() != NewName)
                            throw KernelAssert::LogicError("Native type is already registered with another name");

                        return (*ExistingInfo)->GetId();
                    }

                    Bool NameExists = false;

                    RegisteredTypes.ForEach(
                        [&](const TypeId&, const KernelMemory::SharedPtr<TypeInfo>& RegisteredInfo)
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
                        throw KernelAssert::LogicError("Reflection type name is already registered");

                    if (NextId == 0 || NextId == MaxValue<TypeId>())
                        throw KernelAssert::OverflowError("Reflection type id space is exhausted");

                    const TypeId Id = NextId;

                    KernelMemory::SharedPtr<TypeInfo> Info = KernelMemory::MakeShared<TypeInfo>(Id, Name, ByteSize, Alignment, NativeType);

                    if (!RegisteredTypes.Insert(Id, Move(Info)))
                        throw KernelAssert::LogicError("Reflection type id collision");

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
                    KernelSync::LockGuard Lock(StateMutex);
                    const KernelMemory::SharedPtr<TypeInfo>* Info = RegisteredTypes.Find(Id);
                    return Info == nullptr ? nullptr : Info->Get();
                }

                const TypeInfo* TypeRegistry::FindByName(KernelContainers::StringView Name) const
                {
                    KernelSync::LockGuard Lock(StateMutex);

                    const TypeInfo* Result = nullptr;

                    RegisteredTypes.ForEach(
                        [&](const TypeId&, const KernelMemory::SharedPtr<TypeInfo>& RegisteredInfo)
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
                    KernelSync::LockGuard Lock(StateMutex);
                    return RegisteredTypes.GetCount();
                }
            }
        }
    }
}
