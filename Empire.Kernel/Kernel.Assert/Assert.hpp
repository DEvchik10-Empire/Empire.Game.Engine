#pragma once
// Empire.Kernel / Kernel.Assert — публичный API модуля.
// Системные заголовки (windows.h, wayland-client.h и т.п.) сюда не попадают.

#if defined(_WIN32)
    #if defined(KERNEL_ASSERT_EXPORT)
        #define KERNEL_ASSERT_API __declspec(dllexport)
    #else
        #define KERNEL_ASSERT_API __declspec(dllimport)
    #endif
#else
    #define KERNEL_ASSERT_API __attribute__((visibility("default")))
#endif

namespace Empire::Kernel::Assert
{
}
