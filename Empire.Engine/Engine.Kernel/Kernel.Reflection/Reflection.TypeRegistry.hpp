#pragma once

#include "Empire.Engine/Engine.Kernel/Kernel.Assert/Assert.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Containers/Containers.Array.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Containers/Containers.HashMap.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Containers/Containers.String.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Containers/Containers.StringView.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Memory/Memory.SharedPtr.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Sync/Sync.Mutex.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Types/Types.Utility.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireEngine
        {
            namespace EngineKernel
            {
                namespace KernelReflection
                {
                    using TypeId = KernelTypes::UInt64;

                    // Описание члена класса: имя, доступ к значению по указателю на объект, размер и ключ типа значения.
                    // Аналог атрибутов Jolt RTTI (AddAttribute/GetAttribute), без макросов.
                    struct TypeAttribute
                    {
                        KernelContainers::String Name;
                        void* (*Access)(void*) = nullptr;
                        KernelTypes::Size ValueSize = 0;
                        KernelTypes::TypeKey ValueType = 0;

                        [[nodiscard]] void* GetValue(void* Object) const noexcept
                        {
                            return Access(Object);
                        }

                        [[nodiscard]] const void* GetValue(const void* Object) const noexcept
                        {
                            return Access(const_cast<void*>(Object));
                        }
                    };

                    template <typename Pointer>
                    struct MemberPointerTraits;

                    template <typename Class, typename Value>
                    struct MemberPointerTraits<Value Class::*>
                    {
                        using ClassType = Class;
                        using ValueType = Value;
                    };

                    class TypeInfo
                    {
                        private:
                            struct BaseType
                            {
                                const TypeInfo* Info;
                                const void* (*Cast)(const void*);
                            };

                        private:
                            TypeId IdValue;
                            KernelContainers::String NameValue;
                            KernelTypes::Size SizeValue;
                            KernelTypes::Size AlignmentValue;
                            KernelTypes::TypeKey NativeTypeValue;
                            KernelTypes::Bool AbstractValue = false;
                            KernelContainers::Array<BaseType> BaseTypes;
                            KernelContainers::Array<TypeAttribute> Attributes;

                        public:
                            TypeInfo(
                                TypeId Id,
                                const KernelTypes::Char* Name,
                                KernelTypes::Size ByteSize,
                                KernelTypes::Size Alignment,
                                KernelTypes::TypeKey NativeType,
                                KernelTypes::Bool IsAbstract
                            );

                            TypeId GetId() const noexcept;
                            KernelContainers::StringView GetName() const noexcept;
                            KernelTypes::UInt32 GetHash() const noexcept;
                            KernelTypes::Size GetSize() const noexcept;
                            KernelTypes::Size GetAlignment() const noexcept;
                            KernelTypes::TypeKey GetNativeType() const noexcept;
                            [[nodiscard]] KernelTypes::Bool IsAbstract() const noexcept;
                            KernelTypes::Size GetBaseClassCount() const noexcept;
                            const TypeInfo* GetBaseClass(KernelTypes::Size Index) const noexcept;

                            KernelTypes::Bool operator==(const TypeInfo& Other) const noexcept;
                            KernelTypes::Bool operator!=(const TypeInfo& Other) const noexcept;

                            KernelTypes::Size GetAttributeCount() const noexcept;
                            const TypeAttribute* GetAttribute(KernelTypes::Size Index) const noexcept;
                            const TypeAttribute* FindAttribute(KernelContainers::StringView Name) const noexcept;

                            // Добавляет атрибут-член по указателю на член класса: AddAttribute<&Body::Mass>("Mass").
                            // Повторное имя или пустое имя отклоняются (false). Вызывать при регистрации, до параллельного использования типа.
                            template <auto Member>
                            KernelTypes::Bool AddAttribute(KernelContainers::StringView Name)
                            {
                                using Traits = MemberPointerTraits<decltype(Member)>;
                                return AddAttributeInternal(
                                    Name,
                                    [](void* Object) -> void*
                                    {
                                        return &(static_cast<typename Traits::ClassType*>(Object)->*Member);
                                    },
                                    sizeof(typename Traits::ValueType),
                                    KernelTypes::GetTypeKey<typename Traits::ValueType>()
                                );
                            }

                            KernelTypes::Bool IsKindOf(const TypeInfo& Type) const noexcept;
                            const void* CastTo(const void* Object, const TypeInfo& Type) const noexcept;

                        private:
                            KernelTypes::Bool AddAttributeInternal(
                                KernelContainers::StringView Name,
                                void* (*Access)(void*),
                                KernelTypes::Size ValueSize,
                                KernelTypes::TypeKey ValueType
                            );

                        public:
                            template <typename Derived, typename Base>
                            void AddBase(const TypeInfo& BaseInfo)
                            {
                                BaseTypes.EmplaceBack(BaseType{
                                    &BaseInfo,
                                    [](const void* Object) -> const void*
                                    {
                                        return static_cast<const Base*>(static_cast<const Derived*>(Object));
                                    }
                                });
                            }
                    };

                    // Проверка и приведение типов по явно переданному динамическому описанию.
                    // В отличие от compiler RTTI, это не требует включения RTTI в компиляторе.
                    [[nodiscard]] inline KernelTypes::Bool IsType(const TypeInfo& Actual, const TypeInfo& Expected) noexcept
                    {
                        return Actual == Expected;
                    }

                    [[nodiscard]] inline KernelTypes::Bool IsKindOf(const TypeInfo& Actual, const TypeInfo& Expected) noexcept
                    {
                        return Actual.IsKindOf(Expected);
                    }

                    template <typename T>
                    [[nodiscard]] T* DynamicCast(void* Object, const TypeInfo& Actual, const TypeInfo& Expected) noexcept
                    {
                        return static_cast<T*>(const_cast<void*>(Actual.CastTo(Object, Expected)));
                    }

                    template <typename T>
                    [[nodiscard]] const T* DynamicCast(const void* Object, const TypeInfo& Actual, const TypeInfo& Expected) noexcept
                    {
                        return static_cast<const T*>(Actual.CastTo(Object, Expected));
                    }

                    // Описания хранятся в отдельных блоках, поэтому указатели TypeInfo живут столько же, сколько реестр.
                    class TypeRegistry
                    {
                        private:
                            mutable KernelSync::Mutex StateMutex;
                            TypeId NextId = 1;
                            KernelContainers::HashMap<TypeId, KernelMemory::SharedPtr<TypeInfo>> RegisteredTypes;
                            KernelContainers::HashMap<KernelTypes::TypeKey, TypeId> NativeTypes;
                            KernelContainers::HashMap<KernelTypes::UInt32, TypeId> TypeHashes;

                        public:
                            TypeRegistry() = default;
                            TypeRegistry(const TypeRegistry&) = delete;
                            TypeRegistry& operator=(const TypeRegistry&) = delete;

                            template <typename T>
                            TypeId RegisterType(const KernelTypes::Char* Name, KernelTypes::Bool IsAbstract = false)
                            {
                                return Register(
                                    KernelTypes::GetTypeKey<T>(),
                                    Name,
                                    sizeof(T),
                                    alignof(T),
                                    IsAbstract
                                );
                            }

                            // Регистрация идемпотентна: повторный вызов для того же типа с тем же именем возвращает уже выданный TypeId.
                            // Тот же тип под другим именем, другой тип под занятым именем, пустое имя и неверное выравнивание бросают исключение.
                            TypeId Register(
                                KernelTypes::TypeKey NativeType,
                                const KernelTypes::Char* Name,
                                KernelTypes::Size ByteSize,
                                KernelTypes::Size Alignment,
                                KernelTypes::Bool IsAbstract = false
                            );

                            const TypeInfo* Find(TypeId Id) const;
                            const TypeInfo* FindByNativeType(KernelTypes::TypeKey NativeType) const;
                            const TypeInfo* FindByName(KernelContainers::StringView Name) const;
                            const TypeInfo* FindByHash(KernelTypes::UInt32 Hash) const;
                            KernelContainers::Array<const TypeInfo*> GetAllTypes() const;
                            void Clear();

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

                            template <typename Derived, typename Base>
                            KernelTypes::Bool RegisterBase()
                            {
                                KernelSync::LockGuard Lock(StateMutex);
                                const TypeId* DerivedId = NativeTypes.Find(KernelTypes::GetTypeKey<Derived>());
                                const TypeId* BaseId = NativeTypes.Find(KernelTypes::GetTypeKey<Base>());
                                if (DerivedId == nullptr || BaseId == nullptr)
                                    return false;
                                const KernelMemory::SharedPtr<TypeInfo>* DerivedInfo = RegisteredTypes.Find(*DerivedId);
                                const KernelMemory::SharedPtr<TypeInfo>* BaseInfo = RegisteredTypes.Find(*BaseId);
                                if (DerivedInfo == nullptr || BaseInfo == nullptr || DerivedInfo == BaseInfo)
                                    return false;

                                // Не допускаем циклы в графе наследования: IsKindOf/CastTo обходят его рекурсивно.
                                if ((*BaseInfo)->IsKindOf(**DerivedInfo))
                                    return false;

                                // Повторная регистрация именно прямой связи идемпотентна; транзитивные связи не заменяют direct-base metadata.
                                for (KernelTypes::Size Index = 0; Index < (*DerivedInfo)->GetBaseClassCount(); ++Index)
                                {
                                    if ((*DerivedInfo)->GetBaseClass(Index) == BaseInfo->Get())
                                        return true;
                                }

                                (*DerivedInfo)->template AddBase<Derived, Base>(**BaseInfo);
                                return true;
                            }

                            KernelTypes::Size GetTypeCount() const;
                    };
                }
            }
        }
    }
}
