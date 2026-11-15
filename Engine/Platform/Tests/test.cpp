#include <chrono>
#include <cstddef>
#include <cstring>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>

#include "Empire.Window/IWindowImpl.hpp"
#include "Empire.Window/WindowBackend.hpp"
#include "Empire.Window/WindowConfig.hpp"
#include "Empire.Window/WindowImplFactory.hpp"

using namespace Engine::Platform;

namespace
{
    const char* BackendName(WindowBackend backend)
    {
        switch (backend)
        {
            case WindowBackend::Auto:    return "Auto";
            case WindowBackend::Wayland: return "Wayland";
            case WindowBackend::legasy:  return "legasy";
        }

        return "Unknown";
    }

    // Запуск:  ./test            -> Auto
    //          ./test wayland    -> только Wayland
    //          ./test legasy     -> только X11 (XCB)
    WindowBackend ParseBackend(int argc, char** argv)
    {
        if (argc > 1)
        {
            if (std::strcmp(argv[1], "wayland") == 0)
                return WindowBackend::Wayland;

            if (std::strcmp(argv[1], "legasy") == 0)
                return WindowBackend::legasy;
        }

        return WindowBackend::Auto;
    }

    struct Entry
    {
        int id;
        std::unique_ptr<IWindowImpl> window;
    };
}

int main(int argc, char** argv)
{
    const WindowBackend backend = ParseBackend(argc, argv);

    std::cout << "=== 4 windows test, backend: "
              << BackendName(backend) << " ===\n";

    struct Size { int width; int height; };

    const Size sizes[] = {
        { 800, 600 },
        { 500, 400 },
        { 600, 500 },
        { 400, 300 },
    };

    // ---------------------------------------------------------
    // Создаём 4 окна
    // ---------------------------------------------------------
    std::vector<Entry> windows;

    for (int i = 0; i < 4; ++i)
    {
        WindowConfig config;

        config.width = sizes[i].width;
        config.height = sizes[i].height;
        config.title = "Empire Engine - Window " + std::to_string(i + 1);
        config.backend = backend;

        std::unique_ptr<IWindowImpl> window = CreateWindowImpl(config);

        if (!window || !window->IsOpen())
        {
            std::cout << "[Test] FAILED: window " << (i + 1)
                      << " was not created.\n";
            return 1;
        }

        window->Show();

        std::cout << "[Test] Window " << (i + 1)
                  << " created, backend: "
                  << BackendName(window->GetBackend()) << '\n';

        windows.push_back({ i + 1, std::move(window) });
    }

    std::cout << "[Test] Close the windows one by one (any order).\n"
              << "[Test] The program exits when the last one is closed.\n\n";

    // ---------------------------------------------------------
    // Главный цикл: пока есть хотя бы одно окно
    // ---------------------------------------------------------
    while (!windows.empty())
    {
        for (auto it = windows.begin(); it != windows.end();)
        {
            IWindowImpl& window = *it->window;

            // false из PollEvent = "очередь пуста", а не ошибка.
            while (window.IsOpen() && window.PollEvent())
            {
            }

            if (window.ShouldClose())
            {
                window.Close();             // убрать окно с экрана
                const int id = it->id;
                it = windows.erase(it);     // и освободить объект

                std::cout << "[Test] Window " << id
                          << " closed, left: " << windows.size() << '\n';
            }
            else
            {
                ++it;
            }
        }

        // Пока нет рендера - не жжём 100% ядра.
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    std::cout << "=== All windows closed ===\n";

    return 0;
}