#include "VkSceneTarget.hpp"

namespace Engine
{
    namespace Rendering
    {
        namespace Vulkan
        {
            SceneTarget::SceneTarget(const Device& device, const SwapChain& swapChain)
                : device(device),
                  swapChain(swapChain)
            {
                try
                {
                    CreateRenderPass();
                    CreateSizeDependent();
                }
                catch (...)
                {
                    Destroy();
                    throw;
                }
            }

            SceneTarget::~SceneTarget()
            {
                Destroy();
            }

            void SceneTarget::CreateRenderPass()
            {
                VkAttachmentDescription color{};
                color.format = swapChain.GetFormat();
                color.samples = VK_SAMPLE_COUNT_1_BIT;
                color.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
                color.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
                color.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                color.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                color.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                color.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

                VkAttachmentDescription depthAttachment{};
                depthAttachment.format = device.FindDepthFormat();
                depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
                depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
                depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

                VkAttachmentReference colorRef{};
                colorRef.attachment = 0;
                colorRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

                VkAttachmentReference depthRef{};
                depthRef.attachment = 1;
                depthRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

                VkSubpassDescription subpass{};
                subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
                subpass.colorAttachmentCount = 1;
                subpass.pColorAttachments = &colorRef;
                subpass.pDepthStencilAttachment = &depthRef;

                // Ждём (а) изображение от swapchain и (б) конец предыдущего кадра
                // по буферу глубины (он один на все кадры "в полёте").
                VkSubpassDependency dependency{};
                dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
                dependency.dstSubpass = 0;
                dependency.srcStageMask =
                    VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                    VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT |
                    VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
                dependency.dstStageMask =
                    VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                    VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
                dependency.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
                dependency.dstAccessMask =
                    VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
                    VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

                const VkAttachmentDescription attachments[] = {color, depthAttachment};

                VkRenderPassCreateInfo info{};
                info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
                info.attachmentCount = 2;
                info.pAttachments = attachments;
                info.subpassCount = 1;
                info.pSubpasses = &subpass;
                info.dependencyCount = 1;
                info.pDependencies = &dependency;

                Check(vkCreateRenderPass(device.Get(), &info, nullptr, &renderPass), "vkCreateRenderPass");
            }

            void SceneTarget::CreateSizeDependent()
            {
                const VkExtent2D extent = swapChain.GetExtent();

                depth = std::make_unique<DepthImage>(device, extent.width, extent.height);

                const Types::u32 count = swapChain.GetImageCount();

                colorViews.assign(count, VK_NULL_HANDLE);
                framebuffers.assign(count, VK_NULL_HANDLE);

                for (Types::u32 i = 0; i < count; ++i)
                {
                    VkImageViewCreateInfo viewInfo{};
                    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
                    viewInfo.image = swapChain.GetImage(i);
                    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
                    viewInfo.format = swapChain.GetFormat();
                    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                    viewInfo.subresourceRange.levelCount = 1;
                    viewInfo.subresourceRange.layerCount = 1;

                    Check(
                        vkCreateImageView(device.Get(), &viewInfo, nullptr, &colorViews[i]),
                        "vkCreateImageView(color)");

                    const VkImageView attachments[] = {colorViews[i], depth->GetView()};

                    VkFramebufferCreateInfo fbInfo{};
                    fbInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
                    fbInfo.renderPass = renderPass;
                    fbInfo.attachmentCount = 2;
                    fbInfo.pAttachments = attachments;
                    fbInfo.width = extent.width;
                    fbInfo.height = extent.height;
                    fbInfo.layers = 1;

                    Check(
                        vkCreateFramebuffer(device.Get(), &fbInfo, nullptr, &framebuffers[i]),
                        "vkCreateFramebuffer");
                }
            }

            void SceneTarget::DestroySizeDependent()
            {
                for (VkFramebuffer framebuffer : framebuffers)
                {
                    if (framebuffer != VK_NULL_HANDLE)
                    {
                        vkDestroyFramebuffer(device.Get(), framebuffer, nullptr);
                    }
                }

                for (VkImageView view : colorViews)
                {
                    if (view != VK_NULL_HANDLE)
                    {
                        vkDestroyImageView(device.Get(), view, nullptr);
                    }
                }

                framebuffers.clear();
                colorViews.clear();
                depth.reset();
            }

            void SceneTarget::Rebuild()
            {
                vkDeviceWaitIdle(device.Get());

                DestroySizeDependent();
                CreateSizeDependent();
            }

            void SceneTarget::Destroy()
            {
                vkDeviceWaitIdle(device.Get());

                DestroySizeDependent();

                if (renderPass != VK_NULL_HANDLE)
                {
                    vkDestroyRenderPass(device.Get(), renderPass, nullptr);
                    renderPass = VK_NULL_HANDLE;
                }
            }

            VkRenderPass SceneTarget::GetRenderPass() const
            {
                return renderPass;
            }

            VkFramebuffer SceneTarget::GetFramebuffer(Types::u32 imageIndex) const
            {
                return framebuffers[imageIndex];
            }
        }
    }
}
