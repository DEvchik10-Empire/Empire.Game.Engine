#include "VkInstance.hpp"

#include <cstring>
#include <iostream>
#include <stdexcept>

namespace Engine
{
    namespace Rendering
    {
        namespace Vulkan
        {
            namespace
            {
                constexpr Types::CStr ValidationLayerName =
                    "VK_LAYER_KHRONOS_validation";

                VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
                    VkDebugUtilsMessageSeverityFlagBitsEXT severity,
                    VkDebugUtilsMessageTypeFlagsEXT /*type*/,
                    const VkDebugUtilsMessengerCallbackDataEXT* data,
                    void* /*userData*/)
                {
                    const char* level =
                        (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
                            ? "ERROR"
                            : "WARNING";

                    std::cerr
                        << "[Vulkan][" << level << "] "
                        << data->pMessage
                        << '\n';

                    // VK_FALSE = не прерывать вызов, который вызвал сообщение.
                    return VK_FALSE;
                }

                std::vector<VkExtensionProperties> GetAvailableExtensions()
                {
                    Types::u32 count = 0;

                    Check(
                        vkEnumerateInstanceExtensionProperties(
                            nullptr, &count, nullptr),
                        "vkEnumerateInstanceExtensionProperties");

                    std::vector<VkExtensionProperties> list(count);

                    Check(
                        vkEnumerateInstanceExtensionProperties(
                            nullptr, &count, list.data()),
                        "vkEnumerateInstanceExtensionProperties");

                    return list;
                }

                Types::b8 ContainsExtension(
                    const std::vector<VkExtensionProperties>& list,
                    Types::CStr name)
                {
                    for (const VkExtensionProperties& extension : list)
                    {
                        if (std::strcmp(extension.extensionName, name) == 0)
                        {
                            return true;
                        }
                    }

                    return false;
                }

                Types::b8 HasValidationLayer()
                {
                    Types::u32 count = 0;

                    if (vkEnumerateInstanceLayerProperties(&count, nullptr)
                        != VK_SUCCESS)
                    {
                        return false;
                    }

                    std::vector<VkLayerProperties> layers(count);

                    if (vkEnumerateInstanceLayerProperties(&count, layers.data())
                        < VK_SUCCESS)
                    {
                        return false;
                    }

                    for (const VkLayerProperties& layer : layers)
                    {
                        if (std::strcmp(layer.layerName, ValidationLayerName) == 0)
                        {
                            return true;
                        }
                    }

                    return false;
                }
            }

            Instance::Instance(const InstanceDesc& desc)
            {
                const std::vector<VkExtensionProperties> available =
                    GetAvailableExtensions();

                std::vector<Types::CStr> enabledExtensions;
                std::vector<Types::CStr> enabledLayers;

                // ---------------------------------------------------------
                // Поверхности (WSI). Включаем ВСЕ доступные платформенные
                // расширения: один и тот же Instance подойдёт и для
                // Wayland-окна, и для X11-окна (бэкенд окна выбирается
                // в рантайме, см. WindowBackend::Auto).
                // ---------------------------------------------------------
                if (!ContainsExtension(available, VK_KHR_SURFACE_EXTENSION_NAME))
                {
                    throw std::runtime_error(
                        "Vulkan: VK_KHR_surface is not available");
                }

                enabledExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);

#if defined(VK_USE_PLATFORM_WAYLAND_KHR)
                if (ContainsExtension(available, VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME))
                {
                    enabledExtensions.push_back(VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME);
                }
#endif

#if defined(VK_USE_PLATFORM_XCB_KHR)
                if (ContainsExtension(available, VK_KHR_XCB_SURFACE_EXTENSION_NAME))
                {
                    enabledExtensions.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
                }
#endif

                if (enabledExtensions.size() == 1)
                {
                    throw std::runtime_error(
                        "Vulkan: no platform surface extension "
                        "(VK_KHR_wayland_surface / VK_KHR_xcb_surface) is available");
                }

                // ---------------------------------------------------------
                // Validation
                // ---------------------------------------------------------
                if (desc.enableValidation)
                {
                    if (
                        HasValidationLayer() &&
                        ContainsExtension(available, VK_EXT_DEBUG_UTILS_EXTENSION_NAME))
                    {
                        enabledLayers.push_back(ValidationLayerName);
                        enabledExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
                        validation = true;
                    }
                    else
                    {
                        std::cerr
                            << "[Vulkan] Validation requested, but "
                            << ValidationLayerName
                            << " is not installed (continuing without it).\n";
                    }
                }

                // ---------------------------------------------------------
                // vkCreateInstance
                // ---------------------------------------------------------
                VkApplicationInfo app{};
                app.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
                app.pApplicationName = desc.applicationName.c_str();
                app.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
                app.pEngineName = "Empire.Engine";
                app.engineVersion = VK_MAKE_VERSION(0, 0, 1);
                app.apiVersion = VK_API_VERSION_1_1;

                VkInstanceCreateInfo info{};
                info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
                info.pApplicationInfo = &app;
                info.enabledLayerCount =
                    static_cast<Types::u32>(enabledLayers.size());
                info.ppEnabledLayerNames = enabledLayers.data();
                info.enabledExtensionCount =
                    static_cast<Types::u32>(enabledExtensions.size());
                info.ppEnabledExtensionNames = enabledExtensions.data();

                Check(vkCreateInstance(&info, nullptr, &handle), "vkCreateInstance");

                for (Types::CStr name : enabledExtensions)
                {
                    extensions.emplace_back(name);
                }

                // ---------------------------------------------------------
                // Debug messenger (ошибки validation -> stderr)
                // ---------------------------------------------------------
                if (validation)
                {
                    auto create =
                        reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
                            vkGetInstanceProcAddr(
                                handle, "vkCreateDebugUtilsMessengerEXT"));

                    if (create)
                    {
                        VkDebugUtilsMessengerCreateInfoEXT messengerInfo{};
                        messengerInfo.sType =
                            VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
                        messengerInfo.messageSeverity =
                            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
                        messengerInfo.messageType =
                            VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
                        messengerInfo.pfnUserCallback = DebugCallback;

                        // Не критично: без messenger просто не будет вывода.
                        create(handle, &messengerInfo, nullptr, &messenger);
                    }
                }
            }

            Instance::~Instance()
            {
                if (messenger != VK_NULL_HANDLE)
                {
                    auto destroy =
                        reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
                            vkGetInstanceProcAddr(
                                handle, "vkDestroyDebugUtilsMessengerEXT"));

                    if (destroy)
                    {
                        destroy(handle, messenger, nullptr);
                    }

                    messenger = VK_NULL_HANDLE;
                }

                if (handle != VK_NULL_HANDLE)
                {
                    vkDestroyInstance(handle, nullptr);
                    handle = VK_NULL_HANDLE;
                }
            }

            VkInstance Instance::Get() const
            {
                return handle;
            }

            Types::b8 Instance::HasExtension(Types::CStr name) const
            {
                for (const Types::String& extension : extensions)
                {
                    if (extension == name)
                    {
                        return true;
                    }
                }

                return false;
            }

            Types::b8 Instance::IsValidationEnabled() const
            {
                return validation;
            }
        }
    }
}
