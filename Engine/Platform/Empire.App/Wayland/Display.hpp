#pragma once 

#include <wayland-client.h>
#include "Types.hpp"

namespace Platform
{
    using namespace Engine;

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

                b8 Dispatch();
                b8 DispatchPending();
                b8 Roundtrip();

                i32 GetFd() const;

                wl_display* Get() const;
                wl_registry* GetRegistry() const;

                void Flush();

            private:
                wl_display* display;
        };
    }
}