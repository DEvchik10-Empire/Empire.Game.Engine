#pragma once
// Empire.Math / Math.Transform — публичный API модуля.
// Системные заголовки (windows.h, wayland-client.h и т.п.) сюда не попадают.

#if defined(_WIN32)
    #if defined(MATH_TRANSFORM_EXPORT)
        #define MATH_TRANSFORM_API __declspec(dllexport)
    #else
        #define MATH_TRANSFORM_API __declspec(dllimport)
    #endif
#else
    #define MATH_TRANSFORM_API __attribute__((visibility("default")))
#endif

namespace Empire::Math::Transform
{
}
