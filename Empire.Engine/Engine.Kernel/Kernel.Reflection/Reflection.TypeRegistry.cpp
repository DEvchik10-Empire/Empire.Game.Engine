#include "Empire.Engine/Engine.Kernel/Kernel.Reflection/Reflection.TypeRegistry.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        using namespace EmpireEngine::EngineKernel::KernelTypes;

        namespace EmpireEngine
        {
            namespace EngineKernel
            {
                namespace KernelReflection
                {
                    namespace
                    {
                        UInt32 HashTypeName(KernelContainers::StringView Name) noexcept
                        {
                            UInt64 Hash = 0xcbf29ce484222325ULL;
                            for (Size Index = 0; Index < Name.GetCount(); ++Index)
                            {
                                Hash ^= static_cast<UInt8>(Name[Index]);
                                Hash *= 0x100000001b3ULL;
                            }
                            return static_cast<UInt32>(Hash ^ (Hash >> 32));
                        }
                    }

                    TypeInfo::TypeInfo(
                        TypeId Id,
                        const Char* Name,
                        Size ByteSize,
                        Size Alignment,
                        TypeKey NativeType,
                        Bool IsAbstract
                    )
                        : IdValue(Id),
                          NameValue(Name),
                          SizeValue(ByteSize),
                          AlignmentValue(Alignment),
                          NativeTypeValue(NativeType),
                          AbstractValue(IsAbstract)
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

                    UInt32 TypeInfo::GetHash() const noexcept
                    {
                        return HashTypeName(NameValue.GetView());
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

                    Bool TypeInfo::IsAbstract() const noexcept
                    {
                        return AbstractValue;
                    }

                    Size TypeInfo::GetBaseClassCount() const noexcept
                    {
                        return BaseTypes.GetCount();
                    }

                    const TypeInfo* TypeInfo::GetBaseClass(Size Index) const noexcept
                    {
                        return Index < BaseTypes.GetCount() ? BaseTypes[Index].Info : nullptr;
                    }

                    Size TypeInfo::GetAttributeCount() const noexcept
                    {
                        return Attributes.size();
                    }

                    const TypeAttribute* TypeInfo::GetAttribute(Size Index) const noexcept
                    {
                        return Index < Attributes.size() ? &Attributes[Index] : nullptr;
                    }

                    const TypeAttribute* TypeInfo::FindAttribute(KernelContainers::StringView Name) const noexcept
                    {
                        for (Size Index = 0; Index < Attributes.size(); ++Index)
                        {
                            if (Attributes[Index].Name.GetView() == Name)
                                return &Attributes[Index];
                        }
                        return nullptr;
                    }

                    Bool TypeInfo::AddAttributeInternal(KernelContainers::StringView Name, void* (*Access)(void*), Size ValueSize, KernelTypes::TypeKey ValueType)
                    {
                        if (Name.IsEmpty() || FindAttribute(Name) != nullptr)
                            return false;

                        TypeAttribute Attribute;
                        Attribute.Name = KernelContainers::String(Name);
                        Attribute.Access = Access;
                        Attribute.ValueSize = ValueSize;
                        Attribute.ValueType = ValueType;
                        Attributes.EmplaceBack(KernelTypes::Move(Attribute));
                        return true;
                    }

                    Bool TypeInfo::operator==(const TypeInfo& Other) const noexcept
                    {
                        return this == &Other;
                    }

                    Bool TypeInfo::operator!=(const TypeInfo& Other) const noexcept
                    {
                        return !(*this == Other);
                    }

                    Bool TypeInfo::IsKindOf(const TypeInfo& Type) const noexcept
                    {
                        if (this == &Type)
                            return true;

                        for (const BaseType& Base : BaseTypes)
                        {
                            if (Base.Info->IsKindOf(Type))
                                return true;
                        }

                        return false;
                    }

                    const void* TypeInfo::CastTo(const void* Object, const TypeInfo& Type) const noexcept
                    {
                        if (Object == nullptr)
                            return nullptr;
                        if (this == &Type)
                            return Object;

                        for (const BaseType& Base : BaseTypes)
                        {
                            const void* BaseObject = Base.Cast(Object);
                            if (const void* Result = Base.Info->CastTo(BaseObject, Type))
                                return Result;
                        }

                        return nullptr;
                    }

                    TypeId TypeRegistry::Register(
                        TypeKey NativeType,
                        const Char* Name,
                        Size ByteSize,
                        Size Alignment,
                        Bool IsAbstract
                    )
                    {
                        if (Name == nullptr || Name[0] == '\0')
                            KernelAssert::Fail("Reflection type name cannot be empty");

                        if (Alignment == 0 || (Alignment & (Alignment - 1)) != 0)
                            KernelAssert::Fail("Reflection type alignment is invalid");

                        const KernelContainers::StringView NewName(Name);

                        KernelSync::LockGuard Lock(StateMutex);

                        const TypeId* ExistingType = NativeTypes.Find(NativeType);

                        if (ExistingType != nullptr)
                        {
                            const KernelMemory::SharedPtr<TypeInfo>* ExistingInfo = RegisteredTypes.Find(*ExistingType);

                            if (ExistingInfo == nullptr)
                                KernelAssert::Fail("Reflection registry is inconsistent");

                            if ((*ExistingInfo)->GetName() != NewName)
                                KernelAssert::Fail("Native type is already registered with another name");
                            if ((*ExistingInfo)->IsAbstract() != IsAbstract)
                                KernelAssert::Fail("Native type is already registered with another abstract/concrete state");

                            return (*ExistingInfo)->GetId();
                        }

                        const UInt32 NameHash = HashTypeName(NewName);
                        if (const TypeId* ExistingId = TypeHashes.Find(NameHash))
                        {
                            const KernelMemory::SharedPtr<TypeInfo>* ExistingInfo = RegisteredTypes.Find(*ExistingId);
                            if (ExistingInfo == nullptr)
                                KernelAssert::Fail("Reflection registry is inconsistent");
                            if ((*ExistingInfo)->GetName() == NewName)
                                KernelAssert::Fail("Reflection type name is already registered");
                            KernelAssert::Fail("Reflection type name hash collides with another registered type");
                        }

                        if (NextId == 0 || NextId == MaxValue<TypeId>())
                            KernelAssert::Fail("Reflection type id space is exhausted");

                        const TypeId Id = NextId;

                        KernelMemory::SharedPtr<TypeInfo> Info = KernelMemory::MakeShared<TypeInfo>(Id, Name, ByteSize, Alignment, NativeType, IsAbstract);

                        if (!RegisteredTypes.Insert(Id, Move(Info)))
                            KernelAssert::Fail("Reflection type id collision");

                        Bool NativeTypeInserted = false;
                        Bool TypeHashInserted = false;

                            NativeTypeInserted = NativeTypes.Insert(NativeType, Id);
                            if (!NativeTypeInserted)
                                KernelAssert::Fail("Reflection native type collision");

                            TypeHashInserted = TypeHashes.Insert(NameHash, Id);
                            if (!TypeHashInserted)
                                KernelAssert::Fail("Reflection type hash collision");


                        NextId++;
                        return Id;
                    }

                    const TypeInfo* TypeRegistry::Find(TypeId Id) const
                    {
                        KernelSync::LockGuard Lock(StateMutex);
                        const KernelMemory::SharedPtr<TypeInfo>* Info = RegisteredTypes.Find(Id);
                        return Info == nullptr ? nullptr : Info->Get();
                    }

                    const TypeInfo* TypeRegistry::FindByNativeType(TypeKey NativeType) const
                    {
                        KernelSync::LockGuard Lock(StateMutex);
                        const TypeId* Id = NativeTypes.Find(NativeType);
                        if (Id == nullptr)
                            return nullptr;
                        const KernelMemory::SharedPtr<TypeInfo>* Info = RegisteredTypes.Find(*Id);
                        return Info == nullptr ? nullptr : Info->Get();
                    }

                    const TypeInfo* TypeRegistry::FindByName(KernelContainers::StringView Name) const
                    {
                        KernelSync::LockGuard Lock(StateMutex);
                        const TypeId* Id = TypeHashes.Find(HashTypeName(Name));
                        if (Id == nullptr)
                            return nullptr;
                        const KernelMemory::SharedPtr<TypeInfo>* Info = RegisteredTypes.Find(*Id);
                        return Info != nullptr && (*Info)->GetName() == Name ? Info->Get() : nullptr;
                    }

                    const TypeInfo* TypeRegistry::FindByHash(UInt32 Hash) const
                    {
                        KernelSync::LockGuard Lock(StateMutex);
                        const TypeId* Id = TypeHashes.Find(Hash);
                        if (Id == nullptr)
                            return nullptr;
                        const KernelMemory::SharedPtr<TypeInfo>* Info = RegisteredTypes.Find(*Id);
                        return Info == nullptr ? nullptr : Info->Get();
                    }

                    KernelContainers::Array<const TypeInfo*> TypeRegistry::GetAllTypes() const
                    {
                        KernelSync::LockGuard Lock(StateMutex);
                        KernelContainers::Array<const TypeInfo*> Result;
                        Result.Reserve(RegisteredTypes.GetCount());
                        RegisteredTypes.ForEach(
                            [&](const TypeId&, const KernelMemory::SharedPtr<TypeInfo>& RegisteredInfo)
                            {
                                Result.Add(RegisteredInfo.Get());
                                return true;
                            }
                        );
                        return Result;
                    }

                    void TypeRegistry::Clear()
                    {
                        KernelSync::LockGuard Lock(StateMutex);
                        RegisteredTypes.Clear();
                        NativeTypes.Clear();
                        TypeHashes.Clear();
                        NextId = 1;
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
}
