#pragma once

#include "Types.hpp"

namespace Engine
{
    namespace Kernel
    {
        /*
         * Мост между Platform (окна) и Rendering (графика).
         *
         * Это ТОЛЬКО данные: ни wayland-, ни xcb-, ни vulkan-заголовков тут нет.
         *
         *   Platform  заполняет эту структуру  (IWindowImpl::GetNativeHandle)
         *   Rendering читает эту структуру     (Vulkan::Surface и т.д.)
         *
         * Platform и Rendering друг про друга не знают.
         * Встречаются они только в коде приложения (см. Tests/Rendering).
         */

        enum class NativeWindowKind : Types::u8
        {
            None,
            Wayland,
            Xcb,
            Win32,
            Cocoa
        };

        struct NativeWindowHandle
        {
            NativeWindowKind kind = NativeWindowKind::None;

            union
            {
                struct
                {
                    Types::VoidPtr display;     // wl_display*
                    Types::VoidPtr surface;     // wl_surface*
                } wayland;

                struct
                {
                    Types::VoidPtr connection;  // xcb_connection_t*
                    Types::u32 window;          // xcb_window_t
                } xcb;

                struct
                {
                    Types::VoidPtr instance;    // HINSTANCE
                    Types::VoidPtr window;      // HWND
                } win32;

                struct
                {
                    Types::VoidPtr view;        // NSView*
                } cocoa;
            };

            // Размер в пикселях на момент вызова GetNativeHandle().
            // 0 = размер ещё неизвестен (Wayland до первого configure).
            Types::u32 width = 0;
            Types::u32 height = 0;

            static NativeWindowHandle MakeWayland(
                Types::VoidPtr display,
                Types::VoidPtr surface,
                Types::u32 width,
                Types::u32 height
            )
            {
                NativeWindowHandle handle{};

                handle.kind = NativeWindowKind::Wayland;
                handle.wayland.display = display;
                handle.wayland.surface = surface;
                handle.width = width;
                handle.height = height;

                return handle;
            }

            static NativeWindowHandle MakeXcb(
                Types::VoidPtr connection,
                Types::u32 window,
                Types::u32 width,
                Types::u32 height
            )
            {
                NativeWindowHandle handle{};

                handle.kind = NativeWindowKind::Xcb;
                handle.xcb.connection = connection;
                handle.xcb.window = window;
                handle.width = width;
                handle.height = height;

                return handle;
            }
        };
    }
}
