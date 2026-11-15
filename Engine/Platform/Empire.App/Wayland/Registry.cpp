#include "Registry.hpp"

#include <cstring>

#include "Display.hpp"

namespace Engine
{
    namespace Platform
    {
        namespace Wayland
        {
            const wl_registry_listener Registry::listener =
            {
                Registry::Global,
                Registry::GlobalRemove
            };


            Registry::Registry(Display& display)
            {
                registry = wl_display_get_registry(display.Get());

                wl_registry_add_listener(
                    registry,
                    &listener,
                    this
                );
            }


            Registry::~Registry()
            {
                if (registry)
                {
                    wl_registry_destroy(registry);
                }
            }


            void Registry::Global(
                void* data,
                wl_registry* /*registry*/,
                Types::u32 name,
                const char* interface,
                Types::u32 version
            )
            {
                Registry* self = static_cast<Registry*>(data);

                self->BindInterface(
                    interface,
                    version,
                    name
                );
            }


            void Registry::GlobalRemove(
                void* data,
                wl_registry* /*registry*/,
                Types::u32 name
            )
            {
                Registry* self = static_cast<Registry*>(data);

                self->UnbindInterface(name);
            }


            void Registry::BindInterface(
                const char* interface,
                Types::u32 version,
                Types::u32 name
            )
            {
                for (InterfaceBinder* binder : binders)
                {
                    const wl_interface* description =
                        binder->GetInterfaceDescription();

                    if (std::strcmp(description->name, interface) != 0)
                    {
                        continue;
                    }

                    if (binder->version > version)
                    {
                        continue;
                    }

                    void* object = wl_registry_bind(
                        registry,
                        name,
                        description,
                        binder->version
                    );

                    binder->Bind(object, name);

                    break;
                }
            }


            void Registry::UnbindInterface(Types::u32 name)
            {
                for (InterfaceBinder* binder : binders)
                {
                    if (binder->Unbind(name))
                    {
                        break;
                    }
                }
            }


            void Registry::SetBinders(
                const std::vector<InterfaceBinder*>& binders
            )
            {
                this->binders = binders;
            }


            wl_registry* Registry::Get() const
            {
                return registry;
            }
        }
    }
}