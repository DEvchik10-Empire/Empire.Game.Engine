#include <chrono>
#include <thread>

#include "../../Empire.Window/legasy/Impl.hpp"

namespace Engine
{
    namespace Platform
    {
        namespace legasy
        {
            namespace
            {
                // Вычитываем ВСЕ накопившиеся события окна.
                // false из PollEvent = "очередь пуста", а не "ошибка/выход",
                // поэтому тут нет никакого break.
                void Pump(Impl& window)
                {
                    while (window.IsOpen() && window.PollEvent())
                    {
                    }

                    // Пользователь нажал крестик -> реально убираем окно.
                    // Close() можно звать много раз.
                    if (window.ShouldClose())
                    {
                        window.Close();
                    }
                }
            }

            void RunTest()
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
                    Pump(window1);
                    Pump(window2);
                    Pump(window3);
                    Pump(window4);

                    // Пока нет рендера - не жжём 100% ядра.
                    // Когда появится рендер, это место займёт кадр.
                    std::this_thread::sleep_for(
                        std::chrono::milliseconds(1));
                }
            }
        }
    }
}

int main()
{
    Engine::Platform::legasy::RunTest();

    return 0;
}
