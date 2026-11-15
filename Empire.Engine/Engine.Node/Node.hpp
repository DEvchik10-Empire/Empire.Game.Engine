#pragma once
// Empire.Engine / Engine.Node — публичный API модуля.
// Системные заголовки (windows.h, wayland-client.h и т.п.) сюда не попадают.

#if defined(_WIN32)
    #if defined(ENGINE_NODE_EXPORT)
        #define ENGINE_NODE_API __declspec(dllexport)
    #else
        #define ENGINE_NODE_API __declspec(dllimport)
    #endif
#else
    #define ENGINE_NODE_API __attribute__((visibility("default")))
#endif

namespace Empire::Engine::Node
{
}
