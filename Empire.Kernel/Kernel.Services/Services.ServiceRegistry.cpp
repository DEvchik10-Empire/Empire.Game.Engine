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
                    decltype(RegisteredServices) Removed;

                    {
                        std::lock_guard Lock(Mutex);
                        Removed.swap(RegisteredServices);
                    }
                }

                Size ServiceRegistry::GetCount() const
                {
                    std::lock_guard Lock(Mutex);
                    return RegisteredServices.size();
                }
            }
        }
    }
}
