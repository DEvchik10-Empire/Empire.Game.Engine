#pragma once
// Empire.Math / Math.Vector — публичный API модуля.
// Системные заголовки (windows.h, wayland-client.h и т.п.) сюда не попадают.

#if defined(_WIN32)
    #if defined(MATH_VECTOR_EXPORT)
        #define MATH_VECTOR_API __declspec(dllexport)
    #else
        #define MATH_VECTOR_API __declspec(dllimport)
    #endif
#else
    #define MATH_VECTOR_API __attribute__((visibility("default")))
#endif

namespace Empire::Math::Vector
{
}
