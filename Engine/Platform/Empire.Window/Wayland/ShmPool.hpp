#pragma once

#include <memory>
#include <wayland-client.h>

#include "Buffer.hpp"

namespace Engine
{
    namespace Platform
    {
        namespace Wayland
        {
            class ShmPool
            {
            private:
                wl_shm* shm = nullptr;

            public:
                explicit ShmPool(
                    wl_shm* shm
                );

                std::unique_ptr<Buffer> CreateBuffer(
                    Types::u32 width,
                    Types::u32 height
                );
            };
        }
    }
}