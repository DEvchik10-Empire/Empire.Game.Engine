#pragma once

#include <vector>

#include "VkCommon.hpp"

namespace Engine
{
    namespace Rendering
    {
        namespace Vulkan
        {
            struct InstanceDesc
            {
                Types::String applicationName = "Empire";

                // Слой VK_LAYER_KHRONOS_validation. Если слоя нет в системе,
                // инстанс всё равно создастся (в stderr будет предупреждение).
                Types::b8 enableValidation = false;
            };

            class Instance
            {
                public:
                    explicit Instance(const InstanceDesc& desc = InstanceDesc{});
                    ~Instance();

                    // VkInstance нельзя копировать. Перемещать тоже не нужно:
                    // держи Instance на стеке или в unique_ptr.
                    Instance(const Instance&) = delete;
                    Instance& operator=(const Instance&) = delete;
                    Instance(Instance&&) = delete;
                    Instance& operator=(Instance&&) = delete;

                    VkInstance Get() const;

                    // Какие расширения реально включены (VK_KHR_wayland_surface и т.д.).
                    Types::b8 HasExtension(Types::CStr name) const;

                    Types::b8 IsValidationEnabled() const;

                private:
                    VkInstance handle = VK_NULL_HANDLE;
                    VkDebugUtilsMessengerEXT messenger = VK_NULL_HANDLE;

                    std::vector<Types::String> extensions;
                    Types::b8 validation = false;
            };
        }
    }
}
