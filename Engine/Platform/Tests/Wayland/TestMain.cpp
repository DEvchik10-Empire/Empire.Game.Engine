#include "../../Empire.Window/Wayland/Impl.hpp"

using namespace Engine::Platform::Wayland;

int main()
{
    WindowConfig config1;

    config1.width = 800;
    config1.height = 600;
    config1.title = "Empire Engine - Window 1";

    WindowConfig config2;

    config2.width = 500;
    config2.height = 400;
    config2.title = "Empire Engine - Window 2";

    WindowConfig config3;

    config3.width = 600;
    config3.height = 500;
    config3.title = "Empire Engine - Window 3";

    WindowConfig config4;

    config4.width = 400;
    config4.height = 300;
    config4.title = "Empire Engine - Window 4";

    Impl window1(config1);
    Impl window2(config2);
    Impl window3(config3);
    Impl window4(config4);

    while (
        window1.IsOpen() ||
        window2.IsOpen() ||
        window3.IsOpen() ||
        window4.IsOpen()
    )
    {
        if (window1.IsOpen())
        {
            if (!window1.PollEvent())
            {
                break;
            }
        }

        if (window2.IsOpen())
        {
            if (!window2.PollEvent())
            {
                break;
            }
        }

        if (window3.IsOpen())
        {
            if (!window3.PollEvent())
            {
                break;
            }
        }

        if (window4.IsOpen())
        {
            if (!window4.PollEvent())
            {
                break;
            }
        }
    }

    return 0;
}