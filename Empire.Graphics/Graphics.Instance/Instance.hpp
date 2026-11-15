#pragma once
// Empire.Graphics / Graphics.Instance — публичный API модуля.
// Системные заголовки (windows.h, wayland-client.h и т.п.) сюда не попадают.

#if defined(_WIN32)
    #if defined(GRAPHICS_INSTANCE_EXPORT)
        #define GRAPHICS_INSTANCE_API __declspec(dllexport)
    #else
        #define GRAPHICS_INSTANCE_API __declspec(dllimport)
    #endif
#else
    #define GRAPHICS_INSTANCE_API __attribute__((visibility("default")))
#endif

namespace Empire::Graphics::Instance
{
}
