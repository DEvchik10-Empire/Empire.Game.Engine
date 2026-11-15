#pragma once

#include <memory>
#include <wayland-client.h>

#include "../../Empire.App/Wayland/Interface.hpp"
#include "ShmPool.hpp"

namespace Engine
{
    namespace Platform
    {
        namespace Wayland
        {
            class Shm : public Interface
            {
            private:
                wl_shm* shm = nullptr;

            public:
                explicit Shm(
                    wl_shm* shm
                );

                ~Shm() override;

                ShmPool CreatePool();

                wl_shm* Get() const;
            };


            class ShmBinder : public InterfaceBinder
            {
            public:
                explicit ShmBinder(
                    Types::u32 version
                );

                const wl_interface*
                GetInterfaceDescription() const override;

                std::unique_ptr<Interface>
                CreateInterface(
                    void* data
                ) override;
            };
        }
    }
}