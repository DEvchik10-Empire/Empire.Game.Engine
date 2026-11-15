#pragma once

#include <memory>

#include "Surface.hpp"
#include "XdgWmBase.hpp"
#include "Shm.hpp"
#include "Buffer.hpp"

#include "../../Empire.App/Wayland/Compositor.hpp"

namespace Engine
{
    namespace Platform
    {
        namespace Wayland
        {
            class Window :
                public SurfaceCallbacks,
                public XdgSurfaceCallbacks,
                public XdgToplevelCallbacks
            {
            private:
                Surface surface;
                XdgSurface xdgSurface;
                XdgToplevel xdgToplevel;

                Shm* shm = nullptr;
                std::unique_ptr<Buffer> buffer;

                Types::u32 width = 0;
                Types::u32 height = 0;

                bool configured = false;
                bool shouldClose = false;

                bool CreateBuffer();
                void Draw();

            public:
                Window() = default;
                ~Window() override = default;

                Window(const Window&) = delete;
                Window& operator=(const Window&) = delete;

                bool Init(
                    Compositor& compositor,
                    XdgWmBase& wmBase,
                    Shm& shm,
                    const char* title
                );

                bool ShouldClose() const;
                bool IsConfigured() const;

                Types::u32 GetWidth() const;
                Types::u32 GetHeight() const;

                void OnConfigure(
                    Types::i32 width,
                    Types::i32 height
                ) override;

                void OnClose() override;
            };
        }
    }
}