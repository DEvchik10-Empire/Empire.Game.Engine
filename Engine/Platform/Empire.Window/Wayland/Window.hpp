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
                bool destroyed = false;

                // true  - окно само рисует серый фон (wl_shm)
                // false - поверхность целиком принадлежит рендеру (Vulkan)
                bool selfDraw = true;

                bool CreateBuffer();
                void Draw();

            public:
                Window() = default;
                ~Window() override = default;

                Window(const Window&) = delete;
                Window& operator=(const Window&) = delete;

                // Вызвать ДО Init().
                // width/height <= 0 -> размер выберет композитор (или 800x600).
                // Начальный размер используется только при selfDraw == false.
                void Configure(
                    Types::i32 width,
                    Types::i32 height,
                    bool selfDraw
                );

                // wl_surface для рендера. nullptr после Destroy().
                wl_surface* GetSurface() const;

                bool Init(
                    Compositor& compositor,
                    XdgWmBase& wmBase,
                    Shm& shm,
                    const Types::String& title
                );

                // Реально убирает окно: уничтожает wl_buffer, xdg_toplevel,
                // xdg_surface и wl_surface (в таком порядке). Можно звать много раз.
                // После этого вызови Display::Flush(), чтобы запросы дошли до композитора.
                void Destroy();

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