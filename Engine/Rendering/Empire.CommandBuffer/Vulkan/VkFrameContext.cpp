#include "VkFrameContext.hpp"

#include <cstdint>

namespace Engine
{
    namespace Rendering
    {
        namespace Vulkan
        {
            namespace
            {
                // Ждём изображение не бесконечно: если окно скрыто и композитор
                // перестал показывать кадры, acquire иначе завис бы навсегда.
                constexpr std::uint64_t AcquireTimeoutNs = 1'000'000'000; // 1 сек

                void TransitionImage(
                    VkCommandBuffer commandBuffer,
                    VkImage image,
                    VkImageLayout oldLayout,
                    VkImageLayout newLayout,
                    VkPipelineStageFlags srcStage,
                    VkPipelineStageFlags dstStage,
                    VkAccessFlags srcAccess,
                    VkAccessFlags dstAccess)
                {
                    VkImageMemoryBarrier barrier{};
                    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
                    barrier.srcAccessMask = srcAccess;
                    barrier.dstAccessMask = dstAccess;
                    barrier.oldLayout = oldLayout;
                    barrier.newLayout = newLayout;
                    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                    barrier.image = image;
                    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                    barrier.subresourceRange.baseMipLevel = 0;
                    barrier.subresourceRange.levelCount = 1;
                    barrier.subresourceRange.baseArrayLayer = 0;
                    barrier.subresourceRange.layerCount = 1;

                    vkCmdPipelineBarrier(
                        commandBuffer,
                        srcStage,
                        dstStage,
                        0,
                        0, nullptr,
                        0, nullptr,
                        1, &barrier);
                }
            }

            FrameContext::FrameContext(const Device& device, SwapChain& swapChain)
                : device(device),
                  swapChain(swapChain)
            {
                try
                {
                    VkCommandPoolCreateInfo poolInfo{};
                    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
                    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
                    poolInfo.queueFamilyIndex = device.GetQueueFamily();

                    Check(
                        vkCreateCommandPool(
                            device.Get(), &poolInfo, nullptr, &commandPool),
                        "vkCreateCommandPool");

                    VkCommandBuffer buffers[FramesInFlight] = {};

                    VkCommandBufferAllocateInfo allocateInfo{};
                    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
                    allocateInfo.commandPool = commandPool;
                    allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
                    allocateInfo.commandBufferCount = FramesInFlight;

                    Check(
                        vkAllocateCommandBuffers(
                            device.Get(), &allocateInfo, buffers),
                        "vkAllocateCommandBuffers");

                    VkSemaphoreCreateInfo semaphoreInfo{};
                    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

                    // SIGNALED: первый Begin() не должен вечно ждать fence,
                    // который никто ещё не отправлял.
                    VkFenceCreateInfo fenceInfo{};
                    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
                    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

                    for (Types::u32 i = 0; i < FramesInFlight; ++i)
                    {
                        frames[i].commandBuffer = buffers[i];

                        Check(
                            vkCreateSemaphore(
                                device.Get(), &semaphoreInfo, nullptr,
                                &frames[i].imageAvailable),
                            "vkCreateSemaphore");

                        Check(
                            vkCreateFence(
                                device.Get(), &fenceInfo, nullptr,
                                &frames[i].inFlight),
                            "vkCreateFence");
                    }
                }
                catch (...)
                {
                    Destroy();
                    throw;
                }
            }

            FrameContext::~FrameContext()
            {
                Destroy();
            }

            void FrameContext::Destroy()
            {
                if (device.Get() == VK_NULL_HANDLE)
                {
                    return;
                }

                vkDeviceWaitIdle(device.Get());

                for (PerFrame& frame : frames)
                {
                    if (frame.inFlight != VK_NULL_HANDLE)
                    {
                        vkDestroyFence(device.Get(), frame.inFlight, nullptr);
                        frame.inFlight = VK_NULL_HANDLE;
                    }

                    if (frame.imageAvailable != VK_NULL_HANDLE)
                    {
                        vkDestroySemaphore(device.Get(), frame.imageAvailable, nullptr);
                        frame.imageAvailable = VK_NULL_HANDLE;
                    }

                    // Командные буферы освобождаются вместе с пулом.
                    frame.commandBuffer = VK_NULL_HANDLE;
                }

                if (commandPool != VK_NULL_HANDLE)
                {
                    vkDestroyCommandPool(device.Get(), commandPool, nullptr);
                    commandPool = VK_NULL_HANDLE;
                }
            }

