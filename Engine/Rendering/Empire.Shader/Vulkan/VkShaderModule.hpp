#pragma once

#include "../../Empire.Device/Vulkan/VkCommon.hpp"
#include "../../Empire.Device/Vulkan/VkDevice.hpp"

namespace Engine
{
    namespace Rendering
    {
        namespace Vulkan
        {
            // SPIR-V шейдер, загруженный из .spv файла
            // (получается из HLSL: см. папку Shaders/ в корне движка).
            class ShaderModule
            {
                public:
                    ShaderModule(const Device& device, const Types::String& spvPath);
                    ~ShaderModule();

                    ShaderModule(const ShaderModule&) = delete;
                    ShaderModule& operator=(const ShaderModule&) = delete;

                    VkShaderModule Get() const;

                private:
                    const Device& device;
                    VkShaderModule handle = VK_NULL_HANDLE;
            };
        }
    }
}
