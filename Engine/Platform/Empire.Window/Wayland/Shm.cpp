#include "Shm.hpp"

namespace Engine
{
    namespace Platform
    {
        namespace Wayland
        {
            Shm::Shm(
                wl_shm* shm
            )
                : shm(shm)
            {
            }


            Shm::~Shm()
            {
                if (shm)
                {
                    wl_shm_destroy(shm);
                    shm = nullptr;
                }
            }


            ShmPool Shm::CreatePool()
            {
                return ShmPool(shm);
            }


            wl_shm* Shm::Get() const
            {
                return shm;
            }


            ShmBinder::ShmBinder(
                Types::u32 version
            )
                : InterfaceBinder(version)
            {
            }


            const wl_interface*
            ShmBinder::GetInterfaceDescription() const
            {
                return &wl_shm_interface;
            }


            std::unique_ptr<Interface>
            ShmBinder::CreateInterface(
                void* data
            )
            {
                return std::unique_ptr<Interface>(
                    new Shm(
                        static_cast<wl_shm*>(data)
                    )
                );
            }
        }
    }
}