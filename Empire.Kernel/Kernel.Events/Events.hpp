#pragma once
// Empire.Kernel / Kernel.Events — публичный API модуля.
// Системные заголовки (windows.h, wayland-client.h и т.п.) сюда не попадают.

#if defined(_WIN32)
    #if defined(KERNEL_EVENTS_EXPORT)
        #define KERNEL_EVENTS_API __declspec(dllexport)
    #else
        #define KERNEL_EVENTS_API __declspec(dllimport)
    #endif
#else
    #define KERNEL_EVENTS_API __attribute__((visibility("default")))
#endif

namespace Empire::Kernel::Events
{
}
