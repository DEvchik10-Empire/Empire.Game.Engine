#pragma once

#include "Empire.Engine/Engine.Kernel/Kernel.Containers/Containers.HashMap.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Memory/Memory.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Reflection/Reflection.TypeRegistry.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Sync/Sync.Mutex.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Types/Types.NonCopyable.hpp"
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
                    // Типизированная регистрация и создание объектов (перенесено по смыслу из Jolt 5.7.0, Core/Factory.h).
                    // Вместо RTTI-макросов используются явные RegisterType<T> вызовы.
                    class Factory : public KernelTypes::NonCopyable
                    {
                        private:
                            using CreateFunction = void* (*)();
                            using DestroyFunction = void (*)(void*) noexcept;

                            struct TypeFunctions
                            {
                                CreateFunction Create = nullptr;
                                DestroyFunction Destroy = nullptr;
                            };

                            mutable KernelSync::Mutex StateMutex;
                            TypeRegistry Types;
                            KernelContainers::HashMap<KernelTypes::TypeKey, TypeFunctions> RegisteredTypes;

                            template <typename T>
                            static void* CreateInstance()
                            {
                                void* Storage = KernelMemory::Allocate(sizeof(T), alignof(T));

                                    return KernelTypes::ConstructAt(static_cast<T*>(Storage));

                            }

                            template <typename T>
                            static void DestroyInstance(void* Instance) noexcept
                            {
                                KernelMemory::Destroy(static_cast<T*>(Instance));
                            }

                        public:
                            class Object : public KernelTypes::NonCopyable
                            {
                                private:
                                    void* Pointer = nullptr;
                                    DestroyFunction Destroy = nullptr;

                                    Object(void* Value, DestroyFunction Destroyer) noexcept : Pointer(Value), Destroy(Destroyer) {}
                                    friend class Factory;

                                public:
                                    Object() = default;
                                    Object(Object&& Other) noexcept : Pointer(Other.Pointer), Destroy(Other.Destroy)
                                    {
                                        Other.Pointer = nullptr;
                                        Other.Destroy = nullptr;
                                    }
                                    Object& operator=(Object&& Other) noexcept
                                    {
                                        if (this != &Other)
                                        {
                                            Reset();
                                            Pointer = Other.Pointer;
                                            Destroy = Other.Destroy;
                                            Other.Pointer = nullptr;
                                            Other.Destroy = nullptr;
                                        }
                                        return *this;
                                    }
                                    ~Object() { Reset(); }

                                    void Reset() noexcept
                                    {
                                        if (Pointer != nullptr)
                                            Destroy(Pointer);
                                        Pointer = nullptr;
                                        Destroy = nullptr;
                                    }

                                    [[nodiscard]] explicit operator KernelTypes::Bool() const noexcept { return Pointer != nullptr; }
                                    [[nodiscard]] void* Get() noexcept { return Pointer; }
                                    [[nodiscard]] const void* Get() const noexcept { return Pointer; }

                                    template <typename T>
                                    [[nodiscard]] T* As() noexcept { return static_cast<T*>(Pointer); }

                                    template <typename T>
                                    [[nodiscard]] const T* As() const noexcept { return static_cast<const T*>(Pointer); }
                            };

                            template <typename T>
                                requires requires { T{}; }
                            TypeId RegisterType(const KernelTypes::Char* Name)
                            {
                                KernelSync::LockGuard Lock(StateMutex);
                                const TypeId Id = Types.RegisterType<T>(Name);
                                const TypeFunctions Functions{&CreateInstance<T>, &DestroyInstance<T>};
                                RegisteredTypes.Set(KernelTypes::GetTypeKey<T>(), Functions);
                                return Id;
                            }

                            template <typename T>
                            TypeId RegisterAbstractType(const KernelTypes::Char* Name)
                            {
                                KernelSync::LockGuard Lock(StateMutex);
                                return Types.RegisterType<T>(Name, true);
                            }

                            template <typename Derived, typename Base>
                            KernelTypes::Bool RegisterBase()
                            {
                                KernelSync::LockGuard Lock(StateMutex);
                                return Types.template RegisterBase<Derived, Base>();
                            }

                            [[nodiscard]] Object Create(KernelTypes::TypeKey Type) const
                            {
                                TypeFunctions Functions;
                                {
                                    KernelSync::LockGuard Lock(StateMutex);
                                    const TypeFunctions* Found = RegisteredTypes.Find(Type);
                                    if (Found == nullptr)
                                        return Object();
                                    Functions = *Found;
                                }
                                return Object(Functions.Create(), Functions.Destroy);
                            }

                            [[nodiscard]] Object Create(const TypeInfo& Type) const
                            {
                                TypeFunctions Functions;
                                {
                                    KernelSync::LockGuard Lock(StateMutex);
                                    const TypeFunctions* Found = RegisteredTypes.Find(Type.GetNativeType());
                                    if (Found == nullptr)
                                        return Object();
                                    Functions = *Found;
                                }
                                return Object(Functions.Create(), Functions.Destroy);
                            }

                            [[nodiscard]] Object Create(KernelContainers::StringView Name) const
                            {
                                TypeFunctions Functions;
                                {
                                    KernelSync::LockGuard Lock(StateMutex);
                                    const TypeInfo* Type = Types.FindByName(Name);
                                    if (Type == nullptr)
                                        return Object();
                                    const TypeFunctions* Found = RegisteredTypes.Find(Type->GetNativeType());
                                    if (Found == nullptr)
                                        return Object();
                                    Functions = *Found;
                                }
                                return Object(Functions.Create(), Functions.Destroy);
                            }

                            [[nodiscard]] Object CreateByHash(KernelTypes::UInt32 Hash) const
                            {
                                TypeFunctions Functions;
                                {
                                    KernelSync::LockGuard Lock(StateMutex);
                                    const TypeInfo* Type = Types.FindByHash(Hash);
                                    if (Type == nullptr)
                                        return Object();
                                    const TypeFunctions* Found = RegisteredTypes.Find(Type->GetNativeType());
                                    if (Found == nullptr)
                                        return Object();
                                    Functions = *Found;
                                }
                                return Object(Functions.Create(), Functions.Destroy);
                            }

                            [[nodiscard]] const TypeInfo* Find(TypeId Id) const
                            {
                                return Types.Find(Id);
                            }

                            [[nodiscard]] KernelTypes::Bool IsAbstract(const TypeInfo& Type) const noexcept
                            {
                                return Type.IsAbstract();
                            }

                            [[nodiscard]] const TypeInfo* Find(KernelContainers::StringView Name) const
                            {
                                return Types.FindByName(Name);
                            }

                            [[nodiscard]] const TypeInfo* FindByNativeType(KernelTypes::TypeKey Type) const
                            {
                                return Types.FindByNativeType(Type);
                            }

                            [[nodiscard]] const TypeInfo* FindByHash(KernelTypes::UInt32 Hash) const
                            {
                                return Types.FindByHash(Hash);
                            }

                            [[nodiscard]] KernelContainers::Array<const TypeInfo*> GetAllClasses() const
                            {
                                return Types.GetAllTypes();
                            }

                            void Clear()
                            {
                                KernelSync::LockGuard Lock(StateMutex);
                                RegisteredTypes.Clear();
                                Types.Clear();
                            }

                            [[nodiscard]] KernelTypes::Size GetTypeCount() const
                            {
                                return Types.GetTypeCount();
                            }
                    };
                }
            }
        }
    }
}
