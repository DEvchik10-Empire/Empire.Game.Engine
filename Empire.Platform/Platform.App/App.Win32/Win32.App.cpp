#include "Empire.Platform/Platform.App/App.Win32/Win32.App.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpirePlatform
        {
            namespace PlatformApp
            {
                namespace AppWin32
                {
                    App& App::Instance() noexcept
                    {
                        static App Win32App;
                        return Win32App;
                    }

                    App::App() noexcept
                    {
                        NativeInstance = GetModuleHandleW(nullptr);

                        if (NativeInstance == nullptr)
                        {
                            ErrorCode = static_cast<UInt32>(GetLastError());
                            if (ErrorCode == 0)
                                ErrorCode = static_cast<UInt32>(ERROR_INVALID_HANDLE);
                            return;
                        }

                        ErrorCode = 0;
                    }

                    App::~App() = default;

                    HINSTANCE App::GetNativeHandle() const noexcept
                    {
                        return NativeInstance;
                    }

                    Bool App::IsReady() const noexcept
                    {
                        return NativeInstance != nullptr;
                    }

                    UInt32 App::GetErrorCode() const noexcept
                    {
                        return ErrorCode;
                    }
                }
            }
        }
    }
}
