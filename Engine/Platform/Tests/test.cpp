#include <iostream>
#include <memory>

#include "Empire.Window/IWindowImpl.hpp"
#include "Empire.Window/WindowConfig.hpp"
#include "Empire.Window/WindowBackend.hpp"
#include "Empire.Window/WindowImplFactory.hpp"


namespace Engine
{
    namespace Platform
    {
        const char* BackendName(
            WindowBackend backend
        )
        {
            switch (backend)
            {
                case WindowBackend::Auto:
                    return "Auto";

                case WindowBackend::Wayland:
                    return "Wayland";

                case WindowBackend::legasy:
                    return "legasy";
            }

            return "Unknown";
        }
    }
}


int main()
{
    using namespace Engine::Platform;

    std::cout
        << "=== Empire.Window IWindowImpl Test ===\n\n";


    /*
     * ---------------------------------------------------------
     * Auto backend
     * ---------------------------------------------------------
     */

    std::cout
        << "[Test] Creating Auto backend...\n";

    WindowConfig autoConfig;

    autoConfig.width = 800;
    autoConfig.height = 600;
    autoConfig.title = "Empire Engine - Auto";
    autoConfig.backend = WindowBackend::Auto;

    std::unique_ptr<IWindowImpl> autoWindow =
        CreateWindowImpl(autoConfig);

    std::cout
        << "[Test] Requested backend: "
        << BackendName(autoConfig.backend)
        << '\n';

    if (!autoWindow)
    {
        std::cout
            << "[Test] FAILED: Auto backend could not create a window.\n";

        return 1;
    }

    std::cout
        << "[Test] Selected backend:  "
        << BackendName(autoWindow->GetBackend())
        << '\n';

    std::cout
        << "[Test] Is open:            "
        << (autoWindow->IsOpen() ? "yes" : "no")
        << '\n';

    if (!autoWindow->IsOpen())
    {
        std::cout
            << "[Test] FAILED: Auto backend returned a closed window.\n";

        return 1;
    }

    autoWindow->SetTitle(
        "Empire Engine - Auto Test"
    );

    autoWindow->Show();

    std::cout
        << "[Test] Auto selected: "
        << BackendName(autoWindow->GetBackend())
        << '\n';

    std::cout
        << "[Test] Auto window is open.\n"
        << "[Test] Close the window to continue...\n\n";

    while (!autoWindow->ShouldClose())
    {
        autoWindow->PollEvent();
    }

    std::cout
        << "[Test] Auto window closed.\n\n";


    /*
     * ---------------------------------------------------------
     * Explicit Wayland backend
     * ---------------------------------------------------------
     */

    std::cout
        << "[Test] Creating explicit Wayland backend...\n";

    WindowConfig waylandConfig;

    waylandConfig.width = 800;
    waylandConfig.height = 600;
    waylandConfig.title = "Empire Engine - Wayland";
    waylandConfig.backend = WindowBackend::Wayland;

    std::unique_ptr<IWindowImpl> wayland =
        CreateWindowImpl(waylandConfig);

    std::cout
        << "[Test] Requested backend: "
        << BackendName(waylandConfig.backend)
        << '\n';

    if (!wayland)
    {
        std::cout
            << "[Test] FAILED: Wayland backend could not create a window.\n";

        return 1;
    }

    std::cout
        << "[Test] Selected backend:  "
        << BackendName(wayland->GetBackend())
        << '\n';

    std::cout
        << "[Test] Is open:            "
        << (wayland->IsOpen() ? "yes" : "no")
        << '\n';

    if (!wayland->IsOpen())
    {
        std::cout
            << "[Test] FAILED: Wayland window is not open.\n";

        return 1;
    }

    if (wayland->GetBackend() != WindowBackend::Wayland)
    {
        std::cout
            << "[Test] FAILED: Wrong backend was selected.\n";

        return 1;
    }

    wayland->SetTitle(
        "Empire Engine - Wayland Test"
    );

    wayland->Show();

    std::cout
        << "[Test] Wayland window is open.\n"
        << "[Test] Close the window to continue...\n\n";

    while (!wayland->ShouldClose())
    {
        wayland->PollEvent();
    }

    std::cout
        << "[Test] Wayland window closed.\n\n";


    /*
     * ---------------------------------------------------------
     * Explicit legasy backend
     * ---------------------------------------------------------
     */

    std::cout
        << "[Test] Creating explicit legasy backend...\n";

    WindowConfig legasyConfig;

    legasyConfig.width = 800;
    legasyConfig.height = 600;
    legasyConfig.title = "Empire Engine - legasy";
    legasyConfig.backend = WindowBackend::legasy;

    std::unique_ptr<IWindowImpl> legasy =
        CreateWindowImpl(legasyConfig);

    std::cout
        << "[Test] Requested backend: "
        << BackendName(legasyConfig.backend)
        << '\n';

    if (!legasy)
    {
        std::cout
            << "[Test] FAILED: legasy backend could not create a window.\n";

        return 1;
    }

    std::cout
        << "[Test] Selected backend:  "
        << BackendName(legasy->GetBackend())
        << '\n';

    std::cout
        << "[Test] Is open:            "
        << (legasy->IsOpen() ? "yes" : "no")
        << '\n';

    if (!legasy->IsOpen())
    {
        std::cout
            << "[Test] FAILED: legasy window is not open.\n";

        return 1;
    }

    if (legasy->GetBackend() != WindowBackend::legasy)
    {
        std::cout
            << "[Test] FAILED: Wrong backend was selected.\n";

        return 1;
    }

    legasy->SetTitle(
        "Empire Engine - legasy Test"
    );

    legasy->Show();

    std::cout
        << "[Test] legasy window is open.\n"
        << "[Test] Close the window to finish the test...\n\n";

    while (!legasy->ShouldClose())
    {
        legasy->PollEvent();
    }

    std::cout
        << "[Test] legasy window closed.\n\n";


    /*
     * ---------------------------------------------------------
     * Finished
     * ---------------------------------------------------------
     */

    std::cout
        << "=== All tests passed ===\n";

    return 0;
}