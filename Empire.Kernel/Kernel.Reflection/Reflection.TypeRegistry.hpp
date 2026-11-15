#pragma once

#include "Empire.Kernel/Kernel.Assert/Assert.hpp"
#include "Empire.Kernel/Kernel.Containers/Containers.HashMap.hpp"
#include "Empire.Kernel/Kernel.Containers/Containers.String.hpp"
#include "Empire.Kernel/Kernel.Containers/Containers.StringView.hpp"
#include "Empire.Kernel/Kernel.Memory/Memory.SharedPtr.hpp"
#include "Empire.Kernel/Kernel.Sync/Sync.Mutex.hpp"
#include "Empire.Kernel/Kernel.Types/Types.Utility.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireKernel
        {
            namespace KernelReflection
            {
                using TypeId = KernelTypes::UInt64;

                class TypeInfo
                {
                    private:
                        TypeId IdValue;
                        KernelContainers::String NameValue;
                        KernelTypes::Size SizeValue;
                        KernelTypes::Size AlignmentValue;
                        KernelTypes::TypeKey NativeTypeValue;

                    public:
                        TypeInfo(
                            TypeId Id,
                            const KernelTypes::Char* Name,
                            KernelTypes::Size ByteSize,
                            KernelTypes::Size Alignment,
                            KernelTypes::TypeKey NativeType
                        );

                        TypeId GetId() const noexcept;
                        KernelContainers::StringView GetName() const noexcept;
                        KernelTypes::Size GetSize() const noexcept;
                        KernelTypes::Size GetAlignment() const noexcept;
                        KernelTypes::TypeKey GetNativeType() const noexcept;
                };

                // Описания хранятся в отдельных блоках, поэтому указатели TypeInfo живут столько же, сколько реестр.
                class TypeRegistry
                {
                    private:
                        mutable KernelSync::Mutex StateMutex;
                        TypeId NextId = 1;
                        KernelContainers::HashMap<TypeId, KernelMemory::SharedPtr<TypeInfo>> RegisteredTypes;
                        KernelContainers::HashMap<KernelTypes::TypeKey, TypeId> NativeTypes;

                    public:
                        TypeRegistry() = default;
                        TypeRegistry(const TypeRegistry&) = delete;
                        TypeRegistry& operator=(const TypeRegistry&) = delete;

                        template <typename T>
                        TypeId RegisterType(const KernelTypes::Char* Name)
                        {
                            return Register(
                                KernelTypes::GetTypeKey<T>(),
                                Name,
                                sizeof(T),
                                alignof(T)
                            );
                        }

                        // Регистрация идемпотентна: повторный вызов для того же типа с тем же именем возвращает уже выданный TypeId.
                        // Тот же тип под другим именем, другой тип под занятым именем, пустое имя и неверное выравнивание бросают исключение.
                        TypeId Register(
                            KernelTypes::TypeKey NativeType,
                            const KernelTypes::Char* Name,
                            KernelTypes::Size ByteSize,
                            KernelTypes::Size Alignment
                        );

                        const TypeInfo* Find(TypeId Id) const;
                        const TypeInfo* FindByName(KernelContainers::StringView Name) const;

                        template <typename T>
                        const TypeInfo* Find() const
                        {
                            KernelSync::LockGuard Lock(StateMutex);
                            const TypeId* Found = NativeTypes.Find(KernelTypes::GetTypeKey<T>());

                            if (Found == nullptr)
                                return nullptr;

                            const KernelMemory::SharedPtr<TypeInfo>* Info = RegisteredTypes.Find(*Found);
                            return Info == nullptr ? nullptr : Info->Get();
                        }

                        KernelTypes::Size GetTypeCount() const;
                };
            }
        }
    }
}
