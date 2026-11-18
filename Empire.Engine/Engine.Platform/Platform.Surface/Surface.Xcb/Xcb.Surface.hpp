#pragma once

#include "Empire.Engine/Engine.Kernel/Kernel.Types/Types.hpp"
#include "Empire.Engine/Engine.Platform/Platform.ThirdParty/ThirdParty.Xcb/Xcb.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireEngine
        {
            namespace EnginePlatform
            {
                namespace PlatformSurface
                {
                    namespace SurfaceXcb
                    {
                        using namespace EngineKernel::KernelTypes;

                        class Surface final
                        {
                            public:
                                Surface() noexcept = default;
                                ~Surface();

                                Surface(const Surface&) = delete;
                                Surface& operator=(const Surface&) = delete;
                                Surface(Surface&&) = delete;
                                Surface& operator=(Surface&&) = delete;

                                [[nodiscard]] Bool Initialize(UInt16 Width, UInt16 Height) noexcept;
                                void Shutdown() noexcept;

                                [[nodiscard]] xcb_window_t GetWindow() const noexcept;
                                [[nodiscard]] Int32 GetErrorCode() const noexcept;

                            private:
                                void DestroyWindow() noexcept;

                                xcb_window_t Window = XCB_WINDOW_NONE;
                                Int32 ErrorCode = 0;
                        };
                    }
                }
            }
        }
    }
}