            FrameStatus FrameContext::Begin(FrameInfo& frame)
            {
                PerFrame& slot = frames[current];

                // Ждём, пока GPU закончит кадр, который раньше использовал этот слот.
                Check(
                    vkWaitForFences(
                        device.Get(), 1, &slot.inFlight, VK_TRUE, UINT64_MAX),
                    "vkWaitForFences");

                Types::u32 imageIndex = 0;

                const VkResult acquired = vkAcquireNextImageKHR(
                    device.Get(),
                    swapChain.Get(),
                    AcquireTimeoutNs,
                    slot.imageAvailable,
                    VK_NULL_HANDLE,
                    &imageIndex);

                if (acquired == VK_ERROR_OUT_OF_DATE_KHR)
                {
                    return FrameStatus::OutOfDate;
                }

                if (acquired == VK_TIMEOUT || acquired == VK_NOT_READY)
                {
                    return FrameStatus::Skipped;
                }

                if (acquired != VK_SUCCESS && acquired != VK_SUBOPTIMAL_KHR)
                {
                    Check(acquired, "vkAcquireNextImageKHR");
                }

                suboptimal = (acquired == VK_SUBOPTIMAL_KHR);

                // Fence сбрасываем только когда точно будем отправлять работу,
                // иначе следующий Begin() завис бы на несигнальном fence.
                Check(
                    vkResetFences(device.Get(), 1, &slot.inFlight),
                    "vkResetFences");

                Check(
                    vkResetCommandBuffer(slot.commandBuffer, 0),
                    "vkResetCommandBuffer");

                VkCommandBufferBeginInfo beginInfo{};
                beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
                beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

                Check(
                    vkBeginCommandBuffer(slot.commandBuffer, &beginInfo),
                    "vkBeginCommandBuffer");

                frame.frameIndex = current;
                frame.imageIndex = imageIndex;
                frame.image = swapChain.GetImage(imageIndex);
                frame.extent = swapChain.GetExtent();
                frame.commandBuffer = slot.commandBuffer;

                return FrameStatus::Ok;
            }

            void FrameContext::ClearColor(
                const FrameInfo& frame,
                Types::f32 red,
                Types::f32 green,
                Types::f32 blue,
                Types::f32 alpha)
            {
                // Новое изображение: содержимое нам не важно (UNDEFINED),
                // мы его целиком перезапишем.
                // srcStage = TRANSFER: именно на этой стадии submit ждёт
                // семафор imageAvailable, барьер должен идти после него.
                TransitionImage(
                    frame.commandBuffer,
                    frame.image,
                    VK_IMAGE_LAYOUT_UNDEFINED,
                    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                    VK_PIPELINE_STAGE_TRANSFER_BIT,
                    VK_PIPELINE_STAGE_TRANSFER_BIT,
                    0,
                    VK_ACCESS_TRANSFER_WRITE_BIT);

                VkClearColorValue color{};
                color.float32[0] = red;
                color.float32[1] = green;
                color.float32[2] = blue;
                color.float32[3] = alpha;

                VkImageSubresourceRange range{};
                range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                range.baseMipLevel = 0;
                range.levelCount = 1;
                range.baseArrayLayer = 0;
                range.layerCount = 1;

                vkCmdClearColorImage(
                    frame.commandBuffer,
                    frame.image,
                    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                    &color,
                    1,
                    &range);

                // Готово к показу.
                TransitionImage(
                    frame.commandBuffer,
                    frame.image,
                    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                    VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                    VK_PIPELINE_STAGE_TRANSFER_BIT,
                    VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                    VK_ACCESS_TRANSFER_WRITE_BIT,
                    0);
            }

