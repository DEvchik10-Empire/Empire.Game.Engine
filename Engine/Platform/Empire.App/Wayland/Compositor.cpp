
#include "Compositor.hpp"

namespace Engine
{
    namespace Platform
    {
        namespace Wayland
        {
            Compositor::Compositor(wl_compositor* compositor)
                : compositor(compositor)
            {
            }

            Compositor::~Compositor()
            {
                if (compositor)
                {
                    wl_compositor_destroy(compositor);
                }
            }

            Surface Compositor::CreateSurface() const
            {
                return Surface(
                    wl_compositor_create_surface(compositor)
                );
            }

            wl_compositor* Compositor::Get() const
            {
                return compositor;
            }


            CompositorBinder::CompositorBinder(Types::u32 version)
                : InterfaceBinder(version)
            {
            }

            const wl_interface* CompositorBinder::GetInterfaceDescription() const
            {
                return &wl_compositor_interface;
            }

            std::unique_ptr<Interface> CompositorBinder::CreateInterface(void* data)
            {
                return std::unique_ptr<Interface>(
                    new Compositor(
                        static_cast<wl_compositor*>(data)
                    )
                );
            }
        }
    }
}
