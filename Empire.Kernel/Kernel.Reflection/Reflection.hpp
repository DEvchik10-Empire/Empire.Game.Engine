#pragma once
// Empire.Kernel / Kernel.Reflection — публичный API модуля.
// Системные заголовки (windows.h, wayland-client.h и т.п.) сюда не попадают.

#if defined(_WIN32)
    #if defined(KERNEL_REFLECTION_EXPORT)
        #define KERNEL_REFLECTION_API __declspec(dllexport)
    #else
        #define KERNEL_REFLECTION_API __declspec(dllimport)
    #endif
#else
    #define KERNEL_REFLECTION_API __attribute__((visibility("default")))
#endif

namespace Empire::Kernel::Reflection
{
}
