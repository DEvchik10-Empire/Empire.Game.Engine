#include "ShmPool.hpp"

namespace Engine
{
    namespace Platform
    {
        namespace Wayland
        {
            ShmPool::ShmPool(
                wl_shm* shm
            )
                : shm(shm)
            {
            }


            std::unique_ptr<Buffer>
            ShmPool::CreateBuffer(
                Types::u32 width,
                Types::u32 height
            )
            {
                return std::unique_ptr<Buffer>(
                    new Buffer(
                        shm,
                        width,
                        height
                    )
                );
            }
        }
    }
}