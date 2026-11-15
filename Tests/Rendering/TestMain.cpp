/*
 * Тест рендеринга: Platform (окно) + Rendering (Vulkan).
 *
 * Что проверяет:
 *   1. Окно создаётся через фабрику (Wayland или X11) БЕЗ собственной отрисовки.
 *   2. Из окна достаётся NativeWindowHandle (мост Platform -> Rendering).
 *   3. Vulkan: Instance -> Surface -> Device -> SwapChain -> FrameContext.
 *   4. Каждый кадр окно закрашивается плавно меняющимся цветом.
 *   5. Ресайз окна пересоздаёт swapchain.
 *   6. Закрытие окна (крестик) корректно всё разбирает.
 *
 * Здесь же, в приложении, Platform и Rendering впервые "встречаются":
 * сами они друг про друга не знают.
 *
 * Запуск:
 *   Empire.Rendering.Test.Vulkan [auto|wayland|legasy] [--frames=N] [--no-validation]
 */

#include <chrono>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include "Empire.Window/IWindowImpl.hpp"
#include "Empire.Window/WindowBackend.hpp"
#include "Empire.Window/WindowConfig.hpp"
#include "Empire.Window/WindowImplFactory.hpp"

#include "Empire.Device/Vulkan/VkDevice.hpp"
#include "Empire.Device/Vulkan/VkInstance.hpp"
#include "Empire.SwapChain/Vulkan/VkSurface.hpp"
#include "Empire.SwapChain/Vulkan/VkSwapChain.hpp"
#include "Empire.CommandBuffer/Vulkan/VkFrameContext.hpp"

namespace Platform = Engine::Platform;
namespace Vulkan = Engine::Rendering::Vulkan;

namespace
{
    struct Options
    {
        Platform::WindowBackend backend = Platform::WindowBackend::Auto;

        // 0 = работать, пока окно не закроют.
        unsigned int maxFrames = 0;

        bool validation = true;
    };

    const char* BackendName(Platform::WindowBackend backend)
    {
        switch (backend)
        {
            case Platform::WindowBackend::Auto:    return "Auto";
            case Platform::WindowBackend::Wayland: return "Wayland";
            case Platform::WindowBackend::legasy:  return "legasy (X11)";
        }

        return "Unknown";
    }

    Options ParseOptions(int argc, char** argv)
    {
        Options options;

        for (int i = 1; i < argc; ++i)
        {
            const char* argument = argv[i];

            if (std::strcmp(argument, "wayland") == 0)
            {
                options.backend = Platform::WindowBackend::Wayland;
            }
            else if (std::strcmp(argument, "legasy") == 0)
            {
                options.backend = Platform::WindowBackend::legasy;
            }
            else if (std::strcmp(argument, "auto") == 0)
            {
                options.backend = Platform::WindowBackend::Auto;
            }
            else if (std::strcmp(argument, "--no-validation") == 0)
            {
                options.validation = false;
            }
            else if (std::strncmp(argument, "--frames=", 9) == 0)
            {
                options.maxFrames = static_cast<unsigned int>(
                    std::strtoul(argument + 9, nullptr, 10));
            }
            else
            {
                std::cerr << "[Test] Unknown argument: " << argument << '\n';
            }
        }

        return options;
    }

    // Всё, что связано с Vulkan, живёт ТОЛЬКО внутри этой функции.
    // Когда она завершается (или бросает исключение), все объекты Vulkan
    // уничтожаются в правильном порядке (в обратном созданию) -
    // и только ПОСЛЕ этого main закрывает окно.
    void RunRendering(Platform::IWindowImpl& window, const Options& options)
    {
        Vulkan::InstanceDesc instanceDesc;
        instanceDesc.applicationName = "Empire.Rendering.Test";
        instanceDesc.enableValidation = options.validation;

        Vulkan::Instance instance(instanceDesc);

        std::cout << "[Test] Vulkan instance created"
                  << (instance.IsValidationEnabled() ? " (validation ON)" : "")
                  << '\n';

        // ---- Мост: окно -> нативные хэндлы -> VkSurfaceKHR ----
        Vulkan::Surface surface(instance, window.GetNativeHandle());

        Vulkan::Device device(instance, surface.Get());

        std::cout << "[Test] GPU: " << device.GetName() << '\n';

        const auto [startWidth, startHeight] = window.GetSize();

        Vulkan::SwapChain swapChain(
            device,
            surface,
            static_cast<unsigned int>(startWidth),
            static_cast<unsigned int>(startHeight));

        Vulkan::FrameContext frames(device, swapChain);

        std::cout << "[Test] Swapchain: "
                  << swapChain.GetExtent().width << "x"
                  << swapChain.GetExtent().height
                  << ", images: " << swapChain.GetImageCount() << '\n';

        std::cout << "[Test] Rendering. Resize the window, then close it.\n\n";

        // Размер, под который сейчас собран swapchain
        // (чтобы не пересоздавать его каждый кадр).
        unsigned int swapWidth = static_cast<unsigned int>(startWidth);
        unsigned int swapHeight = static_cast<unsigned int>(startHeight);

        const auto startTime = std::chrono::steady_clock::now();
        auto fpsTime = startTime;

        unsigned int frameCount = 0;
        unsigned int fpsFrames = 0;

        while (window.IsOpen())
        {
            // false из PollEvent = "очередь пуста", а не ошибка.
            while (window.PollEvent())
            {
            }

            if (window.ShouldClose())
            {
                break;
            }

            const auto [width, height] = window.GetSize();

            // Окно свёрнуто или ещё без размера: рисовать нечего.
            if (width <= 0 || height <= 0)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(16));
                continue;
            }

