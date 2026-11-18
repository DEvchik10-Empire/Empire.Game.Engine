#include "Empire.Engine/Engine.Platform/Platform.App/App.XLib/XLib.App.hpp"

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
                        App& App::Instance() noexcept
                        {
                            static App XLibApp;
                            return XLibApp;
                        }

                        App::App() noexcept
                        {
                            // Xlib thread support must be initialized before opening a display.
                            if (XInitThreads() == 0)
                            {
                                ErrorCode = -1;
                                return;
                            }

                            NativeDisplay = XOpenDisplay(nullptr);

                            if (NativeDisplay == nullptr)
                            {
                                ErrorCode = -1;
                                return;
                            }

                            ErrorCode = 0;
                        }

                        App::~App()
                        {
                            if (NativeDisplay != nullptr)
                            {
                                (void)XCloseDisplay(NativeDisplay);
                                NativeDisplay = nullptr;
                            }
                        }

                        Display* App::GetNativeHandle() const noexcept
                        {
                            return NativeDisplay;
                        }

                        Int32 App::GetDefaultScreen() const noexcept
                        {
                            return NativeDisplay != nullptr ? DefaultScreen(NativeDisplay) : -1;
                        }

                        XWindow App::GetRootWindow() const noexcept
                        {
                            return NativeDisplay != nullptr ? DefaultRootWindow(NativeDisplay) : 0;
                        }

                        Bool App::IsReady() const noexcept
                        {
                            return NativeDisplay != nullptr;
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
}
