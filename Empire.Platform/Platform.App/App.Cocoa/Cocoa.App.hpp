#pragma once

#include "Empire.Kernel/Kernel.Types/Types.hpp"
#include "Empire.Platform/Platform.ThirdParty/ThirdParty.Cocoa/Cocoa.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpirePlatform
        {
            namespace PlatformApp
            {
                namespace AppCocoa
                {
                    using namespace EmpireKernel::KernelTypes;

                    class App final
                    {
                        public:
                            [[nodiscard]] static App& Instance() noexcept;

                            App(const App&) = delete;
                            App& operator=(const App&) = delete;
                            App(App&&) = delete;
                            App& operator=(App&&) = delete;

                            // Create AppKit's process-wide application object from the main thread.
                            [[nodiscard]] Bool Initialize() noexcept;

                            // Borrowed AppKit application object; AppKit owns its lifetime.
                            [[nodiscard]] NSApplication* GetNativeHandle() const noexcept;
                            [[nodiscard]] Bool IsReady() const noexcept;
                            // Returns zero on success, -1 off the main thread, or -2 if AppKit failed.
                            [[nodiscard]] Int32 GetErrorCode() const noexcept;

                        private:
                            App() noexcept = default;
                            ~App() = default;

                            NSApplication* NativeApplication = nil;
                            Int32 ErrorCode = 0;
                    };
                }
            }
        }
    }
}
