#include "VkDepthImage.hpp"

namespace Engine
{
    namespace Rendering
    {
        namespace Vulkan
        {
            DepthImage::DepthImage(const Device& device, Types::u32 width, Types::u32 height)
                : device(device),
                  format(device.FindDepthFormat())
            {
                try
                {
                    VkImageCreateInfo info{};
                    info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
                    info.imageType = VK_IMAGE_TYPE_2D;
                    info.format = format;
                    info.extent = {width, height, 1};
                    info.mipLevels = 1;
                    info.arrayLayers = 1;
                    info.samples = VK_SAMPLE_COUNT_1_BIT;
                    info.tiling = VK_IMAGE_TILING_OPTIMAL;
                    info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
                    info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
                    info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

                    Check(vkCreateImage(device.Get(), &info, nullptr, &image), "vkCreateImage(depth)");

                    VkMemoryRequirements requirements{};
                    vkGetImageMemoryRequirements(device.Get(), image, &requirements);

                    VkMemoryAllocateInfo allocate{};
                    allocate.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
                    allocate.allocationSize = requirements.size;
                    allocate.memoryTypeIndex = device.FindMemoryType(
                        requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

                    Check(vkAllocateMemory(device.Get(), &allocate, nullptr, &memory), "vkAllocateMemory(depth)");
                    Check(vkBindImageMemory(device.Get(), image, memory, 0), "vkBindImageMemory(depth)");

                    VkImageViewCreateInfo viewInfo{};
                    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
                    viewInfo.image = image;
                    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
                    viewInfo.format = format;
                    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
                    viewInfo.subresourceRange.levelCount = 1;
                    viewInfo.subresourceRange.layerCount = 1;

                    Check(vkCreateImageView(device.Get(), &viewInfo, nullptr, &view), "vkCreateImageView(depth)");
                }
                catch (...)
                {
                    Destroy();
                    throw;
                }
            }

            DepthImage::~DepthImage()
            {
                Destroy();
            }

            void DepthImage::Destroy()
            {
                if (view != VK_NULL_HANDLE)
                {
                    vkDestroyImageView(device.Get(), view, nullptr);
                    view = VK_NULL_HANDLE;
                }

                if (image != VK_NULL_HANDLE)
                {
                    vkDestroyImage(device.Get(), image, nullptr);
                    image = VK_NULL_HANDLE;
                }

                if (memory != VK_NULL_HANDLE)
                {
                    vkFreeMemory(device.Get(), memory, nullptr);
                    memory = VK_NULL_HANDLE;
                }
            }

            VkImageView DepthImage::GetView() const
            {
                return view;
            }

            VkFormat DepthImage::GetFormat() const
            {
                return format;
            }
        }
    }
}
