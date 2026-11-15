#pragma once

/*
 * Единая точка входа в <vulkan/vulkan.h> для всего Rendering.
 *
 * Макросы VK_USE_PLATFORM_* включают WSI-функции конкретной платформы
 * (vkCreateWaylandSurfaceKHR и т.д.). Они ОБЯЗАНЫ быть определены до
 * первого #include <vulkan/vulkan.h>, поэтому все остальные файлы
 * подключают vulkan только через этот заголовок.
 */

#if defined(__linux__)
    #ifndef VK_USE_PLATFORM_WAYLAND_KHR
        #define VK_USE_PLATFORM_WAYLAND_KHR
    #endif

    #ifndef VK_USE_PLATFORM_XCB_KHR
        #define VK_USE_PLATFORM_XCB_KHR
    #endif
#endif

#include <vulkan/vulkan.h>

#include "Kernel/Types.hpp"

namespace Engine
{
    namespace Rendering
    {
        namespace Vulkan
        {
            using namespace Kernel;

            // Читаемое имя VkResult (VK_ERROR_OUT_OF_DATE_KHR и т.д.).
            const char* ResultToString(VkResult result);

            // Бросает std::runtime_error, если result != VK_SUCCESS.
            // what - что мы пытались сделать ("vkCreateInstance").
            void Check(VkResult result, const char* what);
        }
    }
}
