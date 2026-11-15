#pragma once

#include "../../Empire.Device/Vulkan/VkCommon.hpp"
#include "../../Empire.Device/Vulkan/VkDevice.hpp"
#include "../../Empire.Shader/Vulkan/VkShaderModule.hpp"

namespace Engine
{
    namespace Rendering
    {
        namespace Vulkan
        {
            struct GraphicsPipelineDesc
            {
                VkRenderPass renderPass = VK_NULL_HANDLE;
                VkDescriptorSetLayout setLayout = VK_NULL_HANDLE;

                const ShaderModule* vertexShader = nullptr;
                const ShaderModule* fragmentShader = nullptr;

                // true  -> вершины читаются из буфера в формате Engine::Rendering::Vertex
                // false -> вершинного ввода нет (полноэкранный треугольник по SV_VertexID)
                Types::b8 useVertexBuffer = true;

                Types::b8 depthTest = true;
                Types::b8 depthWrite = true;

                // Размер push constants (0 - нет). Видны в vertex-стадии.
                Types::u32 pushConstantSize = 0;
            };

            /*
             * Графический pipeline. Viewport и scissor - динамические
             * (ставит FrameContext::BeginRenderPass), поэтому resize не
             * требует пересоздавать pipeline.
             */
            class GraphicsPipeline
            {
                public:
                    GraphicsPipeline(const Device& device, const GraphicsPipelineDesc& desc);
                    ~GraphicsPipeline();

                    GraphicsPipeline(const GraphicsPipeline&) = delete;
                    GraphicsPipeline& operator=(const GraphicsPipeline&) = delete;

                    void Bind(VkCommandBuffer commandBuffer) const;

                    VkPipeline Get() const;
                    VkPipelineLayout GetLayout() const;

                private:
                    void Destroy();

                    const Device& device;

                    VkPipelineLayout layout = VK_NULL_HANDLE;
                    VkPipeline handle = VK_NULL_HANDLE;
            };
        }
    }
}
