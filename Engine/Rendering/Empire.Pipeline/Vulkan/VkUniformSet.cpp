#include "VkUniformSet.hpp"

namespace Engine
{
    namespace Rendering
    {
        namespace Vulkan
        {
            UniformSet::UniformSet(
                const Device& device,
                VkDeviceSize size,
                Types::u32 copies,
                const std::vector<const Texture2D*>& textures)
                : device(device)
            {
                try
                {
                    std::vector<VkDescriptorSetLayoutBinding> bindings(1 + textures.size());

                    bindings[0].binding = 0;
                    bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                    bindings[0].descriptorCount = 1;
                    bindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

                    for (std::size_t t = 0; t < textures.size(); ++t)
                    {
                        bindings[1 + t].binding = static_cast<Types::u32>(1 + t);
                        bindings[1 + t].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                        bindings[1 + t].descriptorCount = 1;
                        bindings[1 + t].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
                    }

                    VkDescriptorSetLayoutCreateInfo layoutInfo{};
                    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
                    layoutInfo.bindingCount = static_cast<Types::u32>(bindings.size());
                    layoutInfo.pBindings = bindings.data();

                    Check(
                        vkCreateDescriptorSetLayout(device.Get(), &layoutInfo, nullptr, &layout),
                        "vkCreateDescriptorSetLayout");

                    std::vector<VkDescriptorPoolSize> poolSizes;
                    poolSizes.push_back({VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, copies});

                    if (!textures.empty())
                    {
                        poolSizes.push_back({
                            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                            copies * static_cast<Types::u32>(textures.size())});
                    }

                    VkDescriptorPoolCreateInfo poolInfo{};
                    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
                    poolInfo.maxSets = copies;
                    poolInfo.poolSizeCount = static_cast<Types::u32>(poolSizes.size());
                    poolInfo.pPoolSizes = poolSizes.data();

                    Check(
                        vkCreateDescriptorPool(device.Get(), &poolInfo, nullptr, &pool),
                        "vkCreateDescriptorPool");

                    std::vector<VkDescriptorSetLayout> layouts(copies, layout);

                    VkDescriptorSetAllocateInfo allocate{};
                    allocate.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
                    allocate.descriptorPool = pool;
                    allocate.descriptorSetCount = copies;
                    allocate.pSetLayouts = layouts.data();

                    sets.resize(copies);

                    Check(
                        vkAllocateDescriptorSets(device.Get(), &allocate, sets.data()),
                        "vkAllocateDescriptorSets");

                    for (Types::u32 i = 0; i < copies; ++i)
                    {
                        buffers.push_back(std::make_unique<Buffer>(
                            device, size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT));

                        VkDescriptorBufferInfo bufferInfo{};
                        bufferInfo.buffer = buffers[i]->Get();
                        bufferInfo.offset = 0;
                        bufferInfo.range = size;

                        std::vector<VkDescriptorImageInfo> imageInfos(textures.size());
                        std::vector<VkWriteDescriptorSet> writes(1 + textures.size());

                        writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                        writes[0].dstSet = sets[i];
                        writes[0].dstBinding = 0;
                        writes[0].descriptorCount = 1;
                        writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                        writes[0].pBufferInfo = &bufferInfo;

                        for (std::size_t t = 0; t < textures.size(); ++t)
                        {
                            imageInfos[t].sampler = textures[t]->GetSampler();
                            imageInfos[t].imageView = textures[t]->GetView();
                            imageInfos[t].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

                            writes[1 + t].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                            writes[1 + t].dstSet = sets[i];
                            writes[1 + t].dstBinding = static_cast<Types::u32>(1 + t);
                            writes[1 + t].descriptorCount = 1;
                            writes[1 + t].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                            writes[1 + t].pImageInfo = &imageInfos[t];
                        }

                        vkUpdateDescriptorSets(
                            device.Get(), static_cast<Types::u32>(writes.size()), writes.data(), 0, nullptr);
                    }
                }
                catch (...)
                {
                    Destroy();
                    throw;
                }
            }

            UniformSet::~UniformSet()
            {
                Destroy();
            }

            void UniformSet::Destroy()
            {
                buffers.clear();

                // Наборы освобождаются вместе с пулом.
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
            }

            void UniformSet::Update(Types::u32 frameIndex, const void* data, VkDeviceSize size)
            {
                buffers[frameIndex]->Upload(data, size);
            }

            void UniformSet::Bind(
                VkCommandBuffer commandBuffer,
                VkPipelineLayout pipelineLayout,
                Types::u32 frameIndex) const
            {
                vkCmdBindDescriptorSets(
                    commandBuffer,
                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                    pipelineLayout,
                    0, 1, &sets[frameIndex],
                    0, nullptr);
            }

            VkDescriptorSetLayout UniformSet::GetLayout() const
            {
                return layout;
            }
        }
    }
}
