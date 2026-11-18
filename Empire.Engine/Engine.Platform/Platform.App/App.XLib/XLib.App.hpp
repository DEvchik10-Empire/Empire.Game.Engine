#pragma once

#include "Empire.Engine/Engine.Kernel/Kernel.Types/Types.hpp"
#include "Empire.Engine/Engine.Platform/Platform.ThirdParty/ThirdParty.XLib/XLib.hpp"

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
                    namespace AppXLib
                    {
                        using namespace EngineKernel::KernelTypes;
                        using XWindow = ::Window;

                        class App final
                        {
                            public:
                                // Opens one process-wide X server connection on first access.
                                [[nodiscard]] static App& Instance() noexcept;

                                App(const App&) = delete;
                                App& operator=(const App&) = delete;
                                App(App&&) = delete;
                                App& operator=(App&&) = delete;

                                // Borrowed handle; App closes it during process shutdown.
                                [[nodiscard]] Display* GetNativeHandle() const noexcept;
                                // Default screen selected by XOpenDisplay, or -1 if unavailable.
                                [[nodiscard]] Int32 GetDefaultScreen() const noexcept;
                                // Root window for the selected default screen, or zero if unavailable.
                                [[nodiscard]] XWindow GetRootWindow() const noexcept;
                                [[nodiscard]] Bool IsReady() const noexcept;
                                // Returns zero on success, or -1 if initialization or opening failed.
                                [[nodiscard]] Int32 GetErrorCode() const noexcept;

                            private:
                                App() noexcept;
                                ~App();

                                Display* NativeDisplay = nullptr;
                                Int32 ErrorCode = 0;
                        };
                    }
                }
            }
        }
    }
}
