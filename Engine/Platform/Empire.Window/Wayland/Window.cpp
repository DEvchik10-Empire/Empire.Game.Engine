#include "Window.hpp"

namespace Engine
{
    namespace Platform
    {
        namespace Wayland
        {
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

                if (!CreateBuffer())
                {
                    return;
                }

                Draw();
            }


            void Window::OnClose()
            {
                shouldClose = true;
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