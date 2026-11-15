#pragma once

#include <memory>
#include <vector>

#include "../../Empire.Buffer/Vulkan/VkBuffer.hpp"
#include "../../Empire.Texture/Vulkan/VkTexture2D.hpp"

namespace Engine
{
    namespace Rendering
    {
        namespace Vulkan
        {
            /*
             * Set 0: uniform-буфер кадра (binding 0; vertex + fragment) по одному
             * на каждый кадр "в полёте" - чтобы CPU не перезаписывал данные,
             * которые GPU ещё читает. Плюс текстуры (binding 1, 2, ... в порядке
             * списка; только fragment), общие для всех кадров.
             * Текстуры должны жить дольше UniformSet.
             */
            class UniformSet
            {
                public:
                    UniformSet(
                        const Device& device,
                        VkDeviceSize size,
                        Types::u32 copies,
                        const std::vector<const Texture2D*>& textures = {}
                    );
                    ~UniformSet();

                    UniformSet(const UniformSet&) = delete;
                    UniformSet& operator=(const UniformSet&) = delete;

                    void Update(Types::u32 frameIndex, const void* data, VkDeviceSize size);

                    // Привязывает set 0 для указанного pipeline layout.
                    void Bind(
                        VkCommandBuffer commandBuffer,
                        VkPipelineLayout pipelineLayout,
                        Types::u32 frameIndex) const;

                    VkDescriptorSetLayout GetLayout() const;

                private:
                    void Destroy();

                    const Device& device;

                    VkDescriptorSetLayout layout = VK_NULL_HANDLE;
                    VkDescriptorPool pool = VK_NULL_HANDLE;

                    std::vector<std::unique_ptr<Buffer>> buffers;
                    std::vector<VkDescriptorSet> sets;
            };
        }
    }
}
