#include "Empire.Platform/Platform.App/App.Cocoa/Cocoa.App.hpp"

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
                    App& App::Instance() noexcept
                    {
                        static App CocoaApp;
                        return CocoaApp;
                    }

                    Bool App::Initialize() noexcept
                    {
                        if (![NSThread isMainThread])
                        {
                            ErrorCode = -1;
                            return false;
                        }

                        @autoreleasepool
                        {
                            NativeApplication = [NSApplication sharedApplication];
                        }

                        if (NativeApplication == nil)
                        {
                            ErrorCode = -2;
                            return false;
                        }

                        ErrorCode = 0;
                        return true;
                    }

                    NSApplication* App::GetNativeHandle() const noexcept
                    {
                        return NativeApplication;
                    }

                    Bool App::IsReady() const noexcept
                    {
                        return NativeApplication != nil;
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
