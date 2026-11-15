#pragma once

#include <vector>

#include "../../Empire.Texture/Vulkan/VkTexture2D.hpp"

namespace Engine
{
    namespace Rendering
    {
        namespace Vulkan
        {
            /*
             * Set 1: по одному набору на материал. Binding 0 - текстура base color
             * (fragment). Для материала без текстуры передаётся белая текстура 1x1,
             * чтобы шейдеру не нужны были ветки на уровне привязок.
             * Текстуры должны жить дольше MaterialSet.
             */
            class MaterialSet
            {
                public:
                    MaterialSet(const Device& device, const std::vector<const Texture2D*>& textures);
                    ~MaterialSet();

                    MaterialSet(const MaterialSet&) = delete;
                    MaterialSet& operator=(const MaterialSet&) = delete;

                    // Привязывает set 1 материала с индексом material.
                    void Bind(
                        VkCommandBuffer commandBuffer,
                        VkPipelineLayout pipelineLayout,
                        Types::u32 material) const;

                    Types::u32 GetCount() const;
                    VkDescriptorSetLayout GetLayout() const;

                private:
                    void Destroy();

                    const Device& device;

                    VkDescriptorSetLayout layout = VK_NULL_HANDLE;
                    VkDescriptorPool pool = VK_NULL_HANDLE;
                    std::vector<VkDescriptorSet> sets;
            };
        }
    }
}
