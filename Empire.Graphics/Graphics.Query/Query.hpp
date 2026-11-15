#pragma once
// Empire.Graphics / Graphics.Query — публичный API модуля.
// Системные заголовки (windows.h, wayland-client.h и т.п.) сюда не попадают.

#if defined(_WIN32)
    #if defined(GRAPHICS_QUERY_EXPORT)
        #define GRAPHICS_QUERY_API __declspec(dllexport)
    #else
        #define GRAPHICS_QUERY_API __declspec(dllimport)
    #endif
#else
    #define GRAPHICS_QUERY_API __attribute__((visibility("default")))
#endif

namespace Empire::Graphics::Query
{
}
