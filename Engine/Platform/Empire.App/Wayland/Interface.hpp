#pragma once

#include <memory>
#include <vector>
#include <wayland-client.h>

#include "../Empire.Engine/Kernel/Types.hpp"

namespace Engine
{
    namespace Platform
    {
        using namespace Kernel;

        namespace Wayland
        {
            class InterfaceBinder;

            class Interface
            {
            public:
                InterfaceBinder* binder = nullptr;
                Types::u32 interfaceId = 0;

                virtual ~Interface() = default;
            };


            class InterfaceBinder
            {
            public:
                Types::u32 version = 0;

                std::vector<std::unique_ptr<Interface>> interfaces;

                virtual const wl_interface*
                    GetInterfaceDescription() const = 0;

                virtual std::unique_ptr<Interface>
                    CreateInterface(void* data) = 0;

                void Bind(
                    void* data,
                    Types::u32 interfaceId
                );

                Types::b8 Unbind(
                    Types::u32 interfaceId
                );

                Interface* GetInterface(
                    Types::u32 interfaceId
                ) const;

                Interface* GetFirstInterface() const;

                InterfaceBinder(
                    Types::u32 version
                )
                    : version(version)
                {
                }

                virtual ~InterfaceBinder() = default;
            };
        }
    }
}