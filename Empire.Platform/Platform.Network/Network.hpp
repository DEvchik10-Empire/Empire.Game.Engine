#pragma once
// Empire.Platform / Platform.Network — публичный API модуля.
// Системные заголовки (windows.h, wayland-client.h и т.п.) сюда не попадают.

#if defined(_WIN32)
    #if defined(PLATFORM_NETWORK_EXPORT)
        #define PLATFORM_NETWORK_API __declspec(dllexport)
    #else
        #define PLATFORM_NETWORK_API __declspec(dllimport)
    #endif
#else
    #define PLATFORM_NETWORK_API __attribute__((visibility("default")))
#endif

namespace Empire::Platform::Network
{
}
