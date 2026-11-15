#pragma once

#include "../../Empire.Device/Vulkan/VkCommon.hpp"
#include "../../Empire.Device/Vulkan/VkDevice.hpp"

namespace Engine
{
    namespace Rendering
    {
        namespace Vulkan
        {
            /*
             * Буфер в памяти, видимой CPU (HOST_VISIBLE | HOST_COHERENT).
             *
             * Просто и надёжно: данные пишутся напрямую, без staging-буфера.
             * Для вершин небольших моделей и uniform-буферов этого достаточно;
             * DEVICE_LOCAL + staging добавится, когда понадобятся большие меши.
             *
             * Время жизни: Buffer умирает РАНЬШЕ Device.
             */
            class Buffer
            {
                public:
                    Buffer(const Device& device, VkDeviceSize size, VkBufferUsageFlags usage);
                    ~Buffer();

                    Buffer(const Buffer&) = delete;
                    Buffer& operator=(const Buffer&) = delete;
                    Buffer(Buffer&&) = delete;
                    Buffer& operator=(Buffer&&) = delete;

                    // Копирует size байт в буфер со смещения offset.
                    void Upload(const void* data, VkDeviceSize size, VkDeviceSize offset = 0);

                    VkBuffer Get() const;
                    VkDeviceSize GetSize() const;

                private:
                    void Destroy();

                    const Device& device;

                    VkBuffer handle = VK_NULL_HANDLE;
                    VkDeviceMemory memory = VK_NULL_HANDLE;
                    VkDeviceSize size = 0;
            };
        }
    }
}
