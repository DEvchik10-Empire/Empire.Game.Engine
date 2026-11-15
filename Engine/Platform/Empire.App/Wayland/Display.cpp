#include "Display.hpp"

#include <cerrno>

#include <poll.h>

namespace Engine
{
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
                    wl_display_disconnect(display);
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

            Types::b8 Display::Dispatch()
            {
                return wl_display_dispatch(Get()) != -1;
            }

            Types::b8 Display::DispatchPending()
            {
                return wl_display_dispatch_pending(Get()) != -1;
            }

            Types::b8 Display::Roundtrip()
            {
                return wl_display_roundtrip(Get()) != -1;
            }

            /*
             * Неблокирующая обработка событий.
             *
             * Стандартный паттерн libwayland:
             *   1) prepare_read  - "я сам буду читать сокет"
             *   2) flush         - отправить наши запросы композитору
             *   3) poll(0)       - есть ли что-то на сокете (БЕЗ ожидания)
             *   4) read_events / cancel_read
             *   5) dispatch_pending - вызвать колбэки (configure, close...)
             */
            Types::i32 Display::PumpEvents()
            {
                wl_display* d = Get();

                if (!d)
                {
                    return -1;
                }

                Types::i32 handled = 0;

                // Если в очереди уже что-то лежит, prepare_read вернёт -1:
                // сначала разбираем это.
                while (wl_display_prepare_read(d) != 0)
                {
                    const int n = wl_display_dispatch_pending(d);

                    if (n == -1)
                    {
                        return -1;
                    }

                    handled += n;
                }

                // EAGAIN = буфер сокета полон, дошлём в следующий раз.
                if (wl_display_flush(d) == -1 && errno != EAGAIN)
                {
                    wl_display_cancel_read(d);
                    return -1;
                }

                pollfd pfd{};
                pfd.fd = wl_display_get_fd(d);
                pfd.events = POLLIN;

                if (poll(&pfd, 1, 0) > 0)
                {
                    if (wl_display_read_events(d) == -1)
                    {
                        return -1;
                    }
                }
                else
                {
                    wl_display_cancel_read(d);
                }

                const int n = wl_display_dispatch_pending(d);

                if (n == -1)
                {
                    return -1;
                }

                return handled + n;
            }

            Types::i32 Display::GetFd() const
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
                if (display)
                {
                    wl_display_flush(display);
                }
            }
        }
    }
}
