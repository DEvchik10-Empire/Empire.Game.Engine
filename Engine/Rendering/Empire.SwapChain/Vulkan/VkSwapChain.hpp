#pragma once

#include <vector>

#include "../../Empire.Device/Vulkan/VkCommon.hpp"
#include "../../Empire.Device/Vulkan/VkDevice.hpp"

#include "VkSurface.hpp"

namespace Engine
{
    namespace Rendering
    {
        namespace Vulkan
        {
            /*
             * Цепочка изображений для показа в окне.
             *
             * Изображения создаются с TRANSFER_DST (можно закрасить через
             * vkCmdClearColorImage) и COLOR_ATTACHMENT (на будущее для
             * render pass'ов).
             *
             * Размер окна Rendering сам не спрашивает: приложение передаёт
             * его в конструктор и в Resize().
             *
             * Время жизни: SwapChain умирает РАНЬШЕ Surface и Device.
             */
            class SwapChain
            {
                public:
                    SwapChain(
                        const Device& device,
                        const Surface& surface,
                        Types::u32 width,
                        Types::u32 height
                    );

                    ~SwapChain();

                    SwapChain(const SwapChain&) = delete;
                    SwapChain& operator=(const SwapChain&) = delete;
                    SwapChain(SwapChain&&) = delete;
                    SwapChain& operator=(SwapChain&&) = delete;

                    // Пересоздаёт цепочку под новый размер (сам ждёт GPU).
                    // false  -> размер 0 (окно свёрнуто), ничего не изменилось.
                    // Исключение -> реальная ошибка Vulkan.
                    Types::b8 Resize(Types::u32 width, Types::u32 height);

                    VkSwapchainKHR Get() const;
                    VkFormat GetFormat() const;
                    VkExtent2D GetExtent() const;

                    Types::u32 GetImageCount() const;
                    VkImage GetImage(Types::u32 index) const;

                    // Семафор "кадр отрисован" для конкретного изображения.
                    VkSemaphore GetRenderFinishedSemaphore(Types::u32 imageIndex) const;

                private:
                    Types::b8 Build(Types::u32 width, Types::u32 height);
                    void DestroySemaphores();
                    void Destroy();

                    const Device& device;
                    const Surface& surface;

                    VkSwapchainKHR handle = VK_NULL_HANDLE;
                    VkFormat format = VK_FORMAT_UNDEFINED;
                    VkExtent2D extent{};

                    std::vector<VkImage> images;
                    std::vector<VkSemaphore> renderFinished;
            };
        }
    }
}
