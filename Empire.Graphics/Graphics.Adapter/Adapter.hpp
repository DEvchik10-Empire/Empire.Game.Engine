#pragma once
// Empire.Graphics / Graphics.Adapter — публичный API модуля.
// Системные заголовки (windows.h, wayland-client.h и т.п.) сюда не попадают.

#if defined(_WIN32)
    #if defined(GRAPHICS_ADAPTER_EXPORT)
        #define GRAPHICS_ADAPTER_API __declspec(dllexport)
    #else
        #define GRAPHICS_ADAPTER_API __declspec(dllimport)
    #endif
#else
    #define GRAPHICS_ADAPTER_API __attribute__((visibility("default")))
#endif

namespace Empire::Graphics::Adapter
{
}
