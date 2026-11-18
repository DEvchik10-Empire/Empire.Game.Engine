#pragma once

#include "Empire.Engine/Engine.Kernel/Kernel.Types/Types.hpp"
#include "Empire.Engine/Engine.Platform/Platform.ThirdParty/ThirdParty.Wayland/Wayland.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpireEngine
        {
            namespace EnginePlatform
            {
                namespace PlatformApp
                {
                    namespace AppWayland
                    {
                        using namespace EngineKernel::KernelTypes;

                        class Display final
                        {
                            private:
                                wl_display* NativeDisplay = nullptr;
                                Int32 ErrorCode = 0;

                                void SetErrorCode() noexcept;

                            public:
                                Display() noexcept;
                                ~Display();

                                Display(const Display&) = delete;
                                Display& operator=(const Display&) = delete;
                                Display(Display&& Other) noexcept;
                                Display& operator=(Display&& Other) = delete;

                                // Blocking: waits until Wayland dispatches an event.
                                [[nodiscard]] Bool Dispatch() noexcept;
                                [[nodiscard]] Bool DispatchPending() noexcept;
                                [[nodiscard]] Bool Roundtrip() noexcept;

                                // Nonblocking: prepares, polls, reads and dispatches available events.
                                // Returns the number of dispatched events, or -1 on connection failure.
                                [[nodiscard]] Int32 PumpEvents() noexcept;

                                [[nodiscard]] Int32 GetFd() const noexcept;
                                [[nodiscard]] wl_display* Get() const noexcept;
                                [[nodiscard]] Bool IsReady() const noexcept;
                                [[nodiscard]] Int32 GetErrorCode() const noexcept;
                                [[nodiscard]] Bool Flush() noexcept;
                        };
                    }
                }
            }
        }
    }
}
