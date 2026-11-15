#pragma once
// Empire.Platform / Platform.Library — публичный API модуля.
// Системные заголовки (windows.h, wayland-client.h и т.п.) сюда не попадают.

#if defined(_WIN32)
    #if defined(PLATFORM_LIBRARY_EXPORT)
        #define PLATFORM_LIBRARY_API __declspec(dllexport)
    #else
        #define PLATFORM_LIBRARY_API __declspec(dllimport)
    #endif
#else
    #define PLATFORM_LIBRARY_API __attribute__((visibility("default")))
#endif

namespace Empire::Platform::Library
{
}
