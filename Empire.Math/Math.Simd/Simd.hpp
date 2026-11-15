#pragma once
// Empire.Math / Math.Simd — публичный API модуля.
// Системные заголовки (windows.h, wayland-client.h и т.п.) сюда не попадают.

#if defined(_WIN32)
    #if defined(MATH_SIMD_EXPORT)
        #define MATH_SIMD_API __declspec(dllexport)
    #else
        #define MATH_SIMD_API __declspec(dllimport)
    #endif
#else
    #define MATH_SIMD_API __attribute__((visibility("default")))
#endif

namespace Empire::Math::Simd
{
}
