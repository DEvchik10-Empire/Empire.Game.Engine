#pragma once

#include <memory>

#include "IWindowImpl.hpp"
#include "WindowConfig.hpp"

namespace Engine
{
    namespace Platform
    {
        std::unique_ptr<IWindowImpl>
        CreateWindowImpl(
            const WindowConfig& config
        );
    }
}