#include "Buffer.hpp"

#include <sys/mman.h>
#include <sys/syscall.h>

#include <unistd.h>
#include <linux/memfd.h>

namespace Engine
{
    namespace Platform
    {
        namespace Wayland
        {
            const wl_buffer_listener Buffer::listener =
            {
                Buffer::Release
            };


            static int CreateMemFd()
            {
                return syscall(
                    SYS_memfd_create,
                    "Empire.Engine.Wayland.Buffer",
                    MFD_CLOEXEC
                );
            }


            Buffer::Buffer(
                wl_shm* shm,
                Types::u32 width,
                Types::u32 height
            )
                : width(width),
                  height(height)
            {
                stride =
                    width * 4;

                size =
                    stride * height;

                fd =
                    CreateMemFd();

                if (fd == -1)
                {
                    return;
                }

                if (
                    ftruncate(
                        fd,
                        size
                    ) == -1
                )
                {
                    close(fd);
                    fd = -1;
                    return;
                }

                data =
                    mmap(
                        nullptr,
                        size,
                        PROT_READ | PROT_WRITE,
                        MAP_SHARED,
                        fd,
                        0
                    );

                if (data == MAP_FAILED)
                {
                    data = nullptr;

                    close(fd);
                    fd = -1;

                    return;
                }

                wl_shm_pool* pool =
                    wl_shm_create_pool(
                        shm,
                        fd,
                        size
                    );

                if (!pool)
                {
                    munmap(
                        data,
                        size
                    );

                    data = nullptr;

                    close(fd);
                    fd = -1;

                    return;
                }

                buffer =
                    wl_shm_pool_create_buffer(
                        pool,
                        0,
                        static_cast<int>(width),
                        static_cast<int>(height),
                        static_cast<int>(stride),
                        WL_SHM_FORMAT_ARGB8888
                    );

                wl_shm_pool_destroy(pool);

                if (!buffer)
                {
                    munmap(
                        data,
                        size
                    );

                    data = nullptr;

                    close(fd);
                    fd = -1;

                    return;
                }

                wl_buffer_add_listener(
                    buffer,
                    &listener,
                    this
                );
            }


            Buffer::~Buffer()
            {
                if (
                    buffer &&
                    released
                )
                {
                    wl_buffer_destroy(buffer);
                    buffer = nullptr;
                }

                if (data)
                {
                    munmap(
                        data,
                        size
                    );

                    data = nullptr;
                }

                if (fd != -1)
                {
                    close(fd);
                    fd = -1;
                }
            }


            void Buffer::Release(
                void* data,
                wl_buffer*
            )
            {
                Buffer* self =
                    static_cast<Buffer*>(data);

                self->released = true;
            }


            bool Buffer::IsValid() const
            {
                return buffer != nullptr;
            }


            void* Buffer::GetData() const
            {
                return data;
            }


            Types::u32 Buffer::GetWidth() const
            {
                return width;
            }


            Types::u32 Buffer::GetHeight() const
            {
                return height;
            }


            Types::u32 Buffer::GetStride() const
            {
                return stride;
            }


            wl_buffer* Buffer::Get() const
            {
                return buffer;
            }
        }
    }
}