
#pragma once

#include "../Platform/ThirdParty/Wayland/xdg-shell-client-protocol.h"
#include "../Platform/Empire.App/Wayland/Interface.hpp"
#include "../Empire.Engine/Kernel/Types.hpp"
#include <wayland-client.h>

namespace Engine
{
    namespace Platform
    {
        using namespace Kernel;

        namespace Wayland
        {
            class XdgToplevelCallbacks
            {
            public:
                virtual void OnConfigure(
                    Types::i32 width,
                    Types::i32 height
                )
                {
                }

                virtual void OnClose()
                {
                }

                virtual ~XdgToplevelCallbacks() = default;
            };


            class XdgToplevel
            {
            private:
                xdg_toplevel* toplevel = nullptr;

                XdgToplevelCallbacks* callbacks = nullptr;

                static void Configure(
                    void* data,
                    xdg_toplevel* toplevel,
                    Types::i32 width,
                    Types::i32 height,
                    wl_array* states
                );

                static void Close(
                    void* data,
                    xdg_toplevel* toplevel
                );

                static void Bounds(
                    void* data,
                    xdg_toplevel* toplevel,
                    Types::i32 width,
                    Types::i32 height
                );

                static void Capabilities(
                    void* data,
                    xdg_toplevel* toplevel,
                    wl_array* capabilities
                );

                static const xdg_toplevel_listener listener;

            public:
                XdgToplevel() = default;
                explicit XdgToplevel(xdg_toplevel* toplevel);

                ~XdgToplevel();

                XdgToplevel(const XdgToplevel&) = delete;
                XdgToplevel& operator=(const XdgToplevel&) = delete;

                XdgToplevel(XdgToplevel&& other) noexcept;
                XdgToplevel& operator=(XdgToplevel&& other) noexcept;

                Types::b8 Init(
                    XdgToplevelCallbacks* callbacks
                );

                void SetTitle(const char* title);

                xdg_toplevel* Get() const;
            };


            class XdgSurfaceCallbacks
            {
            public:
                virtual void OnConfigure()
                {
                }

                virtual ~XdgSurfaceCallbacks() = default;
            };


            class XdgSurface
            {
            private:
                xdg_surface* surface = nullptr;

                XdgSurfaceCallbacks* callbacks = nullptr;

                static void Configure(
                    void* data,
                    xdg_surface* surface,
                    Types::u32 serial
                );

                static const xdg_surface_listener listener;

            public:
                XdgSurface() = default;
                explicit XdgSurface(xdg_surface* surface);

                ~XdgSurface();

                XdgSurface(const XdgSurface&) = delete;
                XdgSurface& operator=(const XdgSurface&) = delete;

                XdgSurface(XdgSurface&& other) noexcept;
                XdgSurface& operator=(XdgSurface&& other) noexcept;

                Types::b8 Init(
                    XdgSurfaceCallbacks* callbacks
                );

                XdgToplevel CreateToplevel();

                xdg_surface* Get() const;
            };


            class XdgWmBase : public Interface
            {
            private:
                xdg_wm_base* wmBase = nullptr;

                static void Ping(
                    void* data,
                    xdg_wm_base* wmBase,
                    Types::u32 serial
                );

                static const xdg_wm_base_listener listener;

            public:
                explicit XdgWmBase(void* data);
                ~XdgWmBase() override;

                XdgWmBase(const XdgWmBase&) = delete;
                XdgWmBase& operator=(const XdgWmBase&) = delete;

                XdgSurface CreateSurface(
                    wl_surface* surface
                );

                xdg_wm_base* Get() const;
            };


            class XdgWmBaseBinder : public InterfaceBinder
            {
            public:
                explicit XdgWmBaseBinder(
                    Types::u32 version
                );

                const wl_interface* GetInterfaceDescription() const override;

                std::unique_ptr<Interface> CreateInterface(
                    void* data
                ) override;
            };
        }
    }
}
