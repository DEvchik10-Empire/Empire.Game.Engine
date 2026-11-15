#include "VkUniformSet.hpp"

namespace Engine
{
    namespace Rendering
    {
        namespace Vulkan
        {
            UniformSet::UniformSet(const Device& device, VkDeviceSize size, Types::u32 copies)
                : device(device)
            {
                try
                {
                    VkDescriptorSetLayoutBinding binding{};
                    binding.binding = 0;
                    binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                    binding.descriptorCount = 1;
                    binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

                    VkDescriptorSetLayoutCreateInfo layoutInfo{};
                    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
                    layoutInfo.bindingCount = 1;
                    layoutInfo.pBindings = &binding;

                    Check(
                        vkCreateDescriptorSetLayout(device.Get(), &layoutInfo, nullptr, &layout),
                        "vkCreateDescriptorSetLayout");

                    VkDescriptorPoolSize poolSize{};
                    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                    poolSize.descriptorCount = copies;

                    VkDescriptorPoolCreateInfo poolInfo{};
                    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
                    poolInfo.maxSets = copies;
                    poolInfo.poolSizeCount = 1;
                    poolInfo.pPoolSizes = &poolSize;

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

                        VkWriteDescriptorSet write{};
                        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                        write.dstSet = sets[i];
                        write.dstBinding = 0;
                        write.descriptorCount = 1;
                        write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                        write.pBufferInfo = &bufferInfo;

                        vkUpdateDescriptorSets(device.Get(), 1, &write, 0, nullptr);
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
