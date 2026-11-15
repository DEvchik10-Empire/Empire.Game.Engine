#include "Impl.hpp"

#include "Window.hpp"

#include "../../Empire.App/legasy/Connection.hpp"

namespace Engine
{
    namespace Platform
    {
        namespace legasy
        {
            struct Impl::Data
            {
                Window window;

                Types::b8 closed = false;

                Data(
                    const WindowConfig& config
                )
                    : window(config)
                {
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


            Types::b8 Impl::PollEvent()
            {
                if (!data || data->closed)
                {
                    return false;
                }

                return data->window.PollEvent();
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


            // Реально закрывает окно (уничтожает его на X-сервере).
            // Безопасно звать много раз.
            void Impl::Close()
            {
                if (!data || data->closed)
                {
                    return;
                }

                data->closed = true;
                data->window.Destroy();
            }


            Types::b8 Impl::IsOpen() const
            {
                return !ShouldClose();
            }


            void Impl::SetTitle(
                const Types::String& title
            )
            {
                if (!data)
                {
                    return;
                }

                data->window.SetTitle(title);
            }


            std::pair<Types::i32, Types::i32>
            Impl::GetPosition() const
            {
                if (!data)
                {
                    return {0, 0};
                }

                return data->window.GetPosition();
            }


            void Impl::SetPosition(
                Types::i32 x,
                Types::i32 y
            )
            {
                if (!data)
                {
                    return;
                }

                data->window.SetPosition(x, y);
            }


            std::pair<Types::i32, Types::i32>
            Impl::GetSize() const
            {
                if (!data)
                {
                    return {0, 0};
                }

                return data->window.GetSize();
            }


            void Impl::SetSize(
                Types::i32 width,
                Types::i32 height
            )
            {
                if (!data)
                {
                    return;
                }

                data->window.SetSize(width, height);
            }


            void Impl::SetResizable(
                Types::b8 resizable
            )
            {
                if (!data)
                {
                    return;
                }

                data->window.SetResizable(resizable);
            }


            Types::b8 Impl::IsResizable() const
            {
                if (!data)
                {
                    return false;
                }

                return data->window.IsResizable();
            }


            void Impl::Show()
            {
                if (!data)
                {
                    return;
                }

                data->window.Show();
            }


            void Impl::Hide()
            {
                if (!data)
                {
                    return;
                }

                data->window.Hide();
            }


            void Impl::Minimize()
            {
                if (!data)
                {
                    return;
                }

                data->window.Minimize();
            }


            void Impl::Maximize()
            {
                if (!data)
                {
                    return;
                }

                data->window.Maximize();
            }


            void Impl::Restore()
            {
                if (!data)
                {
                    return;
                }

                data->window.Restore();
            }


            void Impl::SetFullscreen(
                Types::b8 fullscreen
            )
            {
                if (!data)
                {
                    return;
                }

                data->window.SetFullscreen(fullscreen);
            }


            Types::b8 Impl::IsFullscreen() const
            {
                if (!data)
                {
                    return false;
                }

                return data->window.IsFullscreen();
            }


            void Impl::SetAlwaysOnTop(
                Types::b8 onTop
            )
            {
                if (!data)
                {
                    return;
                }

                data->window.SetAlwaysOnTop(onTop);
            }


            Types::b8 Impl::IsAlwaysOnTop() const
            {
                if (!data)
                {
                    return false;
                }

                return data->window.IsAlwaysOnTop();
            }


            Types::b8 Impl::WasResized()
            {
                if (!data)
                {
                    return false;
                }

                return data->window.WasResized();
            }


            Types::b8 Impl::WasMoved()
            {
                if (!data)
                {
                    return false;
                }

                return data->window.WasMoved();
            }


            Types::b8 Impl::HasFocus() const
            {
                if (!data)
                {
                    return false;
                }

                return data->window.HasFocus();
            }

            WindowBackend legasy::Impl::GetBackend() const
            {
                return WindowBackend::legasy;
            }

            NativeWindowHandle legasy::Impl::GetNativeHandle() const
            {
                if (!data || data->closed)
                {
                    return NativeWindowHandle{};
                }

                const auto [width, height] =
                    data->window.GetSize();

                return NativeWindowHandle::MakeXcb(
                    Connection::Instance().get(),
                    data->window.Get(),
                    static_cast<Types::u32>(width),
                    static_cast<Types::u32>(height)
                );
            }

        }
    }
}