#pragma once

#include "Empire.Engine/Engine.Kernel/Kernel.Types/Types.hpp"
#include "Empire.Engine/Engine.Platform/Platform.ThirdParty/ThirdParty.Win32/Win32.hpp"

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
                    namespace AppWin32
                    {
                        using namespace EngineKernel::KernelTypes;

                        class App final
                        {
                            public:
                                // Returns the process-wide Win32 application context.
                                [[nodiscard]] static App& Instance() noexcept;

                                App(const App&) = delete;
                                App& operator=(const App&) = delete;
                                App(App&&) = delete;
                                App& operator=(App&&) = delete;

                                // Borrowed executable module handle; it remains valid until process exit.
                                [[nodiscard]] HINSTANCE GetNativeHandle() const noexcept;
                                [[nodiscard]] Bool IsReady() const noexcept;
                                // Returns ERROR_SUCCESS, or the Win32 error code from initialization.
                                [[nodiscard]] UInt32 GetErrorCode() const noexcept;

                            private:
                                App() noexcept;
                                ~App();

                                HINSTANCE NativeInstance = nullptr;
                                UInt32 ErrorCode = 0;
                        };
                    }
                }
            }
        }
    }
}
