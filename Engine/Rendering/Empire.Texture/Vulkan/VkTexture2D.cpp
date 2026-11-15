#include "VkTexture2D.hpp"

#include <algorithm>
#include <stdexcept>
#include <vector>

#include "../../Empire.Buffer/Vulkan/VkBuffer.hpp"

namespace Engine
{
    namespace Rendering
    {
        namespace Vulkan
        {
            namespace
            {
                void Barrier(
                    Types::u32 levels,
                    VkCommandBuffer commandBuffer,
                    VkImage image,
                    VkImageLayout oldLayout,
                    VkImageLayout newLayout,
                    VkPipelineStageFlags srcStage,
                    VkPipelineStageFlags dstStage,
                    VkAccessFlags srcAccess,
                    VkAccessFlags dstAccess)
                {
                    VkImageMemoryBarrier barrier{};
                    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
                    barrier.srcAccessMask = srcAccess;
                    barrier.dstAccessMask = dstAccess;
                    barrier.oldLayout = oldLayout;
                    barrier.newLayout = newLayout;
                    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                    barrier.image = image;
                    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                    barrier.subresourceRange.levelCount = levels;
                    barrier.subresourceRange.layerCount = 1;

                    vkCmdPipelineBarrier(
                        commandBuffer, srcStage, dstStage, 0,
                        0, nullptr, 0, nullptr, 1, &barrier);
                }
            }

            Texture2D::Texture2D(const Device& device, const ImageR8& source, Types::b8 repeat, Types::b8 mipmaps)
                : device(device)
            {
                Create(source.width, source.height, 1, VK_FORMAT_R8_UNORM, source.pixels, repeat, mipmaps);
            }

            Texture2D::Texture2D(const Device& device, const ImageRGBA8& source, Types::b8 srgb, Types::b8 repeat, Types::b8 mipmaps)
                : device(device)
            {
                Create(
                    source.width, source.height, 4,
                    srgb ? VK_FORMAT_R8G8B8A8_SRGB : VK_FORMAT_R8G8B8A8_UNORM,
                    source.pixels, repeat, mipmaps);
            }

