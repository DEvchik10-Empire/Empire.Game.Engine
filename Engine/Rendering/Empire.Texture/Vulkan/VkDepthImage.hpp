#pragma once

#include "../../Empire.Device/Vulkan/VkCommon.hpp"
#include "../../Empire.Device/Vulkan/VkDevice.hpp"

namespace Engine
{
    namespace Rendering
    {
        namespace Vulkan
        {
            // Буфер глубины под размер кадра. Пересоздаётся при resize.
            class DepthImage
            {
                public:
                    DepthImage(const Device& device, Types::u32 width, Types::u32 height);
                    ~DepthImage();

                    DepthImage(const DepthImage&) = delete;
                    DepthImage& operator=(const DepthImage&) = delete;

                    VkImageView GetView() const;
                    VkFormat GetFormat() const;

                private:
                    void Destroy();

                    const Device& device;

                    VkImage image = VK_NULL_HANDLE;
                    VkDeviceMemory memory = VK_NULL_HANDLE;
                    VkImageView view = VK_NULL_HANDLE;
                    VkFormat format = VK_FORMAT_UNDEFINED;
            };
        }
    }
}
