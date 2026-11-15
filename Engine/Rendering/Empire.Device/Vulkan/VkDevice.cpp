#include <algorithm>
#include "VkDevice.hpp"

#include <cstring>
#include <stdexcept>
#include <vector>

namespace Engine
{
    namespace Rendering
    {
        namespace Vulkan
        {
            namespace
            {
                Types::b8 SupportsSwapChain(VkPhysicalDevice device)
                {
                    Types::u32 count = 0;

                    if (vkEnumerateDeviceExtensionProperties(
                            device, nullptr, &count, nullptr) != VK_SUCCESS)
                    {
                        return false;
                    }

                    std::vector<VkExtensionProperties> extensions(count);

                    if (vkEnumerateDeviceExtensionProperties(
                            device, nullptr, &count, extensions.data()) < VK_SUCCESS)
                    {
                        return false;
                    }

                    for (const VkExtensionProperties& extension : extensions)
                    {
                        if (std::strcmp(
                                extension.extensionName,
                                VK_KHR_SWAPCHAIN_EXTENSION_NAME) == 0)
                        {
                            return true;
                        }
                    }

                    return false;
                }

                // Семейство очередей с графикой + present. false, если такого нет.
                Types::b8 FindQueueFamily(
                    VkPhysicalDevice device,
                    VkSurfaceKHR surface,
                    Types::u32& family)
                {
                    Types::u32 count = 0;
                    vkGetPhysicalDeviceQueueFamilyProperties(device, &count, nullptr);

                    std::vector<VkQueueFamilyProperties> families(count);
                    vkGetPhysicalDeviceQueueFamilyProperties(
                        device, &count, families.data());

                    for (Types::u32 i = 0; i < count; ++i)
                    {
                        if (!(families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT))
                        {
                            continue;
                        }

                        VkBool32 present = VK_FALSE;

                        if (vkGetPhysicalDeviceSurfaceSupportKHR(
                                device, i, surface, &present) != VK_SUCCESS)
                        {
                            continue;
                        }

                        if (present == VK_TRUE)
                        {
                            family = i;
                            return true;
                        }
                    }

                    return false;
                }

                Types::i32 ScoreDeviceType(VkPhysicalDeviceType type)
                {
                    switch (type)
                    {
                        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:   return 1000;
                        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU: return 500;
                        case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:    return 250;
                        case VK_PHYSICAL_DEVICE_TYPE_CPU:            return 100;
                        default:                                     return 10;
                    }
                }
            }

