#pragma once
// Empire.Math / Math.Geometry — публичный API модуля.
// Системные заголовки (windows.h, wayland-client.h и т.п.) сюда не попадают.

#if defined(_WIN32)
    #if defined(MATH_GEOMETRY_EXPORT)
        #define MATH_GEOMETRY_API __declspec(dllexport)
    #else
        #define MATH_GEOMETRY_API __declspec(dllimport)
    #endif
#else
    #define MATH_GEOMETRY_API __attribute__((visibility("default")))
#endif

namespace Empire::Math::Geometry
{
}
