#pragma once
// Empire.Engine / Engine.Diagnostics — публичный API модуля.
// Системные заголовки (windows.h, wayland-client.h и т.п.) сюда не попадают.

#if defined(_WIN32)
    #if defined(ENGINE_DIAGNOSTICS_EXPORT)
        #define ENGINE_DIAGNOSTICS_API __declspec(dllexport)
    #else
        #define ENGINE_DIAGNOSTICS_API __declspec(dllimport)
    #endif
#else
    #define ENGINE_DIAGNOSTICS_API __attribute__((visibility("default")))
#endif

namespace Empire::Engine::Diagnostics
{
}
