#pragma once
// Empire.Graphics / Graphics.Shader — публичный API модуля.
// Системные заголовки (windows.h, wayland-client.h и т.п.) сюда не попадают.

#if defined(_WIN32)
    #if defined(GRAPHICS_SHADER_EXPORT)
        #define GRAPHICS_SHADER_API __declspec(dllexport)
    #else
        #define GRAPHICS_SHADER_API __declspec(dllimport)
    #endif
#else
    #define GRAPHICS_SHADER_API __attribute__((visibility("default")))
#endif

namespace Empire::Graphics::Shader
{
}
