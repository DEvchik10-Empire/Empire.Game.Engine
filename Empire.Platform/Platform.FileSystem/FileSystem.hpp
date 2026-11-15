#pragma once
// Empire.Platform / Platform.FileSystem — публичный API модуля.
// Системные заголовки (windows.h, wayland-client.h и т.п.) сюда не попадают.

#if defined(_WIN32)
    #if defined(PLATFORM_FILESYSTEM_EXPORT)
        #define PLATFORM_FILESYSTEM_API __declspec(dllexport)
    #else
        #define PLATFORM_FILESYSTEM_API __declspec(dllimport)
    #endif
#else
    #define PLATFORM_FILESYSTEM_API __attribute__((visibility("default")))
#endif

namespace Empire::Platform::FileSystem
{
}
