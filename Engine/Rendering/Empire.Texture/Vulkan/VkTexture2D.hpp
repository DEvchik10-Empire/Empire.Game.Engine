#pragma once

#include "../../Empire.Device/Vulkan/VkCommon.hpp"
#include "../../Empire.Device/Vulkan/VkDevice.hpp"
#include "../NoiseImage.hpp"

namespace Engine
{
    namespace Rendering
    {
        namespace Vulkan
        {
            /*
             * Одноканальная 2D-текстура (R8) для чтения в шейдере.
             * Данные заливаются через staging-буфер при создании; билинейная
             * фильтрация, адресация repeat или clamp. mipmaps = true: цепочка
             * уровней считается на CPU (усреднение 2x2), фильтрация трилинейная
             * (в шейдере уровень задаётся через SampleLevel).
             *
             * Время жизни: Texture2D умирает РАНЬШЕ Device.
             */
            class Texture2D
            {
                public:
                    Texture2D(const Device& device, const ImageR8& image, Types::b8 repeat, Types::b8 mipmaps = false);
                    ~Texture2D();

                    Texture2D(const Texture2D&) = delete;
                    Texture2D& operator=(const Texture2D&) = delete;

                    VkImageView GetView() const;
                    VkSampler GetSampler() const;

                private:
                    void Destroy();

                    const Device& device;

                    VkImage image = VK_NULL_HANDLE;
                    VkDeviceMemory memory = VK_NULL_HANDLE;
                    VkImageView view = VK_NULL_HANDLE;
                    VkSampler sampler = VK_NULL_HANDLE;
            };
        }
    }
}
