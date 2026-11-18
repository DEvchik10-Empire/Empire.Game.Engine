#pragma once

#include "Empire.Engine/Engine.Kernel/Kernel.Types/Types.hpp"

struct android_app;

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
                    namespace AppAndroid
                    {
                        using namespace EngineKernel::KernelTypes;

                        class App final
                        {
                            private:
                                ::android_app* NativeApplication = nullptr;
                                Int32 ErrorCode = 0;

                                App() noexcept = default;
                                ~App() = default;

                            public:
                                App(const App&) = delete;
                                App& operator=(const App&) = delete;
                                App(App&&) = delete;
                                App& operator=(App&&) = delete;

                                [[nodiscard]] static App& Instance() noexcept;

                                // Attach to the android_app supplied by android_main; ownership stays with the glue.
                                [[nodiscard]] Bool Initialize(::android_app* Application) noexcept;
                                void Shutdown() noexcept;

                                // Borrowed native-app-glue context; use it on its owning application thread.
                                [[nodiscard]] ::android_app* GetNativeHandle() const noexcept;
                                [[nodiscard]] Bool IsReady() const noexcept;
                                // Returns zero on success, -1 for null input, or -2 for a conflicting context.
                                [[nodiscard]] Int32 GetErrorCode() const noexcept;
                        };
                    }
                }
            }
        }
    }
}
