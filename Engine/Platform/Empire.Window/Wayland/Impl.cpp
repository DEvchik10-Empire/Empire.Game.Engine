#include "Impl.hpp"
#include "Window.hpp"

#include "../../Empire.App/Wayland/Display.hpp"
#include "../../Empire.App/Wayland/Registry.hpp"
#include "../../Empire.App/Wayland/Compositor.hpp"

namespace Engine
{
    namespace Platform
    {
        namespace Wayland
        {
            struct Impl::Data
            {
                Display display;
                Registry registry;

                CompositorBinder compositorBinder;
                XdgWmBaseBinder xdgWmBaseBinder;
                ShmBinder shmBinder;

                Compositor* compositor = nullptr;
                XdgWmBase* wmBase = nullptr;
                Shm* shm = nullptr;

                Window window;

                Types::b8 closed = false;

                Data(
                    const WindowConfig& config
                )
                    : display(),
                      registry(display),
                      compositorBinder(4),
                      xdgWmBaseBinder(1),
                      shmBinder(1)
                {
                    if (!display.Get())
                    {
                        closed = true;
                        return;
                    }

                    registry.SetBinders(
                    {
                        &compositorBinder,
                        &xdgWmBaseBinder,
                        &shmBinder
                    });

                    if (!display.Roundtrip())
                    {
                        closed = true;
                        return;
                    }

                    compositor =
                        static_cast<Compositor*>(
                            compositorBinder.GetFirstInterface()
                        );

                    wmBase =
                        static_cast<XdgWmBase*>(
                            xdgWmBaseBinder.GetFirstInterface()
                        );

                    shm =
                        static_cast<Shm*>(
                            shmBinder.GetFirstInterface()
                        );

                    if (!compositor ||
                        !wmBase ||
                        !shm)
                    {
                        closed = true;
                        return;
                    }

                    window.Configure(
                        config.width,
                        config.height,
                        config.selfDraw
                    );

                    if (!window.Init(
                        *compositor,
                        *wmBase,
                        *shm,
                        config.title
                    ))
                    {
                        closed = true;
                        return;
                    }

                    // Отправляем первый commit сразу, не дожидаясь первого PollEvent.
                    display.Flush();

                    /*
                     * Если поверхность отдана рендеру, до первого present
                     * ОБЯЗАТЕЛЬНО должен прийти и быть подтверждён (ack)
                     * первый configure: подключать буфер к xdg-поверхности
                     * без этого нельзя. Поэтому ждём его здесь, а не в цикле.
                     * (Окно с selfDraw == true это делает само и не меняется.)
                     */
                    if (!config.selfDraw)
                    {
                        for (
                            Types::i32 attempt = 0;
                            attempt < 16 && !window.IsConfigured();
                            ++attempt
                        )
                        {
                            if (!display.Roundtrip())
                            {
                                closed = true;
                                return;
                            }
                        }

                        if (!window.IsConfigured())
                        {
                            closed = true;
                            return;
                        }
                    }
                }
            };


            Impl::Impl(
                const WindowConfig& config
            )
            {
                data = new Data(config);
            }


            Impl::~Impl()
            {
                delete data;
                data = nullptr;
            }


            // НЕ блокирующий.
            // true  -> обработано хотя бы одно событие
            // false -> очередь пуста (или окно закрыто / соединение умерло)
            //
            // Использование:  while (window.PollEvent()) {}
            Types::b8 Impl::PollEvent()
            {
                if (!data || data->closed)
                {
                    return false;
                }

                const Types::i32 handled =
                    data->display.PumpEvents();

                if (handled < 0)
                {
                    // Соединение с композитором умерло.
                    data->window.Destroy();
                    data->closed = true;
                    return false;
                }

                return handled > 0;
            }


            Types::b8 Impl::ShouldClose() const
            {
                if (!data)
                {
                    return true;
                }

                return
                    data->closed ||
                    data->window.ShouldClose();
            }


            // Реально закрывает окно: уничтожает объекты и отправляет запросы
            // композитору (flush), иначе окно останется висеть на экране.
            // Безопасно звать много раз.
            void Impl::Close()
            {
                if (!data || data->closed)
                {
                    return;
                }

                data->window.Destroy();
                data->display.Flush();

                data->closed = true;
            }


            Types::b8 Impl::IsOpen() const
            {
                return !ShouldClose();
            }


            void Impl::SetTitle(
                const Types::String& title
            )
            {
                (void)title;
            }


            std::pair<Types::i32, Types::i32>
            Impl::GetPosition() const
            {
                return {0, 0};
            }


            void Impl::SetPosition(
                Types::i32 x,
                Types::i32 y
            )
            {
                (void)x;
                (void)y;
            }


            std::pair<Types::i32, Types::i32>
            Impl::GetSize() const
            {
                if (!data)
                {
                    return {0, 0};
                }

                return
                {
                    static_cast<Types::i32>(
                        data->window.GetWidth()
                    ),

                    static_cast<Types::i32>(
                        data->window.GetHeight()
                    )
                };
            }


            void Impl::SetSize(
                Types::i32 width,
                Types::i32 height
            )
            {
                (void)width;
                (void)height;
            }


            void Impl::SetResizable(
                Types::b8 resizable
            )
            {
                (void)resizable;
            }


            Types::b8 Impl::IsResizable() const
            {
                return true;
            }


            void Impl::Show()
            {
            }


            void Impl::Hide()
            {
            }


            void Impl::Minimize()
            {
            }


            void Impl::Maximize()
            {
            }


            void Impl::Restore()
            {
            }


            void Impl::SetFullscreen(
                Types::b8 fullscreen
            )
            {
                (void)fullscreen;
            }


            Types::b8 Impl::IsFullscreen() const
            {
                return false;
            }


            void Impl::SetAlwaysOnTop(
                Types::b8 onTop
            )
            {
                (void)onTop;
            }


            Types::b8 Impl::IsAlwaysOnTop() const
            {
                return false;
            }


            Types::b8 Impl::WasResized()
            {
                return false;
            }


            Types::b8 Impl::WasMoved()
            {
                return false;
            }


            Types::b8 Impl::HasFocus() const
            {
                return false;
            }

            WindowBackend Wayland::Impl::GetBackend() const
            {
                return WindowBackend::Wayland;
            }

            NativeWindowHandle Wayland::Impl::GetNativeHandle() const
            {
                if (!data || data->closed)
                {
                    return NativeWindowHandle{};
                }

                return NativeWindowHandle::MakeWayland(
                    data->display.Get(),
                    data->window.GetSurface(),
                    data->window.GetWidth(),
                    data->window.GetHeight()
                );
            }
        }
    }
}