#pragma once
// Empire.Platform / Platform.Threading — публичный API модуля.
// Системные заголовки (windows.h, wayland-client.h и т.п.) сюда не попадают.

#if defined(_WIN32)
    #if defined(PLATFORM_THREADING_EXPORT)
        #define PLATFORM_THREADING_API __declspec(dllexport)
    #else
        #define PLATFORM_THREADING_API __declspec(dllimport)
    #endif
#else
    #define PLATFORM_THREADING_API __attribute__((visibility("default")))
#endif

namespace Empire::Platform::Threading
{
}
