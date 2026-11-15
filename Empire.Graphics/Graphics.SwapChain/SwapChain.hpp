#pragma once
// Empire.Graphics / Graphics.SwapChain — публичный API модуля.
// Системные заголовки (windows.h, wayland-client.h и т.п.) сюда не попадают.

#if defined(_WIN32)
    #if defined(GRAPHICS_SWAPCHAIN_EXPORT)
        #define GRAPHICS_SWAPCHAIN_API __declspec(dllexport)
    #else
        #define GRAPHICS_SWAPCHAIN_API __declspec(dllimport)
    #endif
#else
    #define GRAPHICS_SWAPCHAIN_API __attribute__((visibility("default")))
#endif

namespace Empire::Graphics::SwapChain
{
}
