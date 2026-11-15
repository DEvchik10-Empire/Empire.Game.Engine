#include "VkSwapChain.hpp"

#include <algorithm>
#include <stdexcept>

namespace Engine
{
    namespace Rendering
    {
        namespace Vulkan
        {
            namespace
            {
                VkSurfaceFormatKHR ChooseFormat(
                    const std::vector<VkSurfaceFormatKHR>& formats)
                {
                    // UNORM, а не SRGB: clear color пишется в картинку как есть,
                    // без гамма-кодирования (проще проверять цвета глазами).
                    for (const VkSurfaceFormatKHR& format : formats)
                    {
                        if (
                            format.format == VK_FORMAT_B8G8R8A8_UNORM &&
                            format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
                        {
                            return format;
                        }
                    }

                    for (const VkSurfaceFormatKHR& format : formats)
                    {
                        if (
                            format.format == VK_FORMAT_R8G8B8A8_UNORM &&
                            format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
                        {
                            return format;
                        }
                    }

                    return formats.front();
                }

                VkCompositeAlphaFlagBitsKHR ChooseCompositeAlpha(
                    VkCompositeAlphaFlagsKHR supported)
                {
                    // Wayland-драйверы часто не дают OPAQUE, поэтому перебираем.
                    const VkCompositeAlphaFlagBitsKHR order[] = {
                        VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
                        VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
                        VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
                        VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR
                    };

                    for (VkCompositeAlphaFlagBitsKHR bit : order)
                    {
                        if (supported & bit)
                        {
                            return bit;
                        }
                    }

                    return VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
                }
            }

            SwapChain::SwapChain(
                const Device& device,
                const Surface& surface,
                Types::u32 width,
                Types::u32 height)
                : device(device),
                  surface(surface)
            {
                try
                {
                    if (!Build(width, height))
                    {
                        throw std::runtime_error(
                            "Vulkan: SwapChain size is 0x0 "
                            "(window has no size yet)");
                    }
                }
                catch (...)
                {
                    Destroy();
                    throw;
                }
            }

            SwapChain::~SwapChain()
            {
                Destroy();
            }

            Types::b8 SwapChain::Resize(Types::u32 width, Types::u32 height)
            {
                // Старые изображения и семафоры могут быть ещё в работе.
                device.WaitIdle();

                return Build(width, height);
            }

            // Создаёт (или пересоздаёт) swapchain. Если handle уже есть,
            // он передаётся как oldSwapchain и после успеха уничтожается.
            Types::b8 SwapChain::Build(Types::u32 width, Types::u32 height)
            {
                const VkPhysicalDevice physical = device.GetPhysicalDevice();

                VkSurfaceCapabilitiesKHR capabilities{};

                Check(
                    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
                        physical, surface.Get(), &capabilities),
                    "vkGetPhysicalDeviceSurfaceCapabilitiesKHR");

                // currentExtent == 0xFFFFFFFF значит "размер задаёт
                // приложение" (так отвечает Wayland). Иначе размер диктует
                // сама поверхность (X11).
                VkExtent2D newExtent{};

                if (capabilities.currentExtent.width != 0xFFFFFFFFu)
                {
                    newExtent = capabilities.currentExtent;
                }
                else
                {
                    newExtent.width = std::clamp(
                        width,
                        capabilities.minImageExtent.width,
                        capabilities.maxImageExtent.width);

                    newExtent.height = std::clamp(
                        height,
                        capabilities.minImageExtent.height,
                        capabilities.maxImageExtent.height);
                }

                if (newExtent.width == 0 || newExtent.height == 0)
                {
                    return false;
                }

                if (!(capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT))
                {
                    throw std::runtime_error(
                        "Vulkan: swapchain images do not support TRANSFER_DST");
                }

                // Формат
                Types::u32 formatCount = 0;

                Check(
                    vkGetPhysicalDeviceSurfaceFormatsKHR(
                        physical, surface.Get(), &formatCount, nullptr),
                    "vkGetPhysicalDeviceSurfaceFormatsKHR");

                if (formatCount == 0)
                {
                    throw std::runtime_error(
                        "Vulkan: surface reports no formats");
                }

                std::vector<VkSurfaceFormatKHR> formats(formatCount);

                Check(
                    vkGetPhysicalDeviceSurfaceFormatsKHR(
                        physical, surface.Get(), &formatCount, formats.data()),
                    "vkGetPhysicalDeviceSurfaceFormatsKHR");

                const VkSurfaceFormatKHR chosen = ChooseFormat(formats);

                // Сколько изображений: минимум + 1, но не больше максимума
                // (maxImageCount == 0 значит "без ограничения").
                Types::u32 imageCount = capabilities.minImageCount + 1;

                if (
                    capabilities.maxImageCount > 0 &&
                    imageCount > capabilities.maxImageCount)
                {
                    imageCount = capabilities.maxImageCount;
                }

                VkSwapchainCreateInfoKHR info{};
                info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
                info.surface = surface.Get();
                info.minImageCount = imageCount;
                info.imageFormat = chosen.format;
                info.imageColorSpace = chosen.colorSpace;
                info.imageExtent = newExtent;
                info.imageArrayLayers = 1;
                info.imageUsage =
                    VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
                    VK_IMAGE_USAGE_TRANSFER_DST_BIT;
                info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
                info.preTransform = capabilities.currentTransform;
                info.compositeAlpha =
                    ChooseCompositeAlpha(capabilities.supportedCompositeAlpha);
                info.presentMode = VK_PRESENT_MODE_FIFO_KHR; // есть всегда (vsync)
                info.clipped = VK_TRUE;
                info.oldSwapchain = handle;

                VkSwapchainKHR created = VK_NULL_HANDLE;

                Check(
                    vkCreateSwapchainKHR(device.Get(), &info, nullptr, &created),
                    "vkCreateSwapchainKHR");

                // Старый swapchain больше не нужен (WaitIdle уже был в Resize).
                if (handle != VK_NULL_HANDLE)
                {
                    vkDestroySwapchainKHR(device.Get(), handle, nullptr);
                }

                handle = created;
                format = chosen.format;
                extent = newExtent;

                // Изображения принадлежат swapchain и уничтожаются вместе с ним.
                Types::u32 actualCount = 0;

                Check(
                    vkGetSwapchainImagesKHR(
                        device.Get(), handle, &actualCount, nullptr),
                    "vkGetSwapchainImagesKHR");

                images.resize(actualCount);

                Check(
                    vkGetSwapchainImagesKHR(
                        device.Get(), handle, &actualCount, images.data()),
                    "vkGetSwapchainImagesKHR");

                // По семафору "кадр отрисован" на каждое изображение.
                DestroySemaphores();

                renderFinished.resize(actualCount, VK_NULL_HANDLE);

                VkSemaphoreCreateInfo semaphoreInfo{};
                semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

                for (VkSemaphore& semaphore : renderFinished)
                {
                    Check(
                        vkCreateSemaphore(
                            device.Get(), &semaphoreInfo, nullptr, &semaphore),
                        "vkCreateSemaphore");
                }

                return true;
            }

