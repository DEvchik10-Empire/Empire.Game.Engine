#pragma once
// Empire.Platform / Platform.Time — публичный API модуля.
// Системные заголовки (windows.h, wayland-client.h и т.п.) сюда не попадают.

#if defined(_WIN32)
    #if defined(PLATFORM_TIME_EXPORT)
        #define PLATFORM_TIME_API __declspec(dllexport)
    #else
        #define PLATFORM_TIME_API __declspec(dllimport)
    #endif
#else
    #define PLATFORM_TIME_API __attribute__((visibility("default")))
#endif

namespace Empire::Platform::Time
{
}
