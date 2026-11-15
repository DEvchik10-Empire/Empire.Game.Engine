#pragma once
// Empire.Math / Math.Quaternion — публичный API модуля.
// Системные заголовки (windows.h, wayland-client.h и т.п.) сюда не попадают.

#if defined(_WIN32)
    #if defined(MATH_QUATERNION_EXPORT)
        #define MATH_QUATERNION_API __declspec(dllexport)
    #else
        #define MATH_QUATERNION_API __declspec(dllimport)
    #endif
#else
    #define MATH_QUATERNION_API __attribute__((visibility("default")))
#endif

namespace Empire::Math::Quaternion
{
}
