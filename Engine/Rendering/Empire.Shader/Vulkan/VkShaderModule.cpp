#include "VkShaderModule.hpp"

#include <cstring>
#include <fstream>
#include <stdexcept>
#include <vector>

namespace Engine
{
    namespace Rendering
    {
        namespace Vulkan
        {
            ShaderModule::ShaderModule(const Device& device, const Types::String& spvPath)
                : device(device)
            {
                std::ifstream file(spvPath.c_str(), std::ios::binary | std::ios::ate);

                if (!file)
                {
                    throw std::runtime_error(
                        std::string("Shader not found: ") + spvPath.c_str() +
                        " (compile HLSL first: Shaders/compile.sh or xmake build Empire.Shaders)");
                }

                const std::streamsize bytes = file.tellg();

                if (bytes <= 0 || bytes % 4 != 0)
                {
                    throw std::runtime_error(std::string("Invalid SPIR-V file: ") + spvPath.c_str());
                }

                // SPIR-V - массив 32-битных слов; vector<u32> даёт нужное выравнивание.
                std::vector<Types::u32> code(static_cast<std::size_t>(bytes) / 4);

                file.seekg(0);
                file.read(reinterpret_cast<char*>(code.data()), bytes);

                VkShaderModuleCreateInfo info{};
                info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
                info.codeSize = static_cast<std::size_t>(bytes);
                info.pCode = code.data();

                Check(vkCreateShaderModule(device.Get(), &info, nullptr, &handle), "vkCreateShaderModule");
            }

            ShaderModule::~ShaderModule()
            {
                if (handle != VK_NULL_HANDLE)
                {
                    vkDestroyShaderModule(device.Get(), handle, nullptr);
                    handle = VK_NULL_HANDLE;
                }
            }

            VkShaderModule ShaderModule::Get() const
            {
                return handle;
            }
        }
    }
}
