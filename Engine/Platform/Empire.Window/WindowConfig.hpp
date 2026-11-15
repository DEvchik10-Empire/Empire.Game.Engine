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
        };
    }
}