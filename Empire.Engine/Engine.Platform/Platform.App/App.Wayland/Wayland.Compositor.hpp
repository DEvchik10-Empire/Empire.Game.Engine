#pragma once

#include "Empire.Engine/Engine.Platform/Platform.App/App.Wayland/Wayland.Interface.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireEngine
        {
            namespace EnginePlatform
            {
                namespace PlatformApp
                {
                    namespace AppWayland
                    {
                        class Compositor final : public Interface
                        {
                            private:
                                wl_compositor* NativeCompositor = nullptr;

                            public:
                                explicit Compositor(wl_compositor* NativeCompositor) noexcept;
                                ~Compositor() override;

                                Compositor(const Compositor&) = delete;
                                Compositor& operator=(const Compositor&) = delete;
                                Compositor(Compositor&&) = delete;
                                Compositor& operator=(Compositor&&) = delete;

                                [[nodiscard]] wl_compositor* Get() const noexcept;
                        };

                        class CompositorBinder final : public InterfaceBinder
                        {
                            public:
                                explicit CompositorBinder(UInt32 MaximumVersion) noexcept;

                                [[nodiscard]] const wl_interface* GetInterfaceDescription() const noexcept override;
                                [[nodiscard]] EngineKernel::KernelMemory::SharedPtr<Interface> CreateInterface(
                                    void* NativeInterface
                                ) override;

                                [[nodiscard]] Compositor* GetCompositor() const noexcept;
                        };
                    }
                }
            }
        }
    }
}
