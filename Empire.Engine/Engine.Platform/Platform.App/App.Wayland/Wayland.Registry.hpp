#pragma once

#include "Empire.Engine/Engine.Kernel/Kernel.Containers/Containers.Array.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Types/Types.hpp"
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
                        using namespace EngineKernel::KernelTypes;

                        class Display;

                        class Registry final
                        {
                            private:
                                wl_display* NativeDisplay = nullptr;
                                wl_registry* NativeRegistry = nullptr;
                                EngineKernel::KernelContainers::Array<InterfaceBinder*> Binders;
                                Int32 ErrorCode = 0;

                                [[nodiscard]] Bool BindInterface(
                                    UInt32 GlobalName,
                                    const Char* InterfaceName,
                                    UInt32 AdvertisedVersion
                                ) noexcept;

                                void UnbindInterface(UInt32 GlobalName) noexcept;

                                static void Global(
                                    void* UserData,
                                    wl_registry* NativeRegistry,
                                    uint32_t GlobalName,
                                    const Char* InterfaceName,
                                    uint32_t AdvertisedVersion
                                );

                                static void GlobalRemove(
                                    void* UserData,
                                    wl_registry* NativeRegistry,
                                    uint32_t GlobalName
                                );

                                static const wl_registry_listener Listener;

                            public:
                                Registry() noexcept = default;
                                Registry(const Registry&) = delete;
                                Registry& operator=(const Registry&) = delete;
                                Registry(Registry&&) = delete;
                                Registry& operator=(Registry&&) = delete;
                                ~Registry();

                                // Borrows the connection; Display must outlive this registry.
                                [[nodiscard]] Bool Initialize(Display& WaylandDisplay) noexcept;
                                void Shutdown() noexcept;

                                // Register from the Wayland event thread during backend setup.
                                // This blocks for a roundtrip and may dispatch queued events.
                                [[nodiscard]] Bool RegisterBinder(InterfaceBinder& Binder) noexcept;
                                [[nodiscard]] Bool UnregisterBinder(InterfaceBinder& Binder) noexcept;

                                [[nodiscard]] Bool IsReady() const noexcept;
                                [[nodiscard]] Int32 GetErrorCode() const noexcept;
                                [[nodiscard]] Size GetBinderCount() const noexcept;
                        };
                    }
                }
            }
        }
    }
}
