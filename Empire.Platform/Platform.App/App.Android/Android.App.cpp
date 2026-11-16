#include "Empire.Platform/Platform.App/App.Android/Android.App.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpirePlatform
        {
            namespace PlatformApp
            {
                namespace AppAndroid
                {
                    App& App::Instance() noexcept
                    {
                        static App AndroidApp;
                        return AndroidApp;
                    }

                    Bool App::Initialize(::android_app* Application) noexcept
                    {
                        if (Application == nullptr)
                        {
                            ErrorCode = -1;
                            return false;
                        }

                        if (NativeApplication != nullptr && NativeApplication != Application)
                        {
                            ErrorCode = -2;
                            return false;
                        }

                        NativeApplication = Application;
                        ErrorCode = 0;
                        return true;
                    }

                    void App::Shutdown() noexcept
                    {
                        NativeApplication = nullptr;
                        ErrorCode = 0;
                    }

                    ::android_app* App::GetNativeHandle() const noexcept
                    {
                        return NativeApplication;
                    }

                    Bool App::IsReady() const noexcept
                    {
                        return NativeApplication != nullptr;
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
