
#include "Display.hpp"

namespace Platform
{
    namespace Wayland
    {
        Display::Display()
        {
            display = wl_display_connect(nullptr);
        }

        Display::~Display()
        {
            if (display)
            {
                wl_display_disconnect(Get());
            }
        }

        Display::Display(Display&& other) noexcept
            : display(other.display)
        {
            other.display = nullptr;
        }

        Display& Display::operator=(Display&& other) noexcept
        {
            if (this == &other)
            {
                return *this;
            }

            if (display)
            {
                wl_display_disconnect(display);
            }

            display = other.display;
            other.display = nullptr;

            return *this;
        }

        b8 Display::Dispatch()
        {
            return wl_display_dispatch(Get()) != -1;
        }

        b8 Display::DispatchPending()
        {
            return wl_display_dispatch_pending(Get()) != -1;
        }

        b8 Display::Roundtrip()
        {
            return wl_display_roundtrip(Get()) != -1;
        }

        i32 Display::GetFd() const
        {
            return wl_display_get_fd(Get());
        }

        wl_display* Display::Get() const
        {
            return display;
        }

        wl_registry* Display::GetRegistry() const
        {
            return wl_display_get_registry(Get());
        }

        void Display::Flush()
        {
            wl_display_flush(Get());
        }

    }
}