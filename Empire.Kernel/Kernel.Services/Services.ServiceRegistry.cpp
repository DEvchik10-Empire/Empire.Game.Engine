#include "Empire.Kernel/Kernel.Services/Services.ServiceRegistry.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        using namespace EngineKernel::Types;

        namespace EngineKernel
        {
            namespace Services
            {
                void ServiceRegistry::Clear()
                {
                    Containers::HashMap<TypeKey, Memory::SharedPtr<void>> Removed;

                    {
                        Sync::LockGuard Lock(StateMutex);
                        Removed = Move(RegisteredServices);
                    }
                }

                Size ServiceRegistry::GetCount() const
                {
                    Sync::LockGuard Lock(StateMutex);
                    return RegisteredServices.GetCount();
                }
            }
        }
    }
}
