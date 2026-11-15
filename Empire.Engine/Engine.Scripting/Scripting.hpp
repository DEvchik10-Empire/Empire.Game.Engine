#pragma once
// Empire.Engine / Engine.Scripting — публичный API модуля.
// Системные заголовки (windows.h, wayland-client.h и т.п.) сюда не попадают.

#if defined(_WIN32)
    #if defined(ENGINE_SCRIPTING_EXPORT)
        #define ENGINE_SCRIPTING_API __declspec(dllexport)
    #else
        #define ENGINE_SCRIPTING_API __declspec(dllimport)
    #endif
#else
    #define ENGINE_SCRIPTING_API __attribute__((visibility("default")))
#endif

namespace Empire::Engine::Scripting
{
}