            Device::Device(const Instance& instance, VkSurfaceKHR surface)
            {
                // ---------------------------------------------------------
                // Выбор физического устройства
                // ---------------------------------------------------------
                Types::u32 count = 0;

                Check(
                    vkEnumeratePhysicalDevices(instance.Get(), &count, nullptr),
                    "vkEnumeratePhysicalDevices");

                if (count == 0)
                {
                    throw std::runtime_error(
                        "Vulkan: no GPU with Vulkan support was found");
                }

                std::vector<VkPhysicalDevice> devices(count);

                Check(
                    vkEnumeratePhysicalDevices(instance.Get(), &count, devices.data()),
                    "vkEnumeratePhysicalDevices");

                Types::i32 bestScore = -1;

                for (VkPhysicalDevice candidate : devices)
                {
                    if (!SupportsSwapChain(candidate))
                    {
                        continue;
                    }

                    Types::u32 family = 0;

                    if (!FindQueueFamily(candidate, surface, family))
                    {
                        continue;
                    }

                    VkPhysicalDeviceProperties properties{};
                    vkGetPhysicalDeviceProperties(candidate, &properties);

                    const Types::i32 score =
                        ScoreDeviceType(properties.deviceType);

                    if (score > bestScore)
                    {
                        bestScore = score;
                        physicalDevice = candidate;
                        queueFamily = family;
                        name = properties.deviceName;
                    }
                }

                if (physicalDevice == VK_NULL_HANDLE)
                {
                    throw std::runtime_error(
                        "Vulkan: no GPU can render AND present to this window "
                        "(need graphics queue + VK_KHR_swapchain)");
                }

                // ---------------------------------------------------------
                // Логическое устройство
                // ---------------------------------------------------------
                const Types::f32 priority = 1.0f;

                VkDeviceQueueCreateInfo queueInfo{};
                queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                queueInfo.queueFamilyIndex = queueFamily;
                queueInfo.queueCount = 1;
                queueInfo.pQueuePriorities = &priority;

                const Types::CStr extensions[] = {
                    VK_KHR_SWAPCHAIN_EXTENSION_NAME
                };

                // Анизотропная фильтрация нужна небу (в Godot облачные текстуры
                // идут с filter_linear_mipmap_anisotropic). Включаем, если есть.
                VkPhysicalDeviceFeatures supported{};
                vkGetPhysicalDeviceFeatures(physicalDevice, &supported);

                VkPhysicalDeviceProperties deviceProps{};
                vkGetPhysicalDeviceProperties(physicalDevice, &deviceProps);

                VkPhysicalDeviceFeatures enabled{};

                if (supported.samplerAnisotropy)
                {
                    enabled.samplerAnisotropy = VK_TRUE;
                    maxAnisotropy = std::min(16.0f, deviceProps.limits.maxSamplerAnisotropy);
                }

                VkDeviceCreateInfo info{};
                info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
                info.pEnabledFeatures = &enabled;
                info.queueCreateInfoCount = 1;
                info.pQueueCreateInfos = &queueInfo;
                info.enabledExtensionCount = 1;
                info.ppEnabledExtensionNames = extensions;

                Check(
                    vkCreateDevice(physicalDevice, &info, nullptr, &handle),
                    "vkCreateDevice");

                vkGetDeviceQueue(handle, queueFamily, 0, &queue);
            }

            Device::~Device()
            {
                if (handle != VK_NULL_HANDLE)
                {
                    vkDeviceWaitIdle(handle);
                    vkDestroyDevice(handle, nullptr);
                    handle = VK_NULL_HANDLE;
                }
            }

            Types::f32 Device::GetMaxAnisotropy() const
            {
                return maxAnisotropy;
            }

            VkPhysicalDevice Device::GetPhysicalDevice() const
            {
                return physicalDevice;
            }

            VkDevice Device::Get() const
            {
                return handle;
            }

            VkQueue Device::GetQueue() const
            {
                return queue;
            }

            Types::u32 Device::GetQueueFamily() const
            {
                return queueFamily;
            }

            const Types::String& Device::GetName() const
            {
                return name;
            }

            void Device::WaitIdle() const
            {
                Check(vkDeviceWaitIdle(handle), "vkDeviceWaitIdle");
            }

            Types::u32 Device::FindMemoryType(
                Types::u32 typeBits,
                VkMemoryPropertyFlags properties) const
            {
                VkPhysicalDeviceMemoryProperties memory{};
                vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memory);

                for (Types::u32 i = 0; i < memory.memoryTypeCount; ++i)
                {
                    const Types::b8 allowed = (typeBits & (1u << i)) != 0;
                    const Types::b8 matches =
                        (memory.memoryTypes[i].propertyFlags & properties) == properties;

                    if (allowed && matches)
                    {
                        return i;
                    }
                }

                throw std::runtime_error("Vulkan: no suitable memory type");
            }

            VkFormat Device::FindDepthFormat() const
            {
                const VkFormat candidates[] =
                {
                    VK_FORMAT_D32_SFLOAT,
                    VK_FORMAT_D32_SFLOAT_S8_UINT,
                    VK_FORMAT_D24_UNORM_S8_UINT
                };

                for (const VkFormat format : candidates)
                {
                    VkFormatProperties props{};
                    vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

                    if (props.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
                    {
                        return format;
                    }
                }

                throw std::runtime_error("Vulkan: no supported depth format");
            }
        }
    }
}
