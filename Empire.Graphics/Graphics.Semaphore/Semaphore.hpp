#pragma once
// Empire.Graphics / Graphics.Semaphore — публичный API модуля.
// Системные заголовки (windows.h, wayland-client.h и т.п.) сюда не попадают.

#if defined(_WIN32)
    #if defined(GRAPHICS_SEMAPHORE_EXPORT)
        #define GRAPHICS_SEMAPHORE_API __declspec(dllexport)
    #else
        #define GRAPHICS_SEMAPHORE_API __declspec(dllimport)
    #endif
#else
    #define GRAPHICS_SEMAPHORE_API __attribute__((visibility("default")))
#endif

namespace Empire::Graphics::Semaphore
{
}
