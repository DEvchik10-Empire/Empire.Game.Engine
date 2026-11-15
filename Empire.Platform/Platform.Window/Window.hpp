#pragma once
// Empire.Platform / Platform.Window — публичный API модуля.
// Системные заголовки (windows.h, wayland-client.h и т.п.) сюда не попадают.

#if defined(_WIN32)
    #if defined(PLATFORM_WINDOW_EXPORT)
        #define PLATFORM_WINDOW_API __declspec(dllexport)
    #else
        #define PLATFORM_WINDOW_API __declspec(dllimport)
    #endif
#else
    #define PLATFORM_WINDOW_API __attribute__((visibility("default")))
#endif

namespace Empire::Platform::Window
{
}
