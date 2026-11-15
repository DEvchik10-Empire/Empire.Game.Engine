#pragma once
// Empire.Graphics / Graphics.RenderPass — публичный API модуля.
// Системные заголовки (windows.h, wayland-client.h и т.п.) сюда не попадают.

#if defined(_WIN32)
    #if defined(GRAPHICS_RENDERPASS_EXPORT)
        #define GRAPHICS_RENDERPASS_API __declspec(dllexport)
    #else
        #define GRAPHICS_RENDERPASS_API __declspec(dllimport)
    #endif
#else
    #define GRAPHICS_RENDERPASS_API __attribute__((visibility("default")))
#endif

namespace Empire::Graphics::RenderPass
{
}
