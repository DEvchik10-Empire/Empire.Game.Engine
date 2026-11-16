#include "Empire.Platform/Platform.App/App.XCB/XCB.App.hpp"

namespace DEvchik10Empire
{
    namespace EmpireGameEngine
    {
        namespace EmpirePlatform
        {
            namespace PlatformApp
            {
                namespace AppXCB
                {
                    App& App::Instance() noexcept
                    {
                        static App XCBApp;
                        return XCBApp;
                    }

                    App::App() noexcept
                    {
                        NativeConnection = xcb_connect(nullptr, nullptr);

                        if (NativeConnection == nullptr)
                        {
                            ErrorCode = -1;
                            return;
                        }

                        ErrorCode = xcb_connection_has_error(NativeConnection);

                        if (ErrorCode != 0)
                        {
                            xcb_disconnect(NativeConnection);
                            NativeConnection = nullptr;
                        }
                    }

                    App::~App()
                    {
                        if (NativeConnection != nullptr)
                        {
                            xcb_disconnect(NativeConnection);
                            NativeConnection = nullptr;
                        }
                    }

                    xcb_connection_t* App::GetNativeHandle() const noexcept
                    {
                        return NativeConnection;
                    }

                    Bool App::IsReady() const noexcept
                    {
                        return NativeConnection != nullptr &&
                            xcb_connection_has_error(NativeConnection) == 0;
                    }

                    Int32 App::GetErrorCode() const noexcept
                    {
                        if (NativeConnection == nullptr)
                        {
                            return ErrorCode;
                        }

                        return xcb_connection_has_error(NativeConnection);
                    }
                }
            }
        }
    }
}