            void SwapChain::DestroySemaphores()
            {
                for (VkSemaphore& semaphore : renderFinished)
                {
                    if (semaphore != VK_NULL_HANDLE)
                    {
                        vkDestroySemaphore(device.Get(), semaphore, nullptr);
                        semaphore = VK_NULL_HANDLE;
                    }
                }

                renderFinished.clear();
            }

            void SwapChain::Destroy()
            {
                if (handle == VK_NULL_HANDLE && renderFinished.empty())
                {
                    return;
                }

                vkDeviceWaitIdle(device.Get());

                DestroySemaphores();

                if (handle != VK_NULL_HANDLE)
                {
                    vkDestroySwapchainKHR(device.Get(), handle, nullptr);
                    handle = VK_NULL_HANDLE;
                }

                images.clear();
            }

            VkSwapchainKHR SwapChain::Get() const
            {
                return handle;
            }

            VkFormat SwapChain::GetFormat() const
            {
                return format;
            }

            VkExtent2D SwapChain::GetExtent() const
            {
                return extent;
            }

            Types::u32 SwapChain::GetImageCount() const
            {
                return static_cast<Types::u32>(images.size());
            }

            VkImage SwapChain::GetImage(Types::u32 index) const
            {
                return images.at(index);
            }

            VkSemaphore SwapChain::GetRenderFinishedSemaphore(
                Types::u32 imageIndex) const
            {
                return renderFinished.at(imageIndex);
            }
        }
    }
}
