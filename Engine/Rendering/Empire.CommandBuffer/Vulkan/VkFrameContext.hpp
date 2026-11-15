#pragma once

#include "../../Empire.Device/Vulkan/VkCommon.hpp"
#include "../../Empire.Device/Vulkan/VkDevice.hpp"
#include "../../Empire.SwapChain/Vulkan/VkSwapChain.hpp"

namespace Engine
{
    namespace Rendering
    {
        namespace Vulkan
        {
            // Что произошло с кадром.
            enum class FrameStatus
            {
                Ok,

                // Swapchain устарел (окно поменяло размер): вызови
                // SwapChain::Resize() и начни кадр заново.
                OutOfDate,

                // Кадр пропущен (например, окно скрыто и показ стоит).
                // Просто пробуй в следующем проходе цикла.
                Skipped
            };

            // Данные одного кадра. Заполняет Begin(), читают ClearColor/End.
            struct FrameInfo
            {
                Types::u32 frameIndex = 0;      // 0..FramesInFlight-1
                Types::u32 imageIndex = 0;      // какое изображение swapchain
                VkImage image = VK_NULL_HANDLE;
                VkExtent2D extent{};
                VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
            };

            /*
             * Командный пул, командные буферы и синхронизация кадров.
             *
             * Цикл кадра:
             *
             *     FrameInfo frame;
             *     FrameStatus status = frames.Begin(frame);
             *
             *     if (status == FrameStatus::OutOfDate) { swapChain.Resize(w, h); continue; }
             *     if (status == FrameStatus::Skipped)   { continue; }
             *
             *     frames.ClearColor(frame, r, g, b, a);    // здесь будет рендер
             *
             *     if (frames.End(frame) == FrameStatus::OutOfDate)
             *         swapChain.Resize(w, h);
             *
             * Время жизни: FrameContext умирает РАНЬШЕ SwapChain и Device.
             */
            class FrameContext
            {
                public:
                    // Сколько кадров CPU может готовить, пока GPU рисует предыдущие.
                    static constexpr Types::u32 FramesInFlight = 2;

                    FrameContext(const Device& device, SwapChain& swapChain);
                    ~FrameContext();

                    FrameContext(const FrameContext&) = delete;
                    FrameContext& operator=(const FrameContext&) = delete;
                    FrameContext(FrameContext&&) = delete;
                    FrameContext& operator=(FrameContext&&) = delete;

                    FrameStatus Begin(FrameInfo& frame);

                    // Закрашивает изображение кадра одним цветом (0..1)
                    // и переводит его в состояние "готово к показу".
                    void ClearColor(
                        const FrameInfo& frame,
                        Types::f32 red,
                        Types::f32 green,
                        Types::f32 blue,
                        Types::f32 alpha
                    );

                    // Начинает render pass на всё изображение: очищает цвет и глубину,
                    // ставит viewport и scissor. Изображение после EndRenderPass
                    // сразу готово к показу (finalLayout = PRESENT_SRC).
                    void BeginRenderPass(
                        const FrameInfo& frame,
                        VkRenderPass renderPass,
                        VkFramebuffer framebuffer,
                        Types::f32 red,
                        Types::f32 green,
                        Types::f32 blue,
                        Types::f32 alpha
                    );

                    void EndRenderPass(const FrameInfo& frame);

                    FrameStatus End(const FrameInfo& frame);

                private:
                    struct PerFrame
                    {
                        VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
                        VkSemaphore imageAvailable = VK_NULL_HANDLE;
                        VkFence inFlight = VK_NULL_HANDLE;
                    };

                    void Destroy();

                    const Device& device;
                    SwapChain& swapChain;

                    VkCommandPool commandPool = VK_NULL_HANDLE;
                    PerFrame frames[FramesInFlight];

                    Types::u32 current = 0;

                    // acquire вернул SUBOPTIMAL: показать можно, но пора пересоздать.
                    Types::b8 suboptimal = false;
            };
        }
    }
}
