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
        namespace EngineKernel
        {
            namespace Reflection
            {
                using TypeId = Types::UInt64;

                class TypeInfo
                {
                    private:
                        TypeId IdValue;
                        Containers::String NameValue;
                        Types::Size SizeValue;
                        Types::Size AlignmentValue;
                        Types::TypeKey NativeTypeValue;

                    public:
                        TypeInfo(
                            TypeId Id,
                            const Types::Char* Name,
                            Types::Size ByteSize,
                            Types::Size Alignment,
                            Types::TypeKey NativeType
                        );

                        TypeId GetId() const noexcept;
                        Containers::StringView GetName() const noexcept;
                        Types::Size GetSize() const noexcept;
                        Types::Size GetAlignment() const noexcept;
                        Types::TypeKey GetNativeType() const noexcept;
                };

                // Описания хранятся в отдельных блоках, поэтому указатели TypeInfo живут столько же, сколько реестр.
                class TypeRegistry
                {
                    private:
                        mutable Sync::Mutex StateMutex;
                        TypeId NextId = 1;
                        Containers::HashMap<TypeId, Memory::SharedPtr<TypeInfo>> RegisteredTypes;
                        Containers::HashMap<Types::TypeKey, TypeId> NativeTypes;

                    public:
                        TypeRegistry() = default;
                        TypeRegistry(const TypeRegistry&) = delete;
                        TypeRegistry& operator=(const TypeRegistry&) = delete;

                        template <typename T>
                        TypeId RegisterType(const Types::Char* Name)
                        {
                            return Register(
                                Types::GetTypeKey<T>(),
                                Name,
                                sizeof(T),
                                alignof(T)
                            );
                        }

                        TypeId Register(
                            Types::TypeKey NativeType,
                            const Types::Char* Name,
                            Types::Size ByteSize,
                            Types::Size Alignment
                        );

                        const TypeInfo* Find(TypeId Id) const;
                        const TypeInfo* FindByName(Containers::StringView Name) const;

                        template <typename T>
                        const TypeInfo* Find() const
                        {
                            Sync::LockGuard Lock(StateMutex);
                            const TypeId* Found = NativeTypes.Find(Types::GetTypeKey<T>());

                            if (Found == nullptr)
                                return nullptr;

                            const Memory::SharedPtr<TypeInfo>* Info = RegisteredTypes.Find(*Found);
                            return Info == nullptr ? nullptr : Info->Get();
                        }

                        Types::Size GetTypeCount() const;
                };
            }
        }
    }
}
