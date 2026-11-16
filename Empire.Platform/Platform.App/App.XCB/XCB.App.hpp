#pragma once

#include "Empire.Kernel/Kernel.Types/Types.hpp"
#include "Empire.Platform/Platform.ThirdParty/ThirdParty.XCB/XCB.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpirePlatform
        {
            namespace PlatformApp
            {
                namespace AppXCB
                {
                    using namespace EmpireKernel::KernelTypes;

                    class App final
                    {
                        public:
                            // Creates the process-wide XCB connection on first access.
                            [[nodiscard]] static App& Instance() noexcept;

                            App(const App&) = delete;
                            App& operator=(const App&) = delete;
                            App(App&&) = delete;
                            App& operator=(App&&) = delete;

                            // Borrowed handle; the connection owns it and disconnects at shutdown.
                            [[nodiscard]] xcb_connection_t* GetNativeHandle() const noexcept;
                            [[nodiscard]] Bool IsReady() const noexcept;
                            // Returns the XCB error code, or -1 if xcb_connect returned null.
                            [[nodiscard]] Int32 GetErrorCode() const noexcept;

                        private:
                            App() noexcept;
                            ~App();

                            xcb_connection_t* NativeConnection = nullptr;
                            Int32 ErrorCode = 0;
                    };
                }
            }
        }
    }
}
