#include "Empire.Kernel/Kernel.Services/Services.ServiceRegistry.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        using namespace EmpireKernel::KernelTypes;

        namespace EmpireKernel
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
