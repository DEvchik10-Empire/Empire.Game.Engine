#include "Empire.Engine/Engine.Kernel/Kernel.Services/Services.ServiceRegistry.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        using namespace EmpireEngine::EngineKernel::KernelTypes;

        namespace EmpireEngine
        {
            namespace EngineKernel
            {
                namespace KernelServices
                {
                    void ServiceRegistry::Clear()
                    {
                        KernelContainers::HashMap<TypeKey, KernelMemory::SharedPtr<void>> Removed;

                        {
                            KernelSync::LockGuard Lock(StateMutex);
                            Removed = Move(RegisteredServices);
                        }
                    }

                    Size ServiceRegistry::GetCount() const
                    {
                        KernelSync::LockGuard Lock(StateMutex);
                        return RegisteredServices.GetCount();
                    }
                }
            }
        }
    }
}
