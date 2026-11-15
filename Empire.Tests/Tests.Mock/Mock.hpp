#pragma once
// Empire.Tests / Tests.Mock — публичный API модуля.
// Системные заголовки (windows.h, wayland-client.h и т.п.) сюда не попадают.

#if defined(_WIN32)
    #if defined(TESTS_MOCK_EXPORT)
        #define TESTS_MOCK_API __declspec(dllexport)
    #else
        #define TESTS_MOCK_API __declspec(dllimport)
    #endif
#else
    #define TESTS_MOCK_API __attribute__((visibility("default")))
#endif

namespace Empire::Tests::Mock
{
}
