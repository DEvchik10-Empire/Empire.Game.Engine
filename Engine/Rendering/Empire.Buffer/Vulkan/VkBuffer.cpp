#include "VkBuffer.hpp"

#include <cstring>
#include <stdexcept>

namespace Engine
{
    namespace Rendering
    {
        namespace Vulkan
        {
            Buffer::Buffer(const Device& device, VkDeviceSize size, VkBufferUsageFlags usage)
                : device(device),
                  size(size)
            {
                try
                {
                    VkBufferCreateInfo info{};
                    info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
                    info.size = size;
                    info.usage = usage;
                    info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

                    Check(vkCreateBuffer(device.Get(), &info, nullptr, &handle), "vkCreateBuffer");

                    VkMemoryRequirements requirements{};
                    vkGetBufferMemoryRequirements(device.Get(), handle, &requirements);

                    VkMemoryAllocateInfo allocate{};
                    allocate.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
                    allocate.allocationSize = requirements.size;
                    allocate.memoryTypeIndex = device.FindMemoryType(
                        requirements.memoryTypeBits,
                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

                    Check(vkAllocateMemory(device.Get(), &allocate, nullptr, &memory), "vkAllocateMemory");
                    Check(vkBindBufferMemory(device.Get(), handle, memory, 0), "vkBindBufferMemory");
                }
                catch (...)
                {
                    Destroy();
                    throw;
                }
            }

            Buffer::~Buffer()
            {
                Destroy();
            }

            void Buffer::Destroy()
            {
                if (handle != VK_NULL_HANDLE)
                {
                    vkDestroyBuffer(device.Get(), handle, nullptr);
                    handle = VK_NULL_HANDLE;
                }

                if (memory != VK_NULL_HANDLE)
                {
                    vkFreeMemory(device.Get(), memory, nullptr);
                    memory = VK_NULL_HANDLE;
                }
            }

            void Buffer::Upload(const void* data, VkDeviceSize uploadSize, VkDeviceSize offset)
            {
                if (offset + uploadSize > size)
                {
                    throw std::runtime_error("Vulkan: Buffer::Upload out of range");
                }

                void* mapped = nullptr;

                Check(
                    vkMapMemory(device.Get(), memory, offset, uploadSize, 0, &mapped),
                    "vkMapMemory");

                std::memcpy(mapped, data, static_cast<std::size_t>(uploadSize));

                vkUnmapMemory(device.Get(), memory);
            }

            VkBuffer Buffer::Get() const
            {
                return handle;
            }

            VkDeviceSize Buffer::GetSize() const
            {
                return size;
            }
        }
    }
}
