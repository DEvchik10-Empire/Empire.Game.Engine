#pragma once

#include "Empire.Kernel/Kernel.Assert/Assert.hpp"
#include "Empire.Kernel/Kernel.Containers/Containers.HashMap.hpp"
#include "Empire.Kernel/Kernel.Memory/Memory.SharedPtr.hpp"
#include "Empire.Kernel/Kernel.Sync/Sync.Mutex.hpp"
#include "Empire.Kernel/Kernel.Types/Types.Utility.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EngineKernel
        {
            namespace Services
            {
                class ServiceRegistry
                {
                    private:
                        mutable Sync::Mutex StateMutex;
                        Containers::HashMap<Types::TypeKey, Memory::SharedPtr<void>> RegisteredServices;

                    public:
                        ServiceRegistry() = default;
                        ServiceRegistry(const ServiceRegistry&) = delete;
                        ServiceRegistry& operator=(const ServiceRegistry&) = delete;

                        template <typename ServiceType>
                        void Register(Memory::SharedPtr<ServiceType> Service)
                        {
                            if (!Service)
                                throw Assert::InvalidArgumentError("Cannot register a null service");

                            Sync::LockGuard Lock(StateMutex);
                            const Types::TypeKey Key = Types::GetTypeKey<ServiceType>();

                            if (RegisteredServices.Contains(Key))
                                throw Assert::LogicError("Service is already registered");

                            RegisteredServices.Insert(Key, Memory::SharedPtr<void>(Types::Move(Service)));
                        }

                        template <typename ServiceType, typename... Arguments>
                        Memory::SharedPtr<ServiceType> Create(Arguments&&... Args)
                        {
                            Memory::SharedPtr<ServiceType> Service =
                                Memory::MakeShared<ServiceType>(Types::Forward<Arguments>(Args)...);

                            Register<ServiceType>(Service);
                            return Service;
                        }

                        template <typename ServiceType>
                        Memory::SharedPtr<ServiceType> Get() const
                        {
                            Sync::LockGuard Lock(StateMutex);
                            const Memory::SharedPtr<void>* Found = RegisteredServices.Find(Types::GetTypeKey<ServiceType>());

                            if (Found == nullptr)
                                return nullptr;

                            return Memory::StaticPointerCast<ServiceType>(*Found);
                        }

                        template <typename ServiceType>
                        Types::Bool Contains() const
                        {
                            Sync::LockGuard Lock(StateMutex);
                            return RegisteredServices.Contains(Types::GetTypeKey<ServiceType>());
                        }

                        template <typename ServiceType>
                        Types::Bool Remove()
                        {
                            Memory::SharedPtr<void> Removed;

                            {
                                Sync::LockGuard Lock(StateMutex);
                                const Types::TypeKey Key = Types::GetTypeKey<ServiceType>();
                                Memory::SharedPtr<void>* Found = RegisteredServices.Find(Key);

                                if (Found == nullptr)
                                    return false;

                                Removed = Types::Move(*Found);
                                RegisteredServices.Remove(Key);
                            }

                            return true;
                        }

                        void Clear();
                        Types::Size GetCount() const;
                };
            }
        }
    }
}
