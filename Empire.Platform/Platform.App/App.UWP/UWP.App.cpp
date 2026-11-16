
#include "Empire.Platform/Platform.App/App.UWP/UWP.App.hpp"

#include <winerror.h>

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
                    class ViewProvider final : public winrt::implements<
                        ViewProvider,
                        winrt::Windows::ApplicationModel::Core::IFrameworkViewSource,
                        winrt::Windows::ApplicationModel::Core::IFrameworkView
                    >
                    {
                        private:
                            CoreApplicationView ApplicationView{ nullptr };
                            winrt::event_token ActivatedToken{};
                            Bool HasActivationHandler = false;

                            void OnActivated(
                                const CoreApplicationView& View,
                                const winrt::Windows::ApplicationModel::Activation::IActivatedEventArgs& Arguments
                            ) noexcept
                            {
                                (void)View;
                                (void)Arguments;
                                App::Instance().ActivateCoreWindow();
                            }

                        public:
                            winrt::Windows::ApplicationModel::Core::IFrameworkView CreateView()
                            {
                                return *this;
                            }

                            void Initialize(const CoreApplicationView& View)
                            {
                                ApplicationView = View;
                                App::Instance().SetApplicationView(View);
                                ActivatedToken = ApplicationView.Activated(
                                    { this, &ViewProvider::OnActivated }
                                );
                                HasActivationHandler = true;
                            }

                            void SetWindow(const CoreWindow& Window)
                            {
                                App::Instance().SetCoreWindow(Window);
                            }

                            void Load(const winrt::hstring& EntryPoint)
                            {
                                (void)EntryPoint;
                            }

                            void Run()
                            {
                                App::Instance().ProcessEvents();
                            }

                            void Uninitialize()
                            {
                                if (HasActivationHandler && ApplicationView)
                                {
                                    ApplicationView.Activated(ActivatedToken);
                                    HasActivationHandler = false;
                                }

                                App::Instance().ResetView();
                                ApplicationView = nullptr;
                            }
                    };

                    App& App::Instance() noexcept
                    {
                        static App UWPApp;
                        return UWPApp;
                    }

                    Int32 App::Run() noexcept
                    {
                        App& UWPApp = Instance();
                        if (UWPApp.IsRunning)
                            return static_cast<Int32>(E_ILLEGAL_METHOD_CALL);

                        UWPApp.ResetView();
                        UWPApp.ErrorCode = 0;
                        UWPApp.IsRunning = true;

                        try
                        {
                            winrt::Windows::ApplicationModel::Core::CoreApplication::Run(
                                winrt::make<ViewProvider>()
                            );
                        }
                        catch (const winrt::hresult_error& Error)
                        {
                            UWPApp.ErrorCode = static_cast<Int32>(Error.code());
                        }
                        catch (...)
                        {
                            UWPApp.ErrorCode = static_cast<Int32>(E_FAIL);
                        }

                        UWPApp.ResetView();
                        UWPApp.IsRunning = false;
                        return UWPApp.ErrorCode;
                    }

                    void App::SetApplicationView(const CoreApplicationView& View) noexcept
                    {
                        NativeView = View;
                    }

                    void App::SetCoreWindow(const CoreWindow& Window) noexcept
                    {
                        NativeWindow = Window;
                    }

                    void App::ActivateCoreWindow() noexcept
                    {
                        if (!NativeWindow)
                        {
                            ErrorCode = static_cast<Int32>(E_UNEXPECTED);
                            return;
                        }

                        try
                        {
                            NativeWindow.Activate();
                        }
                        catch (const winrt::hresult_error& Error)
                        {
                            ErrorCode = static_cast<Int32>(Error.code());
                        }
                        catch (...)
                        {
                            ErrorCode = static_cast<Int32>(E_FAIL);
                        }
                    }

                    void App::ProcessEvents() noexcept
                    {
                        if (!NativeWindow)
                        {
                            ErrorCode = static_cast<Int32>(E_UNEXPECTED);
                            return;
                        }

                        try
                        {
                            NativeWindow.Dispatcher().ProcessEvents(
                                winrt::Windows::UI::Core::CoreProcessEventsOption::ProcessUntilQuit
                            );
                        }
                        catch (const winrt::hresult_error& Error)
                        {
                            ErrorCode = static_cast<Int32>(Error.code());
                        }
                        catch (...)
                        {
                            ErrorCode = static_cast<Int32>(E_FAIL);
                        }
                    }

                    void App::ResetView() noexcept
                    {
                        NativeWindow = nullptr;
                        NativeView = nullptr;
                    }

                    CoreApplicationView App::GetApplicationView() const noexcept
                    {
                        return NativeView;
                    }

                    CoreWindow App::GetCoreWindow() const noexcept
                    {
                        return NativeWindow;
                    }

                    Bool App::IsReady() const noexcept
                    {
                        return NativeView && NativeWindow;
                    }

                    Int32 App::GetErrorCode() const noexcept
                    {
                        return ErrorCode;
                    }
                }
            }
        }
    }
}
