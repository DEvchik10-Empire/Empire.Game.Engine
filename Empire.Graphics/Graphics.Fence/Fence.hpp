#pragma once
// Empire.Graphics / Graphics.Fence — публичный API модуля.
// Системные заголовки (windows.h, wayland-client.h и т.п.) сюда не попадают.

#if defined(_WIN32)
    #if defined(GRAPHICS_FENCE_EXPORT)
        #define GRAPHICS_FENCE_API __declspec(dllexport)
    #else
        #define GRAPHICS_FENCE_API __declspec(dllimport)
    #endif
#else
    #define GRAPHICS_FENCE_API __attribute__((visibility("default")))
#endif

namespace Empire::Graphics::Fence
{
}
