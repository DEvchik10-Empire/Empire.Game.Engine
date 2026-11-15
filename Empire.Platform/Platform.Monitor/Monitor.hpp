#pragma once
// Empire.Platform / Platform.Monitor — публичный API модуля.
// Системные заголовки (windows.h, wayland-client.h и т.п.) сюда не попадают.

#if defined(_WIN32)
    #if defined(PLATFORM_MONITOR_EXPORT)
        #define PLATFORM_MONITOR_API __declspec(dllexport)
    #else
        #define PLATFORM_MONITOR_API __declspec(dllimport)
    #endif
#else
    #define PLATFORM_MONITOR_API __attribute__((visibility("default")))
#endif

namespace Empire::Platform::Monitor
{
}
