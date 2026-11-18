#pragma once

#include "Empire.Engine/Engine.Kernel/Kernel.Containers/Containers.Array.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Memory/Memory.SharedPtr.hpp"
#include "Empire.Engine/Engine.Kernel/Kernel.Types/Types.hpp"
#include "Empire.Engine/Engine.Platform/Platform.ThirdParty/ThirdParty.Wayland/Wayland.hpp"

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

                        class InterfaceBinder;
                        class Registry;

                        class Interface
                        {
                            private:
                                InterfaceBinder* OwnerBinder = nullptr;
                                UInt32 GlobalName = 0;

                                friend class InterfaceBinder;

                            public:
                                Interface() noexcept = default;
                                Interface(const Interface&) = delete;
                                Interface& operator=(const Interface&) = delete;
                                Interface(Interface&&) = delete;
                                Interface& operator=(Interface&&) = delete;

                                virtual ~Interface() = default;

                                [[nodiscard]] InterfaceBinder* GetBinder() const noexcept;
                                [[nodiscard]] UInt32 GetGlobalName() const noexcept;
                        };

                        class InterfaceBinder
                        {
                            private:
                                Registry* OwnerRegistry = nullptr;
                                UInt32 MaximumVersion = 1;
                                EngineKernel::KernelContainers::Array<EngineKernel::KernelMemory::SharedPtr<Interface>> BoundInterfaces;

                                [[nodiscard]] Bool Bind(
                                    wl_registry* NativeRegistry,
                                    UInt32 GlobalName,
                                    UInt32 AdvertisedVersion
                                ) noexcept;

                                [[nodiscard]] Bool Unbind(UInt32 GlobalName) noexcept;
                                void Detach() noexcept;

                                friend class Registry;

                            public:
                                explicit InterfaceBinder(UInt32 MaximumVersion) noexcept;
                                InterfaceBinder(const InterfaceBinder&) = delete;
                                InterfaceBinder& operator=(const InterfaceBinder&) = delete;
                                InterfaceBinder(InterfaceBinder&&) = delete;
                                InterfaceBinder& operator=(InterfaceBinder&&) = delete;

                                virtual ~InterfaceBinder();

                                [[nodiscard]] UInt32 GetMaximumVersion() const noexcept;
                                [[nodiscard]] Interface* GetInterface(UInt32 GlobalName) const noexcept;
                                [[nodiscard]] Interface* GetFirstInterface() const noexcept;
                                [[nodiscard]] Size GetInterfaceCount() const noexcept;

                                [[nodiscard]] virtual const wl_interface* GetInterfaceDescription() const noexcept = 0;
                                [[nodiscard]] virtual EngineKernel::KernelMemory::SharedPtr<Interface> CreateInterface(
                                    void* NativeInterface
                                ) = 0;
                        };
                    }
                }
            }
        }
    }
}
