#pragma once
// Empire.Kernel / Kernel.Logging — публичный API модуля.
// Системные заголовки (windows.h, wayland-client.h и т.п.) сюда не попадают.

#if defined(_WIN32)
    #if defined(KERNEL_LOGGING_EXPORT)
        #define KERNEL_LOGGING_API __declspec(dllexport)
    #else
        #define KERNEL_LOGGING_API __declspec(dllimport)
    #endif
#else
    #define KERNEL_LOGGING_API __attribute__((visibility("default")))
#endif

namespace Empire::Kernel::Logging
{
}
