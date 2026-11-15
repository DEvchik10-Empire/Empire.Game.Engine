#pragma once
// Empire.Math / Math.Matrix — публичный API модуля.
// Системные заголовки (windows.h, wayland-client.h и т.п.) сюда не попадают.

#if defined(_WIN32)
    #if defined(MATH_MATRIX_EXPORT)
        #define MATH_MATRIX_API __declspec(dllexport)
    #else
        #define MATH_MATRIX_API __declspec(dllimport)
    #endif
#else
    #define MATH_MATRIX_API __attribute__((visibility("default")))
#endif

namespace Empire::Math::Matrix
{
}
