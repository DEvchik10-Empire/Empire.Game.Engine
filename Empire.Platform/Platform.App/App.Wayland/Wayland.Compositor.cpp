#include "Empire.Platform/Platform.App/App.Wayland/Wayland.Compositor.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpirePlatform
        {
            namespace PlatformApp
            {
                namespace AppWayland
                {
                    Compositor::Compositor(wl_compositor* NativeCompositor) noexcept
                        : NativeCompositor(NativeCompositor)
                    {
                    }

                    Compositor::~Compositor()
                    {
                        if (NativeCompositor != nullptr)
                        {
                            wl_compositor_destroy(NativeCompositor);
                            NativeCompositor = nullptr;
                        }
                    }

                    wl_compositor* Compositor::Get() const noexcept
                    {
                        return NativeCompositor;
                    }

                    CompositorBinder::CompositorBinder(UInt32 MaximumVersion) noexcept
                        : InterfaceBinder(MaximumVersion)
                    {
                    }

                    const wl_interface* CompositorBinder::GetInterfaceDescription() const noexcept
                    {
                        return &wl_compositor_interface;
                    }

                    EmpireKernel::KernelMemory::SharedPtr<Interface> CompositorBinder::CreateInterface(
                        void* NativeInterface
                    )
                    {
                        if (NativeInterface == nullptr)
                            return nullptr;

                        return EmpireKernel::KernelMemory::MakeShared<Compositor>(
                            static_cast<wl_compositor*>(NativeInterface)
                        );
                    }

                    Compositor* CompositorBinder::GetCompositor() const noexcept
                    {
                        return static_cast<Compositor*>(GetFirstInterface());
                    }
                }
            }
        }
    }
}
