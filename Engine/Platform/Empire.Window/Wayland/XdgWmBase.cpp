#include "XdgWmBase.hpp"

namespace Engine
{
    namespace Platform
    {
        namespace Wayland
        {
            // ============================================================
            // XdgToplevel
            // ============================================================

            const xdg_toplevel_listener
                XdgToplevel::listener =
            {
                XdgToplevel::Configure,
                XdgToplevel::Close,
                XdgToplevel::Bounds,
                XdgToplevel::Capabilities
            };


            XdgToplevel::XdgToplevel(
                xdg_toplevel* toplevel
            )
                : toplevel(toplevel)
            {
            }


            void XdgToplevel::Reset()
            {
                if (toplevel)
                {
                    xdg_toplevel_destroy(toplevel);
                    toplevel = nullptr;
                }

                callbacks = nullptr;
            }


            XdgToplevel::~XdgToplevel()
            {
                Reset();
            }


            XdgToplevel::XdgToplevel(
                XdgToplevel&& other
            ) noexcept
                : toplevel(other.toplevel),
                  callbacks(other.callbacks)
            {
                other.toplevel = nullptr;
                other.callbacks = nullptr;
            }


            XdgToplevel& XdgToplevel::operator=(
                XdgToplevel&& other
            ) noexcept
            {
                if (this == &other)
                {
                    return *this;
                }

                Reset();

                toplevel = other.toplevel;
                callbacks = other.callbacks;

                other.toplevel = nullptr;
                other.callbacks = nullptr;

                return *this;
            }


            Types::b8 XdgToplevel::Init(
                XdgToplevelCallbacks* callbacks
            )
            {
                if (!toplevel)
                {
                    return false;
                }

                this->callbacks = callbacks;

                xdg_toplevel_add_listener(
                    toplevel,
                    &listener,
                    this
                );

                return true;
            }


            void XdgToplevel::SetTitle(
                const char* title
            )
            {
                if (toplevel)
                {
                    xdg_toplevel_set_title(
                        toplevel,
                        title
                    );
                }
            }


            xdg_toplevel* XdgToplevel::Get() const
            {
                return toplevel;
            }


            void XdgToplevel::Configure(
                void* data,
                xdg_toplevel*,
                Types::i32 width,
                Types::i32 height,
                wl_array*
            )
            {
                XdgToplevel* self =
                    static_cast<XdgToplevel*>(data);

                if (self->callbacks)
                {
                    self->callbacks->OnConfigure(
                        width,
                        height
                    );
                }
            }


            void XdgToplevel::Close(
                void* data,
                xdg_toplevel*
            )
            {
                XdgToplevel* self =
                    static_cast<XdgToplevel*>(data);

                if (self->callbacks)
                {
                    self->callbacks->OnClose();
                }
            }


            void XdgToplevel::Bounds(
                void*,
                xdg_toplevel*,
                Types::i32,
                Types::i32
            )
            {
            }


            void XdgToplevel::Capabilities(
                void*,
                xdg_toplevel*,
                wl_array*
            )
            {
            }


            // ============================================================
            // XdgSurface
            // ============================================================

            const xdg_surface_listener
                XdgSurface::listener =
            {
                XdgSurface::Configure
            };


            XdgSurface::XdgSurface(
                xdg_surface* surface
            )
                : surface(surface)
            {
            }


            void XdgSurface::Reset()
            {
                if (surface)
                {
                    xdg_surface_destroy(surface);
                    surface = nullptr;
                }

                callbacks = nullptr;
            }


            XdgSurface::~XdgSurface()
            {
                Reset();
            }


            XdgSurface::XdgSurface(
                XdgSurface&& other
            ) noexcept
                : surface(other.surface),
                  callbacks(other.callbacks)
            {
                other.surface = nullptr;
                other.callbacks = nullptr;
            }


            XdgSurface& XdgSurface::operator=(
                XdgSurface&& other
            ) noexcept
            {
                if (this == &other)
                {
                    return *this;
                }

                Reset();

                surface = other.surface;
                callbacks = other.callbacks;

                other.surface = nullptr;
                other.callbacks = nullptr;

                return *this;
            }


            Types::b8 XdgSurface::Init(
                XdgSurfaceCallbacks* callbacks
            )
            {
                if (!surface)
                {
                    return false;
                }

                this->callbacks = callbacks;

                xdg_surface_add_listener(
                    surface,
                    &listener,
                    this
                );

                return true;
            }


            XdgToplevel XdgSurface::CreateToplevel()
            {
                if (!surface)
                {
                    return XdgToplevel();
                }

                return XdgToplevel(
                    xdg_surface_get_toplevel(
                        surface
                    )
                );
            }


            xdg_surface* XdgSurface::Get() const
            {
                return surface;
            }


            void XdgSurface::Configure(
                void* data,
                xdg_surface* surface,
                Types::u32 serial
            )
            {
                /*
                 * Каждое configure от compositor
                 * необходимо подтвердить.
                 */
                xdg_surface_ack_configure(
                    surface,
                    serial
                );

                XdgSurface* self =
                    static_cast<XdgSurface*>(data);

                if (self->callbacks)
                {
                    self->callbacks->OnConfigure();
                }
            }


            // ============================================================
            // XdgWmBase
            // ============================================================

            const xdg_wm_base_listener
                XdgWmBase::listener =
            {
                XdgWmBase::Ping
            };


            XdgWmBase::XdgWmBase(
                void* data
            )
                : wmBase(
                    static_cast<xdg_wm_base*>(data)
                )
            {
                if (wmBase)
                {
                    xdg_wm_base_add_listener(
                        wmBase,
                        &listener,
                        this
                    );
                }
            }


            XdgWmBase::~XdgWmBase()
            {
                if (wmBase)
                {
                    xdg_wm_base_destroy(wmBase);
                    wmBase = nullptr;
                }
            }


            XdgSurface XdgWmBase::CreateSurface(
                wl_surface* surface
            )
            {
                if (!wmBase || !surface)
                {
                    return XdgSurface();
                }

                return XdgSurface(
                    xdg_wm_base_get_xdg_surface(
                        wmBase,
                        surface
                    )
                );
            }


            xdg_wm_base* XdgWmBase::Get() const
            {
                return wmBase;
            }


            void XdgWmBase::Ping(
                void*,
                xdg_wm_base* wmBase,
                Types::u32 serial
            )
            {
                xdg_wm_base_pong(
                    wmBase,
                    serial
                );
            }


            // ============================================================
            // XdgWmBaseBinder
            // ============================================================

            XdgWmBaseBinder::XdgWmBaseBinder(
                Types::u32 version
            )
                : InterfaceBinder(version)
            {
            }


            const wl_interface*
            XdgWmBaseBinder::GetInterfaceDescription() const
            {
                return &xdg_wm_base_interface;
            }


            std::unique_ptr<Interface>
            XdgWmBaseBinder::CreateInterface(
                void* data
            )
            {
                return std::unique_ptr<Interface>(
                    new XdgWmBase(data)
                );
            }
        }
    }
}