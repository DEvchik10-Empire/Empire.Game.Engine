#pragma once

#include <memory>
#include <wayland-client.h>

#include "../../Empire.App/Wayland/Interface.hpp"

namespace Engine
{
    namespace Platform
    {
        namespace Wayland
        {
            using namespace Kernel;

            class Buffer
            {
            private:
                wl_buffer* buffer = nullptr;
                void* data = nullptr;

                int fd = -1;

                Types::u32 width = 0;
                Types::u32 height = 0;
                Types::u32 stride = 0;
                Types::u32 size = 0;

                bool released = true;

                static void Release(
                    void* data,
                    wl_buffer* buffer
                );

                static const wl_buffer_listener listener;

            public:
                Buffer() = default;

                Buffer(
                    wl_shm* shm,
                    Types::u32 width,
                    Types::u32 height
                );

                ~Buffer();

                Buffer(const Buffer&) = delete;
                Buffer& operator=(const Buffer&) = delete;

                Buffer(Buffer&&) = delete;
                Buffer& operator=(Buffer&&) = delete;

                bool IsValid() const;

                void* GetData() const;

                Types::u32 GetWidth() const;
                Types::u32 GetHeight() const;
                Types::u32 GetStride() const;

                wl_buffer* Get() const;
            };
        }
    }
}