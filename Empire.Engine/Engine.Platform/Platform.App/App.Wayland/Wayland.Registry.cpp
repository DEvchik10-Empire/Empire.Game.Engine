#include "Empire.Engine/Engine.Platform/Platform.App/App.Wayland/Wayland.Registry.hpp"
#include "Empire.Engine/Engine.Platform/Platform.App/App.Wayland/Wayland.Display.hpp"

#include <cerrno>

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
                        namespace
                        {
                            Int32 GetWaylandError(wl_display* Display) noexcept
                            {
                                if (Display != nullptr)
                                {
                                    const Int32 DisplayError = wl_display_get_error(Display);
                                    if (DisplayError != 0)
                                        return DisplayError;
                                }

                                return errno != 0 ? errno : -1;
                            }

                            Bool IsSameName(const Char* Left, const Char* Right) noexcept
                            {
                                if (Left == nullptr || Right == nullptr)
                                    return false;

                                while (*Left != '\0' && *Right != '\0')
                                {
                                    if (*Left != *Right)
                                        return false;

                                    ++Left;
                                    ++Right;
                                }

                                return *Left == *Right;
                            }
                        }

                        const wl_registry_listener Registry::Listener =
                        {
                            Registry::Global,
                            Registry::GlobalRemove
                        };

                        Registry::~Registry()
                        {
                            Shutdown();
                        }

                        Bool Registry::Initialize(Display& WaylandDisplay) noexcept
                        {
                            if (NativeRegistry != nullptr)
                            {
                                ErrorCode = -2;
                                return false;
                            }

                            if (!WaylandDisplay.IsReady())
                            {
                                ErrorCode = WaylandDisplay.GetErrorCode();
                                if (ErrorCode == 0)
                                    ErrorCode = -1;
                                return false;
                            }

                            NativeDisplay = WaylandDisplay.Get();

                            errno = 0;
                            NativeRegistry = wl_display_get_registry(NativeDisplay);
                            if (NativeRegistry == nullptr)
                            {
                                ErrorCode = GetWaylandError(NativeDisplay);
                                NativeDisplay = nullptr;
                                return false;
                            }

                            errno = 0;
                            if (wl_registry_add_listener(NativeRegistry, &Listener, this) != 0)
                            {
                                ErrorCode = GetWaylandError(NativeDisplay);
                                wl_registry_destroy(NativeRegistry);
                                NativeRegistry = nullptr;
                                NativeDisplay = nullptr;
                                return false;
                            }

                            ErrorCode = 0;
                            return true;
                        }

                        void Registry::Shutdown() noexcept
                        {
                            while (!Binders.IsEmpty())
                            {
                                InterfaceBinder* Binder = Binders[Binders.GetCount() - 1];
                                if (Binder != nullptr)
                                {
                                    if (!UnregisterBinder(*Binder))
                                    {
                                        // Keep shutdown progressing if the binder list is inconsistent.
                                        Binders.RemoveAt(Binders.GetCount() - 1);
                                    }
                                }
                                else
                                {
                                    Binders.RemoveAt(Binders.GetCount() - 1);
                                }
                            }

                            if (NativeRegistry != nullptr)
                            {
                                wl_registry_destroy(NativeRegistry);
                                NativeRegistry = nullptr;
                            }

                            NativeDisplay = nullptr;
                        }

                        Bool Registry::RegisterBinder(InterfaceBinder& Binder) noexcept
                        {
                            if (!IsReady())
                                return false;

                            if (Binder.OwnerRegistry == this)
                                return true;

                            if (Binder.OwnerRegistry != nullptr)
                                return false;

                            const wl_interface* Description = Binder.GetInterfaceDescription();
                            if (
                                Description == nullptr ||
                                Description->name == nullptr ||
                                Description->version <= 0 ||
                                Binder.GetMaximumVersion() == 0
                            )
                            {
                                return false;
                            }

                            for (InterfaceBinder* ExistingBinder : Binders)
                            {
                                if (
                                    ExistingBinder != nullptr &&
                                    IsSameName(
                                        ExistingBinder->GetInterfaceDescription()->name,
                                        Description->name
                                    )
                                )
                                {
                                    return false;
                                }
                            }

                            try
                            {
                                Binders.Add(&Binder);
                            }
                            catch (...)
                            {
                                ErrorCode = -1;
                                return false;
                            }

                            Binder.OwnerRegistry = this;
                            ErrorCode = 0;

                            errno = 0;
                            if (wl_display_roundtrip(NativeDisplay) < 0)
                            {
                                ErrorCode = GetWaylandError(NativeDisplay);
                                if (!UnregisterBinder(Binder))
                                    ErrorCode = -1;
                                return false;
                            }

                            if (ErrorCode != 0)
                            {
                                if (!UnregisterBinder(Binder))
                                    ErrorCode = -1;
                                return false;
                            }

                            return true;
                        }

                        Bool Registry::UnregisterBinder(InterfaceBinder& Binder) noexcept
                        {
                            if (Binder.OwnerRegistry != this)
                                return false;

                            for (Size Index = 0; Index < Binders.GetCount(); Index++)
                            {
                                if (Binders[Index] == &Binder)
                                {
                                    Binders.RemoveAt(Index);
                                    Binder.Detach();
                                    return true;
                                }
                            }

                            Binder.Detach();
                            return false;
                        }

                        Bool Registry::IsReady() const noexcept
                        {
                            return NativeDisplay != nullptr &&
                                NativeRegistry != nullptr &&
                                wl_display_get_error(NativeDisplay) == 0;
                        }

                        Int32 Registry::GetErrorCode() const noexcept
                        {
                            if (NativeDisplay != nullptr)
                            {
                                const Int32 DisplayError = wl_display_get_error(NativeDisplay);
                                if (DisplayError != 0)
                                    return DisplayError;
                            }

                            return ErrorCode;
                        }

                        Size Registry::GetBinderCount() const noexcept
                        {
                            return Binders.GetCount();
                        }

                        Bool Registry::BindInterface(
                            UInt32 GlobalName,
                            const Char* InterfaceName,
                            UInt32 AdvertisedVersion
                        ) noexcept
                        {
                            for (InterfaceBinder* Binder : Binders)
                            {
                                if (Binder == nullptr)
                                    continue;

                                const wl_interface* Description = Binder->GetInterfaceDescription();
                                if (
                                    Description != nullptr &&
                                    IsSameName(Description->name, InterfaceName)
                                )
                                {
                                    if (!Binder->Bind(NativeRegistry, GlobalName, AdvertisedVersion))
                                    {
                                        const Int32 DisplayError = wl_display_get_error(NativeDisplay);
                                        ErrorCode = DisplayError != 0 ? DisplayError : -1;
                                        return false;
                                    }

                                    return true;
                                }
                            }

                            return false;
                        }

                        void Registry::UnbindInterface(UInt32 GlobalName) noexcept
                        {
                            for (InterfaceBinder* Binder : Binders)
                            {
                                if (Binder != nullptr)
                                {
                                    // Most binders do not own this global, so false is expected.
                                    (void)Binder->Unbind(GlobalName);
                                }
                            }
                        }

                        void Registry::Global(
                            void* UserData,
                            wl_registry* NativeRegistry,
                            uint32_t GlobalName,
                            const Char* InterfaceName,
                            uint32_t AdvertisedVersion
                        )
                        {
                            (void)NativeRegistry;

                            Registry* Instance = static_cast<Registry*>(UserData);
                            if (Instance == nullptr || Instance->NativeRegistry == nullptr)
                                return;

                            // A global with no registered binder is valid and is ignored.
                            (void)Instance->BindInterface(
                                GlobalName,
                                InterfaceName,
                                AdvertisedVersion
                            );
                        }

                        void Registry::GlobalRemove(
                            void* UserData,
                            wl_registry* NativeRegistry,
                            uint32_t GlobalName
                        )
                        {
                            (void)NativeRegistry;

                            Registry* Instance = static_cast<Registry*>(UserData);
                            if (Instance != nullptr)
                                Instance->UnbindInterface(GlobalName);
                        }
                    }
                }
            }
        }
    }
}
