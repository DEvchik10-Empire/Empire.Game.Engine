#pragma once

#include "Empire.Kernel/Kernel.Types/Types.hpp"

#include <memory>
#include <mutex>
#include <stdexcept>
#include <typeindex>
#include <unordered_map>
#include <utility>

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
                        mutable std::mutex Mutex;
                        std::unordered_map<std::type_index, std::shared_ptr<void>> RegisteredServices;

                    public:
                        ServiceRegistry() = default;
                        ServiceRegistry(const ServiceRegistry&) = delete;
                        ServiceRegistry& operator=(const ServiceRegistry&) = delete;

                        template <typename ServiceType>
                        void Register(std::shared_ptr<ServiceType> Service)
                        {
                            if (!Service)
                                throw std::invalid_argument("Cannot register a null service");

                            std::lock_guard Lock(Mutex);
                            const std::type_index Type(typeid(ServiceType));

                            if (RegisteredServices.contains(Type))
                                throw std::logic_error("Service is already registered");

                            RegisteredServices.emplace(Type, std::move(Service));
                        }

                        template <typename ServiceType, typename... Arguments>
                        std::shared_ptr<ServiceType> Create(Arguments&&... Args)
                        {
                            auto Service = std::make_shared<ServiceType>(std::forward<Arguments>(Args)...);
                            Register<ServiceType>(Service);
                            return Service;
                        }

                        template <typename ServiceType>
                        std::shared_ptr<ServiceType> Get() const
                        {
                            std::lock_guard Lock(Mutex);
                            const auto It = RegisteredServices.find(std::type_index(typeid(ServiceType)));

                            if (It == RegisteredServices.end())
                                return nullptr;

                            return std::static_pointer_cast<ServiceType>(It->second);
                        }

                        template <typename ServiceType>
                        Types::Bool Contains() const
                        {
                            std::lock_guard Lock(Mutex);
                            return RegisteredServices.contains(std::type_index(typeid(ServiceType)));
                        }

                        template <typename ServiceType>
                        Types::Bool Remove()
                        {
                            std::shared_ptr<void> Removed;

                            {
                                std::lock_guard Lock(Mutex);
                                const auto It = RegisteredServices.find(std::type_index(typeid(ServiceType)));

                                if (It == RegisteredServices.end())
                                    return false;

                                Removed = std::move(It->second);
                                RegisteredServices.erase(It);
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
