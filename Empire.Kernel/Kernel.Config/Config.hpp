#pragma once
// Empire.Kernel / Kernel.Config — публичный API модуля.
// Системные заголовки (windows.h, wayland-client.h и т.п.) сюда не попадают.

#if defined(_WIN32)
    #if defined(KERNEL_CONFIG_EXPORT)
        #define KERNEL_CONFIG_API __declspec(dllexport)
    #else
        #define KERNEL_CONFIG_API __declspec(dllimport)
    #endif
#else
    #define KERNEL_CONFIG_API __attribute__((visibility("default")))
#endif

namespace Empire::Kernel::Config
{
}
