
#pragma once

#include <memory>

#include <wayland-client.h>

#include "Interface.hpp"
#include "../../Empire.Window/Wayland/Surface.hpp"

namespace Engine
{
    namespace Platform
    {
        namespace Wayland
        {
            class Compositor : public Interface
            {
            private:
                wl_compositor* compositor = nullptr;

            public:
                explicit Compositor(wl_compositor* compositor);

                ~Compositor() override;

                Surface CreateSurface() const;

                wl_compositor* Get() const;
            };


            class CompositorBinder : public InterfaceBinder
            {
            public:
                explicit CompositorBinder(Types::u32 version);

                const wl_interface* GetInterfaceDescription() const override;
                std::unique_ptr<Interface> CreateInterface(void* data) override;
            };
        }
    }
}
