#pragma once

#include "../../../Empire.Engine/Kernel/Types.hpp"

namespace Engine
{
    namespace Platform
    {
        using namespace Kernel;

        namespace Wayland
        {
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

                const char* title = "Empire Engine";
            };
        }
    }
}