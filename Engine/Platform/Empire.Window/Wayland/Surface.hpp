
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
            class SurfaceCallbacks
            {
            public:
                virtual void OnEnter(wl_output* output) {}
                virtual void OnLeave(wl_output* output) {}

                virtual void OnPreferredBufferScale(
                    Types::i32 factor
                ) {}

                virtual void OnFrame() {}

                virtual ~SurfaceCallbacks() = default;
            };


            class Surface
            {
            private:
                wl_surface* surface = nullptr;
                wl_callback* frame = nullptr;

                SurfaceCallbacks* callbacks = nullptr;

                static void Enter(
                    void* data,
                    wl_surface* surface,
                    wl_output* output
                );

                static void Leave(
                    void* data,
                    wl_surface* surface,
                    wl_output* output
                );

                static void PreferredBufferScale(
                    void* data,
                    wl_surface* surface,
                    Types::i32 factor
                );

                static void PreferredBufferTransform(
                    void* data,
                    wl_surface* surface,
                    Types::u32 transform
                );

                static const wl_surface_listener listener;


                static void FrameDone(
                    void* data,
                    wl_callback* callback,
                    Types::u32 callbackData
                );

                static const wl_callback_listener frameListener;

            public:
                Surface() = default;
                explicit Surface(wl_surface* surface);

                ~Surface();

                Surface(const Surface&) = delete;
                Surface& operator=(const Surface&) = delete;

                Surface(Surface&& other) noexcept;
                Surface& operator=(Surface&& other) noexcept;

                wl_surface* Get() const;

                void Attach(
                    wl_buffer* buffer,
                    Types::i32 x,
                    Types::i32 y
                );

                void Damage(
                    Types::i32 x,
                    Types::i32 y,
                    Types::i32 width,
                    Types::i32 height
                );

                void Commit();

                void SetBufferScale(Types::i32 scale);

                void SetFrame();

                Types::b8 Init(SurfaceCallbacks* callbacks);
            };
        }
    }
}