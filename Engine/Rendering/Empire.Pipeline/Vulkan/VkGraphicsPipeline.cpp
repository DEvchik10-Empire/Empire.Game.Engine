#include "VkGraphicsPipeline.hpp"

#include <cstddef>
#include <stdexcept>

#include "../../Empire.Mesh/Mesh.hpp"

namespace Engine
{
    namespace Rendering
    {
        namespace Vulkan
        {
            GraphicsPipeline::GraphicsPipeline(const Device& device, const GraphicsPipelineDesc& desc)
                : device(device)
            {
                if (!desc.vertexShader || !desc.fragmentShader)
                {
                    throw std::runtime_error("GraphicsPipeline: shaders are missing");
                }

                try
                {
                    // ---- layout ----
                    VkPushConstantRange push{};
                    push.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
                    push.offset = 0;
                    push.size = desc.pushConstantSize;

                    VkPipelineLayoutCreateInfo layoutInfo{};
                    layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
                    // Set 0 - кадр (uniform + шумы неба), set 1 - материал (если задан).
                    VkDescriptorSetLayout setLayouts[2] = {desc.setLayout, desc.materialLayout};
                    layoutInfo.setLayoutCount =
                        desc.setLayout == VK_NULL_HANDLE ? 0 : (desc.materialLayout != VK_NULL_HANDLE ? 2 : 1);
                    layoutInfo.pSetLayouts = setLayouts;
                    layoutInfo.pushConstantRangeCount = desc.pushConstantSize > 0 ? 1 : 0;
                    layoutInfo.pPushConstantRanges = &push;

                    Check(
                        vkCreatePipelineLayout(device.Get(), &layoutInfo, nullptr, &layout),
                        "vkCreatePipelineLayout");

                    // ---- шейдеры (точка входа в HLSL всегда main) ----
                    VkPipelineShaderStageCreateInfo stages[2]{};

                    stages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
                    stages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
                    stages[0].module = desc.vertexShader->Get();
                    stages[0].pName = "main";

                    stages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
                    stages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
                    stages[1].module = desc.fragmentShader->Get();
                    stages[1].pName = "main";

                    // ---- вершинный ввод ----
                    VkVertexInputBindingDescription binding{};
                    binding.binding = 0;
                    binding.stride = sizeof(Vertex);
                    binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

                    // location 0/1/2 = POSITION / NORMAL / TEXCOORD0 в Lit.vs.hlsl
                    VkVertexInputAttributeDescription attributes[3]{};
                    attributes[0] = {0, 0, VK_FORMAT_R32G32B32_SFLOAT, static_cast<Types::u32>(offsetof(Vertex, position))};
                    attributes[1] = {1, 0, VK_FORMAT_R32G32B32_SFLOAT, static_cast<Types::u32>(offsetof(Vertex, normal))};
                    attributes[2] = {2, 0, VK_FORMAT_R32G32_SFLOAT,    static_cast<Types::u32>(offsetof(Vertex, uv))};

                    VkPipelineVertexInputStateCreateInfo vertexInput{};
                    vertexInput.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

                    if (desc.useVertexBuffer)
                    {
                        vertexInput.vertexBindingDescriptionCount = 1;
                        vertexInput.pVertexBindingDescriptions = &binding;
                        vertexInput.vertexAttributeDescriptionCount = 3;
                        vertexInput.pVertexAttributeDescriptions = attributes;
                    }

                    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
                    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
                    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

                    VkPipelineViewportStateCreateInfo viewport{};
                    viewport.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
                    viewport.viewportCount = 1;
                    viewport.scissorCount = 1;

                    // Culling выключен: так работают и произвольные .obj
                    // с любым порядком вершин, и полноэкранный треугольник.
                    VkPipelineRasterizationStateCreateInfo raster{};
                    raster.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
                    raster.polygonMode = VK_POLYGON_MODE_FILL;
                    raster.cullMode = VK_CULL_MODE_NONE;
                    raster.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
                    raster.lineWidth = 1.0f;

                    VkPipelineMultisampleStateCreateInfo multisample{};
                    multisample.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
                    multisample.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

                    VkPipelineDepthStencilStateCreateInfo depth{};
                    depth.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
                    depth.depthTestEnable = desc.depthTest ? VK_TRUE : VK_FALSE;
                    depth.depthWriteEnable = desc.depthWrite ? VK_TRUE : VK_FALSE;
                    depth.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;

                    VkPipelineColorBlendAttachmentState blendAttachment{};
                    blendAttachment.colorWriteMask =
                        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

                    VkPipelineColorBlendStateCreateInfo blend{};
                    blend.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
                    blend.attachmentCount = 1;
                    blend.pAttachments = &blendAttachment;

                    const VkDynamicState dynamicStates[] =
                    {
                        VK_DYNAMIC_STATE_VIEWPORT,
                        VK_DYNAMIC_STATE_SCISSOR
                    };

                    VkPipelineDynamicStateCreateInfo dynamic{};
                    dynamic.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
                    dynamic.dynamicStateCount = 2;
                    dynamic.pDynamicStates = dynamicStates;

                    VkGraphicsPipelineCreateInfo info{};
                    info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
                    info.stageCount = 2;
                    info.pStages = stages;
                    info.pVertexInputState = &vertexInput;
                    info.pInputAssemblyState = &inputAssembly;
                    info.pViewportState = &viewport;
                    info.pRasterizationState = &raster;
                    info.pMultisampleState = &multisample;
                    info.pDepthStencilState = &depth;
                    info.pColorBlendState = &blend;
                    info.pDynamicState = &dynamic;
                    info.layout = layout;
                    info.renderPass = desc.renderPass;
                    info.subpass = 0;

                    Check(
                        vkCreateGraphicsPipelines(
                            device.Get(), VK_NULL_HANDLE, 1, &info, nullptr, &handle),
                        "vkCreateGraphicsPipelines");
                }
                catch (...)
                {
                    Destroy();
                    throw;
                }
            }

            GraphicsPipeline::~GraphicsPipeline()
            {
                Destroy();
            }

            void GraphicsPipeline::Destroy()
            {
                if (handle != VK_NULL_HANDLE)
                {
                    vkDestroyPipeline(device.Get(), handle, nullptr);
                    handle = VK_NULL_HANDLE;
                }

                if (layout != VK_NULL_HANDLE)
                {
                    vkDestroyPipelineLayout(device.Get(), layout, nullptr);
                    layout = VK_NULL_HANDLE;
                }
            }

            void GraphicsPipeline::Bind(VkCommandBuffer commandBuffer) const
            {
                vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, handle);
            }

            VkPipeline GraphicsPipeline::Get() const
            {
                return handle;
            }

            VkPipelineLayout GraphicsPipeline::GetLayout() const
            {
                return layout;
            }
        }
    }
}
