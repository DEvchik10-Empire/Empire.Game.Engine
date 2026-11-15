#pragma once

#include "../../Empire.Device/Vulkan/VkCommon.hpp"
#include "../../Empire.Device/Vulkan/VkInstance.hpp"

#include "Kernel/NativeWindowHandle.hpp"

namespace Engine
{
    namespace Rendering
    {
        namespace Vulkan
        {
            /*
             * VkSurfaceKHR из NativeWindowHandle.
             *
             * Это ЕДИНСТВЕННОЕ место Rendering, которое знает, что бывают
             * Wayland/XCB/Win32: switch по handle.kind. Про сами окна
             * (IWindowImpl и т.д.) Rendering ничего не знает.
             *
             * Время жизни: Surface должен умереть РАНЬШЕ окна и РАНЬШЕ Instance.
             */
            class Surface
            {
                public:
                    Surface(const Instance& instance, const NativeWindowHandle& window);
                    ~Surface();

                    Surface(const Surface&) = delete;
                    Surface& operator=(const Surface&) = delete;
                    Surface(Surface&&) = delete;
                    Surface& operator=(Surface&&) = delete;

                    VkSurfaceKHR Get() const;

                private:
                    VkInstance instance = VK_NULL_HANDLE;
                    VkSurfaceKHR handle = VK_NULL_HANDLE;
            };
        }
    }
}
