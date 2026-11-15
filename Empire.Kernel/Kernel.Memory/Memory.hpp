#pragma once
// Empire.Kernel / Kernel.Memory — публичный API модуля.
// Системные заголовки (windows.h, wayland-client.h и т.п.) сюда не попадают.

#if defined(_WIN32)
    #if defined(KERNEL_MEMORY_EXPORT)
        #define KERNEL_MEMORY_API __declspec(dllexport)
    #else
        #define KERNEL_MEMORY_API __declspec(dllimport)
    #endif
#else
    #define KERNEL_MEMORY_API __attribute__((visibility("default")))
#endif

namespace Empire::Kernel::Memory
{
}
