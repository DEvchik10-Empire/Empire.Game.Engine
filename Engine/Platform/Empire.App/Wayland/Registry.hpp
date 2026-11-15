#pragma once

#include <vector>

#include <wayland-client.h>

#include "Interface.hpp"

namespace Engine
{
    namespace Platform
    {
        namespace Wayland
        {
            class Display;

            class Registry
            {
            private:
                wl_registry* registry = nullptr;
                std::vector<InterfaceBinder*> binders;

                static void Global(
                    void* data,
                    wl_registry* registry,
                    Types::u32 name,
                    const char* interface,
                    Types::u32 version
                );

                static void GlobalRemove(
                    void* data,
                    wl_registry* registry,
                    Types::u32 name
                );

                static const wl_registry_listener listener;

                void BindInterface(
                    const char* interface,
                    Types::u32 version,
                    Types::u32 name
                );

                void UnbindInterface(Types::u32 name);

            public:
                explicit Registry(Display& display);
                ~Registry();

                Registry(const Registry&) = delete;
                Registry& operator=(const Registry&) = delete;

                void SetBinders(
                    const std::vector<InterfaceBinder*>& binders
                );

                wl_registry* Get() const;
            };
        }
    }
}