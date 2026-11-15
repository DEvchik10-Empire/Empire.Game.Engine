#include "Window.hpp"

namespace Engine
{
    namespace Platform
    {
        namespace Wayland
        {
            void Window::Configure(
                Types::i32 width,
                Types::i32 height,
                bool selfDraw
            )
            {
                this->selfDraw = selfDraw;

                if (selfDraw)
                {
                    // Старое поведение не меняем: размер задаёт
                    // композитор, иначе 800x600 (см. CreateBuffer).
                    return;
                }

                this->width =
                    width > 0
                        ? static_cast<Types::u32>(width)
                        : 800;

                this->height =
                    height > 0
                        ? static_cast<Types::u32>(height)
                        : 600;
            }


            wl_surface* Window::GetSurface() const
            {
                return surface.Get();
            }


            bool Window::Init(
                Compositor& compositor,
                XdgWmBase& wmBase,
                Shm& shm,
                const Types::String& title
            )
            {
                this->shm = &shm;

                surface =
                    compositor.CreateSurface();

                if (!surface.Get())
                {
                    return false;
                }

                if (!surface.Init(this))
                {
                    return false;
                }

                xdgSurface =
                    wmBase.CreateSurface(
                        surface.Get()
                    );

                if (!xdgSurface.Get())
                {
                    return false;
                }

                if (!xdgSurface.Init(this))
                {
                    return false;
                }

                xdgToplevel =
                    xdgSurface.CreateToplevel();

                if (!xdgToplevel.Get())
                {
                    return false;
                }

                if (!xdgToplevel.Init(this))
                {
                    return false;
                }

                xdgToplevel.SetTitle(title.c_str());

                /*
                 * Initial commit.
                 *
                 * После него compositor отправит
                 * xdg_surface.configure.
                 */
                surface.Commit();

                return true;
            }


            void Window::OnConfigure(
                Types::i32 width,
                Types::i32 height
            )
            {
                if (destroyed)
                {
                    return;
                }

                if (width > 0)
                {
                    this->width =
                        static_cast<Types::u32>(
                            width
                        );
                }

                if (height > 0)
                {
                    this->height =
                        static_cast<Types::u32>(
                            height
                        );
                }

                configured = true;

                // Поверхность принадлежит рендеру: shm-буфер не создаём
                // и ничего не рисуем, иначе он дерётся со swapchain.
                if (!selfDraw)
                {
                    return;
                }

                if (!CreateBuffer())
                {
                    return;
                }

                Draw();
            }


            void Window::OnClose()
            {
                /*
                 * xdg_toplevel.close - это только ПРОСЬБА закрыть окно
                 * ("пользователь нажал крестик"), само окно композитор
                 * не убирает. Здесь только ставим флаг, а уничтожение делает
                 * приложение через Destroy() (см. Impl::Close).
                 */
                shouldClose = true;
            }


            void Window::Destroy()
            {
                if (destroyed)
                {
                    return;
                }

                destroyed = true;
                shouldClose = true;

                /*
                 * Порядок важен: сначала то, что создано позже.
                 * buffer -> xdg_toplevel -> xdg_surface -> wl_surface
                 */
                buffer.reset();
                xdgToplevel.Reset();
                xdgSurface.Reset();
                surface.Reset();
            }


            bool Window::CreateBuffer()
            {
                if (!shm)
                {
                    return false;
                }

                if (width == 0)
                {
                    width = 800;
                }

                if (height == 0)
                {
                    height = 600;
                }

                ShmPool pool =
                    shm->CreatePool();

                buffer =
                    pool.CreateBuffer(
                        width,
                        height
                    );

                if (!buffer)
                {
                    return false;
                }

                return buffer->IsValid();
            }


            void Window::Draw()
            {
                if (
                    !buffer ||
                    !buffer->IsValid()
                )
                {
                    return;
                }

                Types::u32* pixels =
                    static_cast<Types::u32*>(
                        buffer->GetData()
                    );

                const Types::u32 stride =
                    buffer->GetStride() /
                    sizeof(Types::u32);

                for (
                    Types::u32 y = 0;
                    y < height;
                    ++y
                )
                {
                    for (
                        Types::u32 x = 0;
                        x < width;
                        ++x
                    )
                    {
                        pixels[
                            y * stride + x
                        ] = 0xFF202020;
                    }
                }

                surface.Attach(
                    buffer->Get(),
                    0,
                    0
                );

                surface.Damage(
                    0,
                    0,
                    static_cast<Types::i32>(
                        width
                    ),
                    static_cast<Types::i32>(
                        height
                    )
                );

                surface.Commit();
            }


            bool Window::ShouldClose() const
            {
                return shouldClose;
            }


            bool Window::IsConfigured() const
            {
                return configured;
            }


            Types::u32 Window::GetWidth() const
            {
                return width;
            }


            Types::u32 Window::GetHeight() const
            {
                return height;
            }
        }
    }
}