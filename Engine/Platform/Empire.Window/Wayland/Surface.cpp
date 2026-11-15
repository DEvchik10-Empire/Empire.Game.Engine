
#include "Surface.hpp"

namespace Engine
{
    namespace Platform
    {
        namespace Wayland
        {
            const wl_surface_listener Surface::listener =
            {
                Surface::Enter,
                Surface::Leave,
                Surface::PreferredBufferScale,
                Surface::PreferredBufferTransform
            };


            const wl_callback_listener Surface::frameListener =
            {
                Surface::FrameDone
            };


            Surface::Surface(wl_surface* surface)
                : surface(surface)
            {
            }


            void Surface::Reset()
            {
                if (frame)
                {
                    wl_callback_destroy(frame);
                    frame = nullptr;
                }

                if (surface)
                {
                    wl_surface_destroy(surface);
                    surface = nullptr;
                }

                callbacks = nullptr;
            }


            Surface::~Surface()
            {
                Reset();
            }


            Surface::Surface(Surface&& other) noexcept
                : surface(other.surface),
                  frame(other.frame),
                  callbacks(other.callbacks)
            {
                other.surface = nullptr;
                other.frame = nullptr;
                other.callbacks = nullptr;
            }


            Surface& Surface::operator=(Surface&& other) noexcept
            {
                if (this == &other)
                {
                    return *this;
                }

                Reset();

                surface = other.surface;
                frame = other.frame;
                callbacks = other.callbacks;

                other.surface = nullptr;
                other.frame = nullptr;
                other.callbacks = nullptr;

                return *this;
            }


            wl_surface* Surface::Get() const
            {
                return surface;
            }


            void Surface::Attach(
                wl_buffer* buffer,
                Types::i32 x,
                Types::i32 y
            )
            {
                wl_surface_attach(
                    surface,
                    buffer,
                    x,
                    y
                );
            }


            void Surface::Damage(
                Types::i32 x,
                Types::i32 y,
                Types::i32 width,
                Types::i32 height
            )
            {
                wl_surface_damage_buffer(
                    surface,
                    x,
                    y,
                    width,
                    height
                );
            }


            void Surface::Commit()
            {
                wl_surface_commit(surface);
            }


            void Surface::SetBufferScale(Types::i32 scale)
            {
                wl_surface_set_buffer_scale(
                    surface,
                    scale
                );
            }


            void Surface::SetFrame()
            {
                if (frame)
                {
                    return;
                }

                frame = wl_surface_frame(surface);

                if (!frame)
                {
                    return;
                }

                wl_callback_add_listener(
                    frame,
                    &frameListener,
                    this
                );
            }


            Types::b8 Surface::Init(
                SurfaceCallbacks* callbacks
            )
            {
                if (!surface)
                {
                    return false;
                }

                this->callbacks = callbacks;

                wl_surface_add_listener(
                    surface,
                    &listener,
                    this
                );

                return true;
            }


            void Surface::Enter(
                void* data,
                wl_surface* /*surface*/,
                wl_output* output
            )
            {
                Surface* self = static_cast<Surface*>(data);

                if (self->callbacks)
                {
                    self->callbacks->OnEnter(output);
                }
            }


            void Surface::Leave(
                void* data,
                wl_surface* /*surface*/,
                wl_output* output
            )
            {
                Surface* self = static_cast<Surface*>(data);

                if (self->callbacks)
                {
                    self->callbacks->OnLeave(output);
                }
            }


            void Surface::PreferredBufferScale(
                void* data,
                wl_surface* /*surface*/,
                Types::i32 factor
            )
            {
                Surface* self = static_cast<Surface*>(data);

                if (self->callbacks)
                {
                    self->callbacks->OnPreferredBufferScale(factor);
                }
            }


            void Surface::PreferredBufferTransform(
                void* /*data*/,
                wl_surface* /*surface*/,
                Types::u32 /*transform*/
            )
            {
            }


            void Surface::FrameDone(
                void* data,
                wl_callback* /*callback*/,
                Types::u32 /*callbackData*/
            )
            {
                Surface* self = static_cast<Surface*>(data);

                self->frame = nullptr;

                if (self->callbacks)
                {
                    self->callbacks->OnFrame();
                }
            }
        }
    }
}
