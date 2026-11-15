#pragma once
// Empire.Runtime / Runtime.Shared — код, общий для клиента, сервера и игры.

#if defined(_WIN32)
    #if defined(RUNTIME_SHARED_EXPORT)
        #define RUNTIME_SHARED_API __declspec(dllexport)
    #else
        #define RUNTIME_SHARED_API __declspec(dllimport)
    #endif
#else
    #define RUNTIME_SHARED_API __attribute__((visibility("default")))
#endif

namespace Empire::Runtime::Shared
{
}
