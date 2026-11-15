#include "VkMaterialSet.hpp"

#include <stdexcept>

namespace Engine
{
    namespace Rendering
    {
        namespace Vulkan
        {
            MaterialSet::MaterialSet(const Device& device, const std::vector<const Texture2D*>& textures)
                : device(device)
            {
                if (textures.empty())
                {
                    throw std::runtime_error("MaterialSet: no materials");
                }

                try
                {
                    VkDescriptorSetLayoutBinding binding{};
                    binding.binding = 0;
                    binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                    binding.descriptorCount = 1;
                    binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

                    VkDescriptorSetLayoutCreateInfo layoutInfo{};
                    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
                    layoutInfo.bindingCount = 1;
                    layoutInfo.pBindings = &binding;

                    Check(
                        vkCreateDescriptorSetLayout(device.Get(), &layoutInfo, nullptr, &layout),
                        "vkCreateDescriptorSetLayout(material)");

                    const Types::u32 count = static_cast<Types::u32>(textures.size());

                    VkDescriptorPoolSize poolSize{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, count};

                    VkDescriptorPoolCreateInfo poolInfo{};
                    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
                    poolInfo.maxSets = count;
                    poolInfo.poolSizeCount = 1;
                    poolInfo.pPoolSizes = &poolSize;

                    Check(
                        vkCreateDescriptorPool(device.Get(), &poolInfo, nullptr, &pool),
                        "vkCreateDescriptorPool(material)");

                    std::vector<VkDescriptorSetLayout> layouts(count, layout);

                    VkDescriptorSetAllocateInfo allocate{};
                    allocate.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
                    allocate.descriptorPool = pool;
                    allocate.descriptorSetCount = count;
                    allocate.pSetLayouts = layouts.data();

                    sets.resize(count);

                    Check(
                        vkAllocateDescriptorSets(device.Get(), &allocate, sets.data()),
                        "vkAllocateDescriptorSets(material)");

                    for (Types::u32 i = 0; i < count; ++i)
                    {
                        VkDescriptorImageInfo imageInfo{};
                        imageInfo.sampler = textures[i]->GetSampler();
                        imageInfo.imageView = textures[i]->GetView();
                        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

                        VkWriteDescriptorSet write{};
                        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                        write.dstSet = sets[i];
                        write.dstBinding = 0;
                        write.descriptorCount = 1;
                        write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                        write.pImageInfo = &imageInfo;

                        vkUpdateDescriptorSets(device.Get(), 1, &write, 0, nullptr);
                    }
                }
                catch (...)
                {
                    Destroy();
                    throw;
                }
            }

            MaterialSet::~MaterialSet()
            {
                Destroy();
            }

            void MaterialSet::Destroy()
            {
                if (pool != VK_NULL_HANDLE)
                {
                    vkDestroyDescriptorPool(device.Get(), pool, nullptr);
                    pool = VK_NULL_HANDLE;
                }

                if (layout != VK_NULL_HANDLE)
                {
                    vkDestroyDescriptorSetLayout(device.Get(), layout, nullptr);
                    layout = VK_NULL_HANDLE;
                }

                sets.clear();
            }

            void MaterialSet::Bind(
                VkCommandBuffer commandBuffer,
                VkPipelineLayout pipelineLayout,
                Types::u32 material) const
            {
                if (material >= sets.size())
                {
                    material = 0;
                }

                vkCmdBindDescriptorSets(
                    commandBuffer,
                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                    pipelineLayout,
                    1, 1, &sets[material],
                    0, nullptr);
            }

            Types::u32 MaterialSet::GetCount() const
            {
                return static_cast<Types::u32>(sets.size());
            }

            VkDescriptorSetLayout MaterialSet::GetLayout() const
            {
                return layout;
            }
        }
    }
}
