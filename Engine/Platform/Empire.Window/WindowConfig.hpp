#pragma once

#include "Kernel/Types.hpp"
#include "WindowBackend.hpp"

namespace Engine
{
    namespace Platform
    {
        using namespace Kernel;

        struct AutoSize
        {
            operator Types::i32() const
            {
                return -1;
            }
        };

        inline constexpr AutoSize Auto{};

        struct WindowConfig
        {
            Types::i32 width = -1;
            Types::i32 height = -1;

            Types::String title;

            WindowBackend backend =
                WindowBackend::Auto;

            /*
             * Только для Wayland.
             *
             * true  (по умолчанию) - окно само рисует серый фон через wl_shm.
             * false                - окно НИЧЕГО не рисует: wl_surface целиком
             *                        отдаётся рендеру (Vulkan и т.д.).
             *                        Иначе shm-буфер и swapchain дерутся за
             *                        одну поверхность.
             *
             * На X11 (legasy) этот флаг не нужен и игнорируется.
             */
            Types::b8 selfDraw = true;
        };
    }
}