            // Размер окна изменился -> пересобираем swapchain.
            if (
                static_cast<unsigned int>(width) != swapWidth ||
                static_cast<unsigned int>(height) != swapHeight)
            {
                swapWidth = static_cast<unsigned int>(width);
                swapHeight = static_cast<unsigned int>(height);

                swapChain.Resize(swapWidth, swapHeight);

                std::cout << "[Test] Resized: "
                          << swapChain.GetExtent().width << "x"
                          << swapChain.GetExtent().height << '\n';
            }

            // ---------------- кадр ----------------
            Vulkan::FrameInfo frame;

            const Vulkan::FrameStatus began = frames.Begin(frame);

            if (began == Vulkan::FrameStatus::OutOfDate)
            {
                swapChain.Resize(swapWidth, swapHeight);
                continue;
            }

            if (began == Vulkan::FrameStatus::Skipped)
            {
                continue;
            }

            const float seconds =
                std::chrono::duration<float>(
                    std::chrono::steady_clock::now() - startTime).count();

            // Три синусоиды со сдвигом фаз на 120 градусов: цвет плавно "дышит".
            const float red   = 0.5f + 0.5f * std::sin(seconds);
            const float green = 0.5f + 0.5f * std::sin(seconds + 2.0944f);
            const float blue  = 0.5f + 0.5f * std::sin(seconds + 4.1888f);

            frames.ClearColor(frame, red, green, blue, 1.0f);

            if (frames.End(frame) == Vulkan::FrameStatus::OutOfDate)
            {
                swapChain.Resize(swapWidth, swapHeight);
            }

            ++frameCount;
            ++fpsFrames;

            // Раз в секунду печатаем FPS.
            const auto now = std::chrono::steady_clock::now();

            if (now - fpsTime >= std::chrono::seconds(1))
            {
                std::cout << "[Test] " << fpsFrames << " fps  (frame "
                          << frameCount << ")\n";

                fpsFrames = 0;
                fpsTime = now;
            }

            if (options.maxFrames != 0 && frameCount >= options.maxFrames)
            {
                break;
            }
        }

        // Перед уничтожением ресурсов GPU должен закончить работу.
        device.WaitIdle();

        std::cout << "\n[Test] Rendered frames: " << frameCount << '\n';
    }
}

int main(int argc, char** argv)
{
    const Options options = ParseOptions(argc, argv);

    std::cout << "=== Empire.Rendering Vulkan test, window backend: "
              << BackendName(options.backend) << " ===\n";

    // ---- Окно: БЕЗ собственной отрисовки (selfDraw = false) ----
    Platform::WindowConfig config;

    config.width = 800;
    config.height = 600;
    config.title = "Empire Engine - Vulkan clear";
    config.backend = options.backend;
    config.selfDraw = false;

    std::unique_ptr<Platform::IWindowImpl> window =
        Platform::CreateWindowImpl(config);

    if (!window)
    {
        std::cerr << "[Test] FAILED: window was not created.\n";
        return 1;
    }

    std::cout << "[Test] Window created, backend: "
              << BackendName(window->GetBackend()) << '\n';

    window->Show();

    int result = 0;

    try
    {
        RunRendering(*window, options);
    }
    catch (const std::exception& error)
    {
        std::cerr << "[Test] FAILED: " << error.what() << '\n';
        result = 1;
    }

    // К этому моменту все объекты Vulkan уже уничтожены.
    window->Close();
    window.reset();

    if (result == 0)
    {
        std::cout << "=== Test passed ===\n";
    }

    return result;
}
