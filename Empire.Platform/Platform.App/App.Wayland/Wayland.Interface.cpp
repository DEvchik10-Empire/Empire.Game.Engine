#include "Empire.Platform/Platform.App/App.Wayland/Wayland.Interface.hpp"

#include "Empire.Platform/Platform.App/App.Wayland/Wayland.Registry.hpp"

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
                    InterfaceBinder* Interface::GetBinder() const noexcept
                    {
                        return OwnerBinder;
                    }

                    UInt32 Interface::GetGlobalName() const noexcept
                    {
                        return GlobalName;
                    }

                    InterfaceBinder::InterfaceBinder(UInt32 RequestedVersion) noexcept
                        : MaximumVersion(RequestedVersion)
                    {
                    }

                    InterfaceBinder::~InterfaceBinder()
                    {
                        if (OwnerRegistry != nullptr)
                        {
                            if (!OwnerRegistry->UnregisterBinder(*this))
                                Detach();
                        }
                        else
                            Detach();
                    }

                    UInt32 InterfaceBinder::GetMaximumVersion() const noexcept
                    {
                        return MaximumVersion;
                    }

                    Interface* InterfaceBinder::GetInterface(UInt32 GlobalName) const noexcept
                    {
                        for (const EmpireKernel::KernelMemory::SharedPtr<Interface>& BoundInterface : BoundInterfaces)
                        {
                            if (BoundInterface->GetGlobalName() == GlobalName)
                                return BoundInterface.Get();
                        }

                        return nullptr;
                    }

                    Interface* InterfaceBinder::GetFirstInterface() const noexcept
                    {
                        return BoundInterfaces.IsEmpty() ? nullptr : BoundInterfaces[0].Get();
                    }

                    Size InterfaceBinder::GetInterfaceCount() const noexcept
                    {
                        return BoundInterfaces.GetCount();
                    }

                    Bool InterfaceBinder::Bind(
                        wl_registry* NativeRegistry,
                        UInt32 GlobalName,
                        UInt32 AdvertisedVersion
                    ) noexcept
                    {
                        const wl_interface* Description = GetInterfaceDescription();

                        if (
                            NativeRegistry == nullptr ||
                            Description == nullptr ||
                            Description->name == nullptr ||
                            Description->version <= 0 ||
                            MaximumVersion == 0 ||
                            AdvertisedVersion == 0 ||
                            GetInterface(GlobalName) != nullptr
                        )
                        {
                            return false;
                        }

                        const UInt32 InterfaceVersion = static_cast<UInt32>(Description->version);
                        const UInt32 SupportedVersion = MaximumVersion < InterfaceVersion
                            ? MaximumVersion
                            : InterfaceVersion;
                        const UInt32 BindVersion = AdvertisedVersion < SupportedVersion
                            ? AdvertisedVersion
                            : SupportedVersion;

                        if (BindVersion == 0)
                            return false;

                        try
                        {
                            BoundInterfaces.Reserve(BoundInterfaces.GetCount() + 1);
                        }
                        catch (...)
                        {
                            return false;
                        }

                        void* NativeInterface = wl_registry_bind(
                            NativeRegistry,
                            GlobalName,
                            Description,
                            BindVersion
                        );

                        if (NativeInterface == nullptr)
                            return false;

                        EmpireKernel::KernelMemory::SharedPtr<Interface> NewInterface;

                        try
                        {
                            NewInterface = CreateInterface(NativeInterface);
                        }
                        catch (...)
                        {
                            wl_proxy_destroy(static_cast<wl_proxy*>(NativeInterface));
                            return false;
                        }

                        if (NewInterface == nullptr)
                        {
                            wl_proxy_destroy(static_cast<wl_proxy*>(NativeInterface));
                            return false;
                        }

                        NewInterface->OwnerBinder = this;
                        NewInterface->GlobalName = GlobalName;
                        BoundInterfaces.Add(Move(NewInterface));
                        return true;
                    }

                    Bool InterfaceBinder::Unbind(UInt32 GlobalName) noexcept
                    {
                        for (Size Index = 0; Index < BoundInterfaces.GetCount(); Index++)
                        {
                            if (BoundInterfaces[Index]->GetGlobalName() == GlobalName)
                            {
                                BoundInterfaces.RemoveAt(Index);
                                return true;
                            }
                        }

                        return false;
                    }

                    void InterfaceBinder::Detach() noexcept
                    {
                        BoundInterfaces.Clear();
                        OwnerRegistry = nullptr;
                    }
                }
            }
        }
    }
}
