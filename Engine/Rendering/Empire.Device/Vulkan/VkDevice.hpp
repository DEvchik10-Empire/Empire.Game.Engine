#pragma once

#include "VkCommon.hpp"
#include "VkInstance.hpp"

namespace Engine
{
    namespace Rendering
    {
        namespace Vulkan
        {
            /*
             * Логическое устройство + очередь.
             *
             * Выбор GPU: нужна очередь, которая умеет И графику, И present
             * в нашу поверхность, плюс расширение VK_KHR_swapchain.
             * Из подходящих берётся "самая настоящая": дискретная > встроенная
             * > виртуальная > программная (lavapipe).
             *
             * VkSurfaceKHR передаётся в конструктор только для проверки present
             * (голый хэндл, чтобы модуль Device не зависел от модуля SwapChain).
             * Время жизни: Device должен умереть РАНЬШЕ Instance.
             */
            class Device
            {
                public:
                    Device(const Instance& instance, VkSurfaceKHR surface);
                    ~Device();

                    Device(const Device&) = delete;
                    Device& operator=(const Device&) = delete;
                    Device(Device&&) = delete;
                    Device& operator=(Device&&) = delete;

                    VkPhysicalDevice GetPhysicalDevice() const;
                    VkDevice Get() const;
                    VkQueue GetQueue() const;
                    Types::u32 GetQueueFamily() const;

                    // 0 - анизотропная фильтрация недоступна.
                    Types::f32 GetMaxAnisotropy() const;

                    const Types::String& GetName() const;

                    // Ждёт, пока GPU закончит всё, что ему отправили.
                    // Обязательно перед уничтожением ресурсов и resize swapchain.
                    void WaitIdle() const;

                    // Индекс типа памяти, подходящего под typeBits и свойства.
                    // Бросает исключение, если такого типа нет.
                    Types::u32 FindMemoryType(
                        Types::u32 typeBits,
                        VkMemoryPropertyFlags properties
                    ) const;

                    // Лучший доступный формат буфера глубины.
                    VkFormat FindDepthFormat() const;

                private:
                    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
                    VkDevice handle = VK_NULL_HANDLE;
                    VkQueue queue = VK_NULL_HANDLE;
                    Types::f32 maxAnisotropy = 0.0f;

                    Types::u32 queueFamily = 0;
                    Types::String name;
            };
        }
    }
}
