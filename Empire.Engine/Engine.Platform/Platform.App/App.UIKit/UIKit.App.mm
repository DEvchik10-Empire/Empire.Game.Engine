#include "Empire.Engine/Engine.Platform/Platform.App/App.UIKit/UIKit.App.hpp"

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
                        App& App::Instance() noexcept
                        {
                            static App UIKitApp;
                            return UIKitApp;
                        }

                        UIApplication* App::GetNativeHandle() const noexcept
                        {
                            if (![NSThread isMainThread])
                                return nil;

                            return [UIApplication sharedApplication];
                        }

                        Bool App::IsReady() const noexcept
                        {
                            return GetNativeHandle() != nil;
                        }

                        Int32 App::GetErrorCode() const noexcept
                        {
                            if (![NSThread isMainThread])
                                return -1;

                            return GetNativeHandle() != nil ? 0 : -2;
                        }
                    }
                }
            }
        }
    }
}
