#pragma once

#include <memory>
#include <vector>

#include "../../Empire.Device/Vulkan/VkCommon.hpp"
#include "../../Empire.Device/Vulkan/VkDevice.hpp"
#include "../../Empire.SwapChain/Vulkan/VkSwapChain.hpp"
#include "../../Empire.Texture/Vulkan/VkDepthImage.hpp"

namespace Engine
{
    namespace Rendering
    {
        namespace Vulkan
        {
            /*
             * Куда рисуется 3D-сцена: render pass (цвет + глубина),
             * буфер глубины и framebuffer'ы по числу изображений swapchain.
             *
             * После swapChain.Resize() ОБЯЗАТЕЛЬНО вызвать Rebuild().
             * Время жизни: SceneTarget умирает РАНЬШЕ SwapChain и Device
             * и ПОЗЖЕ pipeline'ов, которые создавались под его render pass.
             */
            class SceneTarget
            {
                public:
                    SceneTarget(const Device& device, const SwapChain& swapChain);
                    ~SceneTarget();

                    SceneTarget(const SceneTarget&) = delete;
                    SceneTarget& operator=(const SceneTarget&) = delete;

                    // Пересоздаёт всё, что зависит от размера (сам ждёт GPU).
                    void Rebuild();

                    VkRenderPass GetRenderPass() const;
                    VkFramebuffer GetFramebuffer(Types::u32 imageIndex) const;

                private:
                    void CreateRenderPass();
                    void CreateSizeDependent();
                    void DestroySizeDependent();
                    void Destroy();

                    const Device& device;
                    const SwapChain& swapChain;

                    VkRenderPass renderPass = VK_NULL_HANDLE;

                    std::unique_ptr<DepthImage> depth;
                    std::vector<VkImageView> colorViews;
                    std::vector<VkFramebuffer> framebuffers;
            };
        }
    }
}
