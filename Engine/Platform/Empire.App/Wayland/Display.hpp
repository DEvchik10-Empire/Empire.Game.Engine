#pragma once

#include <wayland-client.h>
#include "../Empire.Engine/Kernel/Types.hpp"

namespace Engine
{
    namespace Platform
    {
        using namespace Kernel;

        namespace Wayland
        {
            class Display
            {
                public:
                    Display();
                    ~Display();

                    Display(const Display&) = delete;
                    Display& operator=(const Display&) = delete;
                    Display(Display&& other) noexcept;
                    Display& operator=(Display&& other) noexcept;

                    // БЛОКИРУЮЩИЙ: ждёт события. Для игрового цикла не подходит.
                    Types::b8 Dispatch();
                    Types::b8 DispatchPending();
                    Types::b8 Roundtrip();

                    // НЕ блокирующий: читает сокет, раздаёт события.
                    // Возвращает число обработанных событий (0 = очередь пуста),
                    // либо -1, если соединение с композитором умерло.
                    Types::i32 PumpEvents();

                    Types::i32 GetFd() const;

                    wl_display* Get() const;
                    wl_registry* GetRegistry() const;

                    void Flush();

                private:
                    wl_display* display = nullptr;
            };
        }
    }
}
