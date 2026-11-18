#pragma once

#include "Empire.Engine/Engine.Kernel/Kernel.Types/Types.hpp"
#include "Empire.Engine/Engine.Platform/Platform.ThirdParty/ThirdParty.UIKit/UIKit.hpp"

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
                    namespace AppUIKit
                    {
                        using namespace EngineKernel::KernelTypes;

                        class App final
                        {
                            public:
                                [[nodiscard]] static App& Instance() noexcept;

                                App(const App&) = delete;
                                App& operator=(const App&) = delete;
                                App(App&&) = delete;
                                App& operator=(App&&) = delete;

                                // UIKit creates this singleton during UIApplicationMain; query it on the main thread.
                                [[nodiscard]] UIApplication* GetNativeHandle() const noexcept;
                                [[nodiscard]] Bool IsReady() const noexcept;
                                // Returns zero when UIApplicationMain has created the app, -1 off the main thread,
                                // or -2 before the app object exists.
                                [[nodiscard]] Int32 GetErrorCode() const noexcept;

                            private:
                                App() noexcept = default;
                                ~App() = default;
                        };
                    }
                }
            }
        }
    }
}
