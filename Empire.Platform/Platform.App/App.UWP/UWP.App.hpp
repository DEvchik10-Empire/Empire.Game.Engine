#pragma once

#include "Empire.Kernel/Kernel.Types/Types.hpp"
#include "Empire.Platform/Platform.ThirdParty/ThirdParty.UWP/UWP.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpirePlatform
        {
            namespace PlatformApp
            {
                namespace AppUWP
                {
                    using namespace EmpireKernel::KernelTypes;
                    using winrt::Windows::ApplicationModel::Core::CoreApplicationView;
                    using winrt::Windows::UI::Core::CoreWindow;

                    class ViewProvider;

                    class App final
                    {
                        private:
                            CoreApplicationView NativeView{ nullptr };
                            CoreWindow NativeWindow{ nullptr };
                            Int32 ErrorCode = 0;
                            Bool IsRunning = false;

                            App() noexcept = default;
                            ~App() = default;

                            void SetApplicationView(const CoreApplicationView& View) noexcept;
                            void SetCoreWindow(const CoreWindow& Window) noexcept;
                            void ActivateCoreWindow() noexcept;
                            void ProcessEvents() noexcept;
                            void ResetView() noexcept;

                            friend class ViewProvider;

                        public:
                            App(const App&) = delete;
                            App& operator=(const App&) = delete;
                            App(App&&) = delete;
                            App& operator=(App&&) = delete;

                            [[nodiscard]] static App& Instance() noexcept;

                            // Call from the UWP executable entry point; CoreApplication owns the lifecycle.
                            [[nodiscard]] static Int32 Run() noexcept;

                            [[nodiscard]] CoreApplicationView GetApplicationView() const noexcept;
                            // CoreWindow is a system-created object and must be used on its view thread.
                            [[nodiscard]] CoreWindow GetCoreWindow() const noexcept;
                            [[nodiscard]] Bool IsReady() const noexcept;
                            // Returns zero on success, or the HRESULT from a failed WinRT operation.
                            [[nodiscard]] Int32 GetErrorCode() const noexcept;
                    };
                }
            }
        }
    }
}