            void FrameContext::BeginRenderPass(
                const FrameInfo& frame,
                VkRenderPass renderPass,
                VkFramebuffer framebuffer,
                Types::f32 red,
                Types::f32 green,
                Types::f32 blue,
                Types::f32 alpha)
            {
                VkClearValue clears[2]{};
                clears[0].color = {{red, green, blue, alpha}};
                clears[1].depthStencil = {1.0f, 0};

                VkRenderPassBeginInfo info{};
                info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
                info.renderPass = renderPass;
                info.framebuffer = framebuffer;
                info.renderArea.offset = {0, 0};
                info.renderArea.extent = frame.extent;
                info.clearValueCount = 2;
                info.pClearValues = clears;

                vkCmdBeginRenderPass(frame.commandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);

                VkViewport viewport{};
                viewport.x = 0.0f;
                viewport.y = 0.0f;
                viewport.width = static_cast<float>(frame.extent.width);
                viewport.height = static_cast<float>(frame.extent.height);
                viewport.minDepth = 0.0f;
                viewport.maxDepth = 1.0f;

                VkRect2D scissor{};
                scissor.extent = frame.extent;

                vkCmdSetViewport(frame.commandBuffer, 0, 1, &viewport);
                vkCmdSetScissor(frame.commandBuffer, 0, 1, &scissor);
            }

            void FrameContext::EndRenderPass(const FrameInfo& frame)
            {
                vkCmdEndRenderPass(frame.commandBuffer);
            }

            FrameStatus FrameContext::End(const FrameInfo& frame)
            {
                PerFrame& slot = frames[frame.frameIndex];

                Check(
                    vkEndCommandBuffer(slot.commandBuffer),
                    "vkEndCommandBuffer");

                const VkSemaphore renderFinished =
                    swapChain.GetRenderFinishedSemaphore(frame.imageIndex);

                // Изображение нужно либо для закраски (TRANSFER), либо для
                // render pass (COLOR_ATTACHMENT_OUTPUT): ждём на обеих стадиях.
                const VkPipelineStageFlags waitStage =
                    VK_PIPELINE_STAGE_TRANSFER_BIT |
                    VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

                VkSubmitInfo submitInfo{};
                submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
                submitInfo.waitSemaphoreCount = 1;
                submitInfo.pWaitSemaphores = &slot.imageAvailable;
                submitInfo.pWaitDstStageMask = &waitStage;
                submitInfo.commandBufferCount = 1;
                submitInfo.pCommandBuffers = &slot.commandBuffer;
                submitInfo.signalSemaphoreCount = 1;
                submitInfo.pSignalSemaphores = &renderFinished;

                Check(
                    vkQueueSubmit(device.GetQueue(), 1, &submitInfo, slot.inFlight),
                    "vkQueueSubmit");

                VkSwapchainKHR swapchainHandle = swapChain.Get();
                Types::u32 imageIndex = frame.imageIndex;

                VkPresentInfoKHR presentInfo{};
                presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
                presentInfo.waitSemaphoreCount = 1;
                presentInfo.pWaitSemaphores = &renderFinished;
                presentInfo.swapchainCount = 1;
                presentInfo.pSwapchains = &swapchainHandle;
                presentInfo.pImageIndices = &imageIndex;

                const VkResult presented =
                    vkQueuePresentKHR(device.GetQueue(), &presentInfo);

                // Следующий кадр возьмёт другой слот.
                current = (current + 1) % FramesInFlight;

                if (
                    presented == VK_ERROR_OUT_OF_DATE_KHR ||
                    presented == VK_SUBOPTIMAL_KHR ||
                    suboptimal)
                {
                    suboptimal = false;
                    return FrameStatus::OutOfDate;
                }

                Check(presented, "vkQueuePresentKHR");

                return FrameStatus::Ok;
            }
        }
    }
}
