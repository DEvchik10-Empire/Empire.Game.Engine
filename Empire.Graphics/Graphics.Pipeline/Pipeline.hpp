#pragma once
// Empire.Graphics / Graphics.Pipeline — публичный API модуля.
// Системные заголовки (windows.h, wayland-client.h и т.п.) сюда не попадают.

#if defined(_WIN32)
    #if defined(GRAPHICS_PIPELINE_EXPORT)
        #define GRAPHICS_PIPELINE_API __declspec(dllexport)
    #else
        #define GRAPHICS_PIPELINE_API __declspec(dllimport)
    #endif
#else
    #define GRAPHICS_PIPELINE_API __attribute__((visibility("default")))
#endif

namespace Empire::Graphics::Pipeline
{
}
