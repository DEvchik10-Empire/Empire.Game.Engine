#pragma once
// Empire.Engine / Engine.Jobs — публичный API модуля.
// Системные заголовки (windows.h, wayland-client.h и т.п.) сюда не попадают.

#if defined(_WIN32)
    #if defined(ENGINE_JOBS_EXPORT)
        #define ENGINE_JOBS_API __declspec(dllexport)
    #else
        #define ENGINE_JOBS_API __declspec(dllimport)
    #endif
#else
    #define ENGINE_JOBS_API __attribute__((visibility("default")))
#endif

namespace Empire::Engine::Jobs
{
}
