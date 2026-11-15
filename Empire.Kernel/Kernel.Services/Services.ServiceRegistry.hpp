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
        namespace EmpireKernel
        {
            namespace KernelServices
            {
                class ServiceRegistry
                {
                    private:
                        mutable KernelSync::Mutex StateMutex;
                        KernelContainers::HashMap<KernelTypes::TypeKey, KernelMemory::SharedPtr<void>> RegisteredServices;

                    public:
                        ServiceRegistry() = default;
                        ServiceRegistry(const ServiceRegistry&) = delete;
                        ServiceRegistry& operator=(const ServiceRegistry&) = delete;

                        template <typename ServiceType>
                        void Register(KernelMemory::SharedPtr<ServiceType> Service)
                        {
                            if (!Service)
                                throw KernelAssert::InvalidArgumentError("Cannot register a null service");

                            KernelSync::LockGuard Lock(StateMutex);
                            const KernelTypes::TypeKey Key = KernelTypes::GetTypeKey<ServiceType>();

                            if (RegisteredServices.Contains(Key))
                                throw KernelAssert::LogicError("Service is already registered");

                            RegisteredServices.Insert(Key, KernelMemory::SharedPtr<void>(KernelTypes::Move(Service)));
                        }

                        template <typename ServiceType, typename... Arguments>
                        KernelMemory::SharedPtr<ServiceType> Create(Arguments&&... Args)
                        {
                            KernelMemory::SharedPtr<ServiceType> Service =
                                KernelMemory::MakeShared<ServiceType>(KernelTypes::Forward<Arguments>(Args)...);

                            Register<ServiceType>(Service);
                            return Service;
                        }

                        template <typename ServiceType>
                        KernelMemory::SharedPtr<ServiceType> Get() const
                        {
                            KernelSync::LockGuard Lock(StateMutex);
                            const KernelMemory::SharedPtr<void>* Found = RegisteredServices.Find(KernelTypes::GetTypeKey<ServiceType>());

                            if (Found == nullptr)
                                return nullptr;

                            return KernelMemory::StaticPointerCast<ServiceType>(*Found);
                        }

                        template <typename ServiceType>
                        KernelTypes::Bool Contains() const
                        {
                            KernelSync::LockGuard Lock(StateMutex);
                            return RegisteredServices.Contains(KernelTypes::GetTypeKey<ServiceType>());
                        }

                        template <typename ServiceType>
                        KernelTypes::Bool Remove()
                        {
                            KernelMemory::SharedPtr<void> Removed;

                            {
                                KernelSync::LockGuard Lock(StateMutex);
                                const KernelTypes::TypeKey Key = KernelTypes::GetTypeKey<ServiceType>();
                                KernelMemory::SharedPtr<void>* Found = RegisteredServices.Find(Key);

                                if (Found == nullptr)
                                    return false;

                                Removed = KernelTypes::Move(*Found);
                                RegisteredServices.Remove(Key);
                            }

                            return true;
                        }

                        void Clear();
                        KernelTypes::Size GetCount() const;
                };
            }
        }
    }
}