            void Texture2D::Create(
                Types::u32 width, Types::u32 height, Types::u32 channels, VkFormat format,
                const std::vector<Types::u8>& pixels, Types::b8 repeat, Types::b8 mipmaps)
            {
                if (width == 0 || height == 0 ||
                    pixels.size() != static_cast<std::size_t>(width) * height * channels)
                {
                    throw std::runtime_error("Texture2D: invalid image");
                }

                // Цепочка уровней: все подряд в одном буфере (уровень 0, 1, ...).
                std::vector<Types::u8> data = pixels;
                std::vector<VkBufferImageCopy> regions;

                {
                    VkBufferImageCopy region{};
                    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                    region.imageSubresource.layerCount = 1;
                    region.imageExtent = {width, height, 1};
                    regions.push_back(region);
                }

                if (mipmaps)
                {
                    Types::u32 w = width;
                    Types::u32 h = height;
                    std::size_t previousOffset = 0;

                    while (w > 1 || h > 1)
                    {
                        const Types::u32 nw = std::max(1u, w / 2);
                        const Types::u32 nh = std::max(1u, h / 2);
                        const std::size_t offset = data.size();

                        data.resize(offset + static_cast<std::size_t>(nw) * nh * channels);

                        for (Types::u32 y = 0; y < nh; ++y)
                        {
                            for (Types::u32 x = 0; x < nw; ++x)
                            {
                                const Types::u32 x0 = std::min(x * 2, w - 1);
                                const Types::u32 x1 = std::min(x * 2 + 1, w - 1);
                                const Types::u32 y0 = std::min(y * 2, h - 1);
                                const Types::u32 y1 = std::min(y * 2 + 1, h - 1);

                                for (Types::u32 c = 0; c < channels; ++c)
                                {
                                    const auto at = [&](Types::u32 px, Types::u32 py) -> Types::u32
                                    {
                                        return data[previousOffset + (static_cast<std::size_t>(py) * w + px) * channels + c];
                                    };

                                    const Types::u32 sum = at(x0, y0) + at(x1, y0) + at(x0, y1) + at(x1, y1);

                                    data[offset + (static_cast<std::size_t>(y) * nw + x) * channels + c] =
                                        static_cast<Types::u8>((sum + 2) / 4);
                                }
                            }
                        }

                        VkBufferImageCopy region{};
                        region.bufferOffset = offset;
                        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                        region.imageSubresource.mipLevel = static_cast<Types::u32>(regions.size());
                        region.imageSubresource.layerCount = 1;
                        region.imageExtent = {nw, nh, 1};
                        regions.push_back(region);

                        previousOffset = offset;
                        w = nw;
                        h = nh;
                    }
                }

                const Types::u32 levels = static_cast<Types::u32>(regions.size());

                VkCommandPool pool = VK_NULL_HANDLE;

                try
                {
                    // ---- изображение в памяти GPU ----
                    VkImageCreateInfo info{};
                    info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
                    info.imageType = VK_IMAGE_TYPE_2D;
                    info.format = format;
                    info.extent = {width, height, 1};
                    info.mipLevels = levels;
                    info.arrayLayers = 1;
                    info.samples = VK_SAMPLE_COUNT_1_BIT;
                    info.tiling = VK_IMAGE_TILING_OPTIMAL;
                    info.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
                    info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
                    info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

                    Check(vkCreateImage(device.Get(), &info, nullptr, &image), "vkCreateImage(texture)");

                    VkMemoryRequirements requirements{};
                    vkGetImageMemoryRequirements(device.Get(), image, &requirements);

                    VkMemoryAllocateInfo allocate{};
                    allocate.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
                    allocate.allocationSize = requirements.size;
                    allocate.memoryTypeIndex = device.FindMemoryType(
                        requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

                    Check(vkAllocateMemory(device.Get(), &allocate, nullptr, &memory), "vkAllocateMemory(texture)");
                    Check(vkBindImageMemory(device.Get(), image, memory, 0), "vkBindImageMemory(texture)");

                    // ---- заливка через staging-буфер ----
                    Buffer staging(device, data.size(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
                    staging.Upload(data.data(), data.size());

                    VkCommandPoolCreateInfo poolInfo{};
                    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
                    poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
                    poolInfo.queueFamilyIndex = device.GetQueueFamily();

                    Check(vkCreateCommandPool(device.Get(), &poolInfo, nullptr, &pool), "vkCreateCommandPool(upload)");

                    VkCommandBufferAllocateInfo cbInfo{};
                    cbInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
                    cbInfo.commandPool = pool;
                    cbInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
                    cbInfo.commandBufferCount = 1;

                    VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
                    Check(vkAllocateCommandBuffers(device.Get(), &cbInfo, &commandBuffer), "vkAllocateCommandBuffers(upload)");

                    VkCommandBufferBeginInfo beginInfo{};
                    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
                    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

                    Check(vkBeginCommandBuffer(commandBuffer, &beginInfo), "vkBeginCommandBuffer(upload)");

                    Barrier(
                        levels, commandBuffer, image,
                        VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
                        0, VK_ACCESS_TRANSFER_WRITE_BIT);

                    vkCmdCopyBufferToImage(
                        commandBuffer, staging.Get(), image,
                        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                        static_cast<Types::u32>(regions.size()), regions.data());

                    Barrier(
                        levels, commandBuffer, image,
                        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                        VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                        VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT);

                    Check(vkEndCommandBuffer(commandBuffer), "vkEndCommandBuffer(upload)");

                    VkSubmitInfo submit{};
                    submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
                    submit.commandBufferCount = 1;
                    submit.pCommandBuffers = &commandBuffer;

                    Check(vkQueueSubmit(device.GetQueue(), 1, &submit, VK_NULL_HANDLE), "vkQueueSubmit(upload)");
                    Check(vkQueueWaitIdle(device.GetQueue()), "vkQueueWaitIdle(upload)");

                    vkDestroyCommandPool(device.Get(), pool, nullptr);
                    pool = VK_NULL_HANDLE;

                    // ---- view + sampler ----
                    VkImageViewCreateInfo viewInfo{};
                    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
                    viewInfo.image = image;
                    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
                    viewInfo.format = format;
                    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                    viewInfo.subresourceRange.levelCount = levels;
                    viewInfo.subresourceRange.layerCount = 1;

                    Check(vkCreateImageView(device.Get(), &viewInfo, nullptr, &view), "vkCreateImageView(texture)");

                    const VkSamplerAddressMode mode =
                        repeat ? VK_SAMPLER_ADDRESS_MODE_REPEAT : VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;

                    VkSamplerCreateInfo samplerInfo{};
                    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
                    samplerInfo.magFilter = VK_FILTER_LINEAR;
                    samplerInfo.minFilter = VK_FILTER_LINEAR;
                    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
                    samplerInfo.addressModeU = mode;
                    samplerInfo.addressModeV = mode;
                    samplerInfo.addressModeW = mode;
                    if (device.GetMaxAnisotropy() > 1.0f)
                    {
                        samplerInfo.anisotropyEnable = VK_TRUE;
                        samplerInfo.maxAnisotropy = device.GetMaxAnisotropy();
                    }

                    samplerInfo.minLod = 0.0f;
                    samplerInfo.maxLod = static_cast<float>(levels);
                    samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;

                    Check(vkCreateSampler(device.Get(), &samplerInfo, nullptr, &sampler), "vkCreateSampler");
                }
                catch (...)
                {
                    if (pool != VK_NULL_HANDLE)
                    {
                        vkDestroyCommandPool(device.Get(), pool, nullptr);
                    }

                    Destroy();
                    throw;
                }
            }

            Texture2D::~Texture2D()
            {
                Destroy();
            }

            void Texture2D::Destroy()
            {
                if (sampler != VK_NULL_HANDLE)
                {
                    vkDestroySampler(device.Get(), sampler, nullptr);
                    sampler = VK_NULL_HANDLE;
                }

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

            VkImageView Texture2D::GetView() const
            {
                return view;
            }

            VkSampler Texture2D::GetSampler() const
            {
                return sampler;
            }
        }
    }
}